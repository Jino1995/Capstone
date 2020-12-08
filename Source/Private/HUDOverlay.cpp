// Fill out your copyright notice in the Description page of Project Settings.


#include "HUDOverlay.h"
#include "CanvasPanel.h"
#include "WidgetLayoutLibrary.h"
#include "CanvasPanelSlot.h"
#include "WidgetBlueprintLibrary.h"
#include "DragWidget.h"
#include "InventorySlot.h"
#include "PlayerCharacter.h"
#include "LevelSlot.h"
#include "CharacterStateWidget.h"
#include "ABSaveGame.h"

void UHUDOverlay::NativeConstruct()
{
	Super::NativeConstruct();

	InventoryWindow = Cast<UInventoryWindow>(GetWidgetFromName(FName("Inventory")));
	LootWindow = Cast<ULootWindow>(GetWidgetFromName(FName("LootContainer")));
	TradeWindow = Cast<UTraderWindow>(GetWidgetFromName(FName("Trade")));
	TrainerWindow = Cast<UTrainerWindow>(GetWidgetFromName(FName("TrainerWindow_BP")));
	StateWidget = Cast<UCharacterStateWidget>(GetWidgetFromName(FName("CharacterState")));
	QuickSlot = Cast<UQuickSlot>(GetWidgetFromName(FName("QuickSlot_BP")));

	InventoryWindow->SetVisibility(ESlateVisibility::Hidden);
	LootWindow->SetVisibility(ESlateVisibility::Hidden);
	TradeWindow->SetVisibility(ESlateVisibility::Hidden);
	TrainerWindow->SetVisibility(ESlateVisibility::Hidden);
}

bool UHUDOverlay::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);

	if (Cast<UInventoryWindow>(Cast<UDragWidget>(InOperation)->WidgetToDrag))
	{
		FVector2D NewP = InGeometry.AbsoluteToLocal(InDragDropEvent.GetScreenSpacePosition()) - Cast<UDragWidget>(InOperation)->DropWindowOffset;
	
		NewP.X = (NewP.X > 1800.f) ? 1800.f : (NewP.X < 5) ? 5.f : NewP.X;
		NewP.Y = (NewP.Y > 500.f) ? 500.f : (NewP.Y < -15) ? -15.f : NewP.Y;
		
		Cast<UCanvasPanel>(GetWidgetFromName(FName("Canvas")))->AddChild(Cast<UInventoryWindow>(Cast<UDragWidget>(InOperation)->WidgetToDrag));
		UWidgetLayoutLibrary::SlotAsCanvasSlot(InventoryWindow)->SetPosition(NewP);
	}
	else if (Cast<ULootWindow>(Cast<UDragWidget>(InOperation)->WidgetToDrag))
	{
		FVector2D NewP = InGeometry.AbsoluteToLocal(InDragDropEvent.GetScreenSpacePosition()) - Cast<UDragWidget>(InOperation)->DropWindowOffset;

		NewP.X = (NewP.X > 1800.f) ? 1800.f : (NewP.X < 5) ? 5.f : NewP.X;
		NewP.Y = (NewP.Y > 500.f) ? 500.f : (NewP.Y < -15) ? -15.f : NewP.Y;

		Cast<UCanvasPanel>(GetWidgetFromName(FName("Canvas")))->AddChild(Cast<ULootWindow>(Cast<UDragWidget>(InOperation)->WidgetToDrag));
		UWidgetLayoutLibrary::SlotAsCanvasSlot(LootWindow)->SetPosition(NewP);
	}
	else if (Cast<UInventorySlot>(Cast<UDragWidget>(InOperation)->WidgetToDrag) && !Cast<AABPlayerController>(GetOwningPlayer())->IsOpenTradeWindow() && !Cast<APlayerCharacter>(GetOwningPlayerPawn())->GetCharacterMovement()->IsFalling())
	{
		if (Cast<APlayerCharacter>(GetOwningPlayerPawn())->GetTargetItemPot())
		{
			if (Cast<AABPlayerController>(GetOwningPlayer())->IsOpenLootWindow())
			{
				GetLootWindow()->AddItem(Cast<UInventorySlot>(Cast<UDragWidget>(InOperation)->WidgetToDrag)->GetItemName(), Cast<UInventorySlot>(Cast<UDragWidget>(InOperation)->WidgetToDrag)->GetItemAmount());
				Cast<UInventorySlot>(Cast<UDragWidget>(InOperation)->WidgetToDrag)->SetItem(FName("Default"));
			}
			else
			{
				Cast<APlayerCharacter>(GetOwningPlayerPawn())->GetTargetItemPot()->AddItem(Cast<UInventorySlot>(Cast<UDragWidget>(InOperation)->WidgetToDrag)->GetItemName(), Cast<UInventorySlot>(Cast<UDragWidget>(InOperation)->WidgetToDrag)->GetItemAmount());
				Cast<UInventorySlot>(Cast<UDragWidget>(InOperation)->WidgetToDrag)->SetItem(FName("Default"));
			}
		}
		else
		{
			FVector CharacterLocation = Cast<APlayerCharacter>(GetOwningPlayerPawn())->GetCharacterMovement()->GetActorLocation();
			AItemPot* NewItemPot = GetWorld()->SpawnActor<AItemPot>(FVector(CharacterLocation.X, CharacterLocation.Y, CharacterLocation.Z-90), FRotator::ZeroRotator);
			NewItemPot->AddItem(Cast<UInventorySlot>(Cast<UDragWidget>(InOperation)->WidgetToDrag)->GetItemName(), Cast<UInventorySlot>(Cast<UDragWidget>(InOperation)->WidgetToDrag)->GetItemAmount());
			Cast<UInventorySlot>(Cast<UDragWidget>(InOperation)->WidgetToDrag)->SetItem(FName("Default"));
		}
	}

	return true;
}

