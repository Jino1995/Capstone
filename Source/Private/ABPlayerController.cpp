// Fill out your copyright notice in the Description page of Project Settings.


#include "ABPlayerController.h"
#include "UniformGridPanel.h"
#include "Blueprint/UserWidget.h"
#include "CanvasPanel.h"
#include "UserWidget.h"
#include "Image.h"
#include "ABGameInstance.h"
#include "InventorySlot.h"
#include "CanvasPanelSlot.h"
#include "WidgetLayoutLibrary.h"
#include "InventoryWindow.h"
#include "HUDOverlay.h"
#include "PlayerCharacter.h"
#include "EquipmentSlot.h"
#include "ItemPot.h"
#include "LevelSlot.h"
#include "ABSaveGame.h"
#include "Npc.h"
#include "SaveWindow.h"
#include "ABGameMode.h"
#include "CharacterStatComponent.h"

AABPlayerController::AABPlayerController()
{
	bInventoryWindow = false;
	bMenuWindow = false;
	bSaveMenu = false;
	bLootWindow = false;
	bTradeWindow = false;
	bTrainerWindow = false;
}

void AABPlayerController::BeginPlay()
{
	Super::BeginPlay();

	SetInputMode(GameInputMode);
	SetPause(false);

	CharacterStat = Cast<APlayerCharacter>(GetPawn())->GetCharacterStat();

	if (HUDOverlayAsset)
	{
		HUDOverlay = CreateWidget<UHUDOverlay>(this, HUDOverlayAsset);
		HUDOverlay->AddToViewport();
		HUDOverlay->SetVisibility(ESlateVisibility::Visible);
		HUDOverlay->BindCharacterStat(CharacterStat);
	}

	if (WPauseMenu)
	{
		PauseMenu = CreateWidget<UUserWidget>(this, WPauseMenu);
		if (PauseMenu)
		{
			PauseMenu->AddToViewport();
			PauseMenu->SetVisibility(ESlateVisibility::Hidden);
		}
	}
	
	if (WDeadMenu)
	{
		DeadMenu = CreateWidget<UUserWidget>(this, WDeadMenu);
		if (DeadMenu)
		{
			DeadMenu->AddToViewport();
			DeadMenu->SetVisibility(ESlateVisibility::Hidden);
		}
	}

	if (WSaveMenu)
	{
		SaveMenu = CreateWidget<USaveWindow>(this, WSaveMenu);
		if (SaveMenu)
		{
			SaveMenu->AddToViewport();
			SaveMenu->SetVisibility(ESlateVisibility::Hidden);
		}
	}

	Cast<AABGameMode>(UGameplayStatics::GetGameMode(GetWorld()))->OnStageClear.AddLambda([this]()->void {
		if (WStageClear)
		{
			StageClearWidget = CreateWidget<UUserWidget>(this, WStageClear);
			if (StageClearWidget)
			{
				StageClearWidget->AddToViewport();
				StageClearWidget->SetVisibility(ESlateVisibility::Visible);
			}
		}
	});

	Cast<AABGameMode>(UGameplayStatics::GetGameMode(GetWorld()))->OnGameClear.AddLambda([this]()->void {
		if (WGameClear)
		{
			GameClearWidget = CreateWidget<UUserWidget>(this, WGameClear);
			if (GameClearWidget)
			{
				SetInputMode(UIInputMode);
				bShowMouseCursor = true;
				GameClearWidget->AddToViewport();
				GameClearWidget->SetVisibility(ESlateVisibility::Visible);
			}
		}
	});

	LoadGame();
	
	if (UGameplayStatics::GetCurrentLevelName(GetWorld()).Contains(FString("Stage")))
	{
		if (WStageStart)
		{
			StageStartWidget = CreateWidget<UUserWidget>(this, WStageStart);
			if (StageStartWidget)
			{
				SetInputMode(UIInputMode);
				StageStartWidget->AddToViewport();
				StageStartWidget->SetVisibility(ESlateVisibility::Visible);
			}
		}
	}
}

void AABPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AABPlayerController::OpenInventoryWindow()
{
	if (bInventoryWindow)
	{
		HUDOverlay->CloseInventoryWindow();
		bInventoryWindow = false;
	}
	else
	{
		HUDOverlay->OpenInventoryWindow();
		bInventoryWindow = true;
	}
}

void AABPlayerController::OpenMenuWindow()
{
	if (bMenuWindow)
	{
		PauseMenu->SetVisibility(ESlateVisibility::Hidden);
		bMenuWindow = false;
		SetPause(false);
		bShowMouseCursor = false;

	}
	else
	{
		PauseMenu->SetVisibility(ESlateVisibility::Visible);
		bMenuWindow = true;
		SetPause(true);
		bShowMouseCursor = true;
	}
}

void AABPlayerController::OpenLootWindow(AItemPot* TargetItemPot)
{
	HUDOverlay->OpenLootWindow(TargetItemPot);
	bLootWindow = true;
}

void AABPlayerController::CloseLootWindow()
{
	if (bLootWindow)
	{
		if (HUDOverlay != nullptr)
		{
			HUDOverlay->CloseLootWindow();
			bLootWindow = false;
		}
	}
}

void AABPlayerController::OpenSaveMenu()
{
	if (bSaveMenu)
	{
		SetInputMode(GameInputMode);
		SaveMenu->SetVisibility(ESlateVisibility::Hidden);
		bSaveMenu = false;
		SetPause(false);
		bShowMouseCursor = false;
	}
	else
	{
		if (bInventoryWindow) OpenInventoryWindow();
		if (bLootWindow) CloseLootWindow();

		HUDOverlay->FillPotion();
		SetInputMode(UIInputMode);
		SaveMenu->SetVisibility(ESlateVisibility::Visible);
		bSaveMenu = true;
		SetPause(true);
		bShowMouseCursor = true;
	}
}

void AABPlayerController::OpenDeadMenu()
{
	DeadMenu->SetVisibility(ESlateVisibility::Visible);
	SetInputMode(UIInputMode);
	bShowMouseCursor = true;
}

void AABPlayerController::OpenTradeWindow(ANpc* TraderTarget)
{
	if (!bTradeWindow)
	{
		HUDOverlay->OpenTradeWindow(TraderTarget->GetSellingItems());
		bTradeWindow = true;
		bInventoryWindow = false;
		OpenInventoryWindow();

		SetInputMode(UIInputMode);
		bShowMouseCursor = true;
	}
}

void AABPlayerController::CloseTradeWindow()
{
	if (bTradeWindow)
	{
		HUDOverlay->CloseTradeWindow();
		bTradeWindow = false;
		SetInputMode(GameInputMode);
		OpenInventoryWindow();
		bShowMouseCursor = false;
	}

}

void AABPlayerController::OpenTrainerWindow()
{
	if (!bTrainerWindow)
	{
		HUDOverlay->OpenTrainerWindow();
		bTrainerWindow = true;
		SetInputMode(UIInputMode);
		bShowMouseCursor = true;
	}
}

void AABPlayerController::CloseTrainerWindow()
{
	if (bTrainerWindow)
	{
		HUDOverlay->CloseTrainerWindow();
		bTrainerWindow = false;
		SetInputMode(GameInputMode);
		bShowMouseCursor = false;
	}
}

void AABPlayerController::RequestIncMoney(int32 IncMoney)
{
	HUDOverlay->GetInventoryWindow()->IncrementMoney(IncMoney);
}