void UHUDOverlay::BindCharacterStat(class UCharacterStatComponent* StatComp)
{
	GetInventoryWindow()->GetLvSlot()->BindCharacterStat(StatComp);
	TrainerWindow->BindCharacterStat(StatComp);
	StateWidget->BindCharacterStat(StatComp);
}

void UHUDOverlay::OpenLootWindow(class AItemPot* TargetItemPot) {
	LootWindow->InitInventory(TargetItemPot);
	LootWindow->SetVisibility(ESlateVisibility::Visible);
}

void UHUDOverlay::CloseLootWindow() {
	if (LootWindow)
	{
		LootWindow->ClearInventory();
		LootWindow->SetVisibility(ESlateVisibility::Hidden);
	}
}
void UHUDOverlay::OpenTradeWindow(TArray<FName> TraderItems)
{
	TradeWindow->FillSlots(TraderItems);
	TradeWindow->SetVisibility(ESlateVisibility::Visible);
}

void UHUDOverlay::CloseTradeWindow()
{
	TradeWindow->SetVisibility(ESlateVisibility::Hidden);
}

void UHUDOverlay::OpenTrainerWindow()
{
	TrainerWindow->SetVisibility(ESlateVisibility::Visible);
}

void UHUDOverlay::CloseTrainerWindow()
{
	TrainerWindow->SetVisibility(ESlateVisibility::Hidden);
}

float UHUDOverlay::CheckPotion()
{
	return QuickSlot->UsePotion();
}

void UHUDOverlay::FillPotion()
{
	QuickSlot->FillPotion();
}

void UHUDOverlay::UpgradePotionNum()
{
	if (GetInventoryWindow()->CheckDecrementMoney(1000))
	{
		QuickSlot->MaxAmountLvUp();
	}
}

void UHUDOverlay::UpgradePotionForce()
{
	if (GetInventoryWindow()->CheckDecrementMoney(1000))
	{
		QuickSlot->ForceLvUp();
	}
}

void UHUDOverlay::SaveCurrentStatus(UABSaveGame* NewSave)
{
	InventoryWindow->SaveItems(NewSave);
	NewSave->PotionStatus.Reinforce_Amount = QuickSlot->GetMaxAmount();
	NewSave->PotionStatus.Reinforce_Force = QuickSlot->GetCurrentForce();
}

void UHUDOverlay::LoadStatus(UABSaveGame* LoadData)
{
	InventoryWindow->LoadItems(LoadData);
	QuickSlot->SetMaxAmount(LoadData->PotionStatus.Reinforce_Amount);
	QuickSlot->FillPotion();
	QuickSlot->SetForceLv(LoadData->PotionStatus.Reinforce_Force);
}