bool AABPlayerController::EquipItem(UInventorySlot* EquippedItem)
{
	UABGameInstance* ABGameInstance = Cast<UABGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	FItemData* NewItemData = ABGameInstance->GetItemData(EquippedItem->GetItemName());
	if (HUDOverlay->GetInventoryWindow()->ChangeEquipItem(EquippedItem))
	{
		switch (NewItemData->GetEquipmentSlot())
		{
		case EEquipmentSlot::Head:
			Cast<APlayerCharacter>(GetPawn())->EquipHead(NewItemData->GetEquipMesh());
			break;
		case EEquipmentSlot::Chest:
			Cast<APlayerCharacter>(GetPawn())->EquipTop(NewItemData->GetEquipMesh());
			break;
		case EEquipmentSlot::Legs:
			Cast<APlayerCharacter>(GetPawn())->EquipBot(NewItemData->GetEquipMesh());
			break;
		case EEquipmentSlot::RightHand:
			if (NewItemData->GetWeaponType() == EWeaponType::OneHanded)
			{
				Cast<APlayerCharacter>(GetPawn())->EquipOneHandedWeapon(NewItemData->GetWeaponBP());
			}
			else if (NewItemData->GetWeaponType() == EWeaponType::TwoHanded)
			{
				Cast<APlayerCharacter>(GetPawn())->EquipTwoHandedWeapon(NewItemData->GetWeaponBP());
			}
			break;
		case EEquipmentSlot::LeftHand:
			Cast<APlayerCharacter>(GetPawn())->EquipShield(NewItemData->GetShieldBP());
			break;
		}
		return true;
	}
	else
	{
		return false;
	}
}

void AABPlayerController::EquipItemForName(FName ItemName)
{
	UABGameInstance* ABGameInstance = Cast<UABGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	FItemData* NewItemData = ABGameInstance->GetItemData(ItemName);
	HUDOverlay->GetInventoryWindow()->EquipItem(ItemName);

	switch (NewItemData->GetEquipmentSlot())
	{
	case EEquipmentSlot::Head:
		Cast<APlayerCharacter>(GetPawn())->EquipHead(NewItemData->GetEquipMesh());
		break;
	case EEquipmentSlot::Chest:
		Cast<APlayerCharacter>(GetPawn())->EquipTop(NewItemData->GetEquipMesh());
		break;
	case EEquipmentSlot::Legs:
		Cast<APlayerCharacter>(GetPawn())->EquipBot(NewItemData->GetEquipMesh());
		break;
	case EEquipmentSlot::RightHand:
		if (NewItemData->GetWeaponType() == EWeaponType::OneHanded)
		{
			Cast<APlayerCharacter>(GetPawn())->EquipOneHandedWeapon(NewItemData->GetWeaponBP());
		}
		else if (NewItemData->GetWeaponType() == EWeaponType::TwoHanded)
		{
			Cast<APlayerCharacter>(GetPawn())->EquipTwoHandedWeapon(NewItemData->GetWeaponBP());
		}
		break;
	case EEquipmentSlot::LeftHand:
		Cast<APlayerCharacter>(GetPawn())->EquipShield(NewItemData->GetShieldBP());
		break;
	}
}

bool AABPlayerController::UnEquipItem(FName ItemName)
{
	UABGameInstance* ABGameInstance = Cast<UABGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	FItemData* NewItemData = ABGameInstance->GetItemData(ItemName);

	if (HUDOverlay->GetInventoryWindow()->UnEquipItem(NewItemData->GetEquipmentSlot()))
	{
		switch (NewItemData->GetEquipmentSlot())
		{
		case EEquipmentSlot::Head:
			Cast<APlayerCharacter>(GetPawn())->UnEquipHead();
			break;
		case EEquipmentSlot::Chest:
			Cast<APlayerCharacter>(GetPawn())->UnEquipTop();
			break;
		case EEquipmentSlot::Legs:
			Cast<APlayerCharacter>(GetPawn())->UnEquipBot();
			break;
		case EEquipmentSlot::RightHand:
			if (NewItemData->GetWeaponType() == EWeaponType::OneHanded)
			{
				Cast<APlayerCharacter>(GetPawn())->UnEquipOneHandedWeapon();
			}
			else if (NewItemData->GetWeaponType() == EWeaponType::TwoHanded)
			{
				Cast<APlayerCharacter>(GetPawn())->UnEquipTwoHandedWeapon();
			}
			break;
		case EEquipmentSlot::LeftHand:
			Cast<APlayerCharacter>(GetPawn())->UnEquipShield();
			break;
		case EEquipmentSlot::BothHands:
			break;
		}
		return true;
	}
	else
	{
		return false;
	}
}

void AABPlayerController::SaveGame()
{
	UABSaveGame* NewSave = NewObject<UABSaveGame>();
	
	Cast<APlayerCharacter>(GetPawn())->GetCharacterStat()->SavePlayerStats(NewSave);
	HUDOverlay->SaveCurrentStatus(NewSave);
	
	FString LvName = UGameplayStatics::GetCurrentLevelName(GetWorld());
	NewSave->LevelName = LvName;
	NewSave->Position = Cast<APlayerCharacter>(GetPawn())->GetActorLocation();
	NewSave->Rotation = Cast<APlayerCharacter>(GetPawn())->GetActorRotation();

	UGameplayStatics::SaveGameToSlot(NewSave, FString("First"), 0);
}

void AABPlayerController::NextStageSaveGame()
{
	UABSaveGame* NewSave = NewObject<UABSaveGame>();

	Cast<APlayerCharacter>(GetPawn())->GetCharacterStat()->SavePlayerStats(NewSave);
	HUDOverlay->SaveCurrentStatus(NewSave);
	NewSave->PotionStatus.CurrentAmount = HUDOverlay->GetQuickSlot()->GetCurrentAmount();
	FString LvName = UGameplayStatics::GetCurrentLevelName(GetWorld());
	NewSave->LevelName = LvName;
	UGameplayStatics::SaveGameToSlot(NewSave, FString("Second"), 0);
}

void AABPlayerController::LoadGame()
{
	auto PreStageData = Cast<UABSaveGame>(UGameplayStatics::LoadGameFromSlot(FString("Second"), 0));
	auto LoadGameData = Cast<UABSaveGame>(UGameplayStatics::LoadGameFromSlot(FString("First"), 0));
	if (PreStageData != nullptr)
	{
		Cast<APlayerCharacter>(GetPawn())->GetCharacterStat()->LoadPlayerStats(PreStageData);
		HUDOverlay->LoadStatus(PreStageData);
		HUDOverlay->GetQuickSlot()->SetCurrentAmount(PreStageData->PotionStatus.CurrentAmount);
		UGameplayStatics::DeleteGameInSlot(FString("Second"), 0);
	}
	else if (LoadGameData != nullptr)
	{
		Cast<APlayerCharacter>(GetPawn())->GetCharacterStat()->LoadPlayerStats(LoadGameData);
		HUDOverlay->LoadStatus(LoadGameData);
		Cast<APlayerCharacter>(GetPawn())->SetActorLocation(LoadGameData->Position);
		Cast<APlayerCharacter>(GetPawn())->SetActorRotation(LoadGameData->Rotation);
	}
	else
	{
		HUDOverlay->GetInventoryWindow()->InitInventory();
		Cast<APlayerCharacter>(GetPawn())->GetCharacterStat()->RenewalStat();
	}
}

void AABPlayerController::RequestPurchaseItem(FName ItemName)
{
	GetHUDOverlay()->GetInventoryWindow()->PurchaceItem(ItemName);
}


float AABPlayerController::RequestPotion()
{
	return HUDOverlay->CheckPotion();
}

void AABPlayerController::UpgradePotionAmount()
{
	HUDOverlay->UpgradePotionNum();
}

void AABPlayerController::UpgradePotionForce()
{
	HUDOverlay->UpgradePotionForce();
}

bool AABPlayerController::CheckUpEnoughMoney(int32 Needful)
{
	return HUDOverlay->GetInventoryWindow()->CheckDecrementMoney(Needful);
}

void AABPlayerController::CloseStageStartWidget()
{
	SetInputMode(GameInputMode);
}