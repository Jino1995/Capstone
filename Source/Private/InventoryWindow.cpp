// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryWindow.h"
#include "InventorySlot.h"
#include "UniformGridPanel.h"
#include "ABGameInstance.h"
#include "Engine.h"
#include "WidgetBlueprintLibrary.h"
#include "WidgetLayoutLibrary.h"
#include "DragWidget.h"
#include "UserWidget.h"
#include "EquipmentSlot.h"
#include "HUDOverlay.h"
#include "ABPlayerController.h"
#include "LevelSlot.h"
#include "ABSaveGame.h"
#include "PlayerCharacter.h"
#include "TextBlock.h"

void UInventoryWindow::NativeConstruct()
{
	Super::NativeConstruct();

	Inventory = Cast<UUniformGridPanel>(GetWidgetFromName(FName("InventoryGridPanel")));

	MoneyTxt = Cast<UTextBlock>(GetWidgetFromName(FName("MoneyText")));

	HeadSlot = Cast<UEquipmentSlot>(GetWidgetFromName(FName("Head")));
	HeadSlot->SetItem(FName("Default"));
	HeadSlot->SetSlotType(EEquipmentSlot::Head, this);

	TopSlot = Cast<UEquipmentSlot>(GetWidgetFromName(FName("Top")));
	TopSlot->SetItem(FName("Default"));
	TopSlot->SetSlotType(EEquipmentSlot::Chest, this);

	BotSlot = Cast<UEquipmentSlot>(GetWidgetFromName(FName("Bot")));
	BotSlot->SetItem(FName("Default"));
	BotSlot->SetSlotType(EEquipmentSlot::Legs, this);

	WeaponSlot = Cast<UEquipmentSlot>(GetWidgetFromName(FName("Weapon")));
	WeaponSlot->SetItem(FName("Default"));
	WeaponSlot->SetSlotType(EEquipmentSlot::RightHand, this);

	ShieldSlot = Cast<UEquipmentSlot>(GetWidgetFromName(FName("Shield")));
	ShieldSlot->SetItem(FName("Default"));
	ShieldSlot->SetSlotType(EEquipmentSlot::LeftHand, this);

	LvSlot = Cast<ULevelSlot>(GetWidgetFromName(FName("LevelSlot")));

	SetMoney(0);
}

void UInventoryWindow::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);

	FVector2D DragWindowOffset = InGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());

	UDragWidget* oper = NewObject<UDragWidget>();
	oper->DefaultDragVisual = this;
	oper->WidgetToDrag = this;
	oper->DropWindowOffset = DragWindowOffset;
	oper->Pivot = EDragPivot::CenterCenter;
	OutOperation = oper;
	
	RemoveFromParent();
}

bool UInventoryWindow::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
	
	return true;
}

FReply UInventoryWindow::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FEventReply reply;
	reply.NativeReply = Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);

	if (InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton) == true)
	{
		reply = UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton);
	}

	return reply.NativeReply;
}

void UInventoryWindow::InitInventory()
{
	Cast<UInventorySlot>(Inventory->GetChildAt(0))->SetItem(TEXT("목검"), 1);
}

bool UInventoryWindow::StoredInEmptySlot(FName ItemName, int32 Amount)
{
	UABGameInstance* ABGameInstance = Cast<UABGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	FItemData* ItemData = ABGameInstance->GetItemData(ItemName);
	int32 MaxStack = ItemData->GetMaxStackSize();
	
	for (int i = 0; i < 32; i++)
	{
		if (Cast<UInventorySlot>(Inventory->GetChildAt(i))->GetItemName() == ItemName && ItemData->GetIsStackable())
		{
			if (Cast<UInventorySlot>(Inventory->GetChildAt(i))->GetItemAmount() + Amount <= MaxStack)
			{
				Cast<UInventorySlot>(Inventory->GetChildAt(i))->AddAmount(Amount);
				return true;
			}
			else
			{
				Amount -= (MaxStack - Cast<UInventorySlot>(Inventory->GetChildAt(i))->GetItemAmount());
				Cast<UInventorySlot>(Inventory->GetChildAt(i))->SetItemAmount(MaxStack);
			}
		}
		else if (Cast<UInventorySlot>(Inventory->GetChildAt(i))->GetItemName() == FName("Default"))
		{
			Cast<UInventorySlot>(Inventory->GetChildAt(i))->SetItem(ItemName, Amount);
			return true;
		}
	}
	return false;
}

void UInventoryWindow::SetMoney(int32 NewMoney)
{
	CurrentMoney = NewMoney;
	MoneyTxt->SetText(FText::FromString(FString::FromInt(CurrentMoney)));
}

void UInventoryWindow::IncrementMoney(int32 IncMoney)
{
	CurrentMoney += IncMoney;
	MoneyTxt->SetText(FText::FromString(FString::FromInt(CurrentMoney)));
}

void UInventoryWindow::DecrementMoney(int32 DecMoney)
{
	CurrentMoney -= DecMoney;
	MoneyTxt->SetText(FText::FromString(FString::FromInt(CurrentMoney)));
}

bool UInventoryWindow::CheckDecrementMoney(int32 DecMoney)
{
	if (CurrentMoney >= DecMoney)
	{
		DecrementMoney(DecMoney);
		return true;
	}
	else 
	{
		return false;
	}
}

bool UInventoryWindow::ChangeEquipItem(class UInventorySlot* NewItem)
{
	FName CurrentItemName;
	UE_LOG(LogTemp, Error, TEXT("1.5 : %s : %d"), *(NewItem->GetItemName()).ToString(), NewItem->GetEquipSlot());
	switch (NewItem->GetEquipSlot())
	{
	case EEquipmentSlot::Head:
		UE_LOG(LogTemp, Error, TEXT("1.6 : %s : %d"), *(NewItem->GetItemName()).ToString(), NewItem->GetEquipSlot());
		Cast<AABPlayerController>(GetOwningPlayer())->UnEquipItem(HeadSlot->GetItemName());
		EquipItem(NewItem->GetItemName());
		NewItem->SetItem(FName("Default"));
		break;
	case EEquipmentSlot::Chest:
		Cast<AABPlayerController>(GetOwningPlayer())->UnEquipItem(TopSlot->GetItemName());
		EquipItem(NewItem->GetItemName());
		NewItem->SetItem(FName("Default"));
		break;
	case EEquipmentSlot::Legs:
		Cast<AABPlayerController>(GetOwningPlayer())->UnEquipItem(BotSlot->GetItemName());
		EquipItem(NewItem->GetItemName());
		NewItem->SetItem(FName("Default"));
		break;
	case EEquipmentSlot::RightHand:
		Cast<AABPlayerController>(GetOwningPlayer())->UnEquipItem(WeaponSlot->GetItemName());
		if(!EquipItem(NewItem->GetItemName())) return false;
		NewItem->SetItem(FName("Default"));
		break;
	case EEquipmentSlot::LeftHand:
		UE_LOG(LogTemp, Error, TEXT("1.6 : %s : %d"), *(ShieldSlot->GetItemName()).ToString(), NewItem->GetEquipSlot());
		Cast<AABPlayerController>(GetOwningPlayer())->UnEquipItem(ShieldSlot->GetItemName());
		if(!EquipItem(NewItem->GetItemName())) return false;
		NewItem->SetItem(FName("Default"));
		break;
	}

	return true;
}

bool UInventoryWindow::EquipItem(FName ItemName)
{
	UABGameInstance* ABGameInstance = Cast<UABGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	FItemData* ItemData = ABGameInstance->GetItemData(ItemName);
	
	switch (ItemData->GetEquipmentSlot())
	{
	case EEquipmentSlot::Head:
		HeadSlot->SetItem(ItemName);
		break;
	case EEquipmentSlot::Chest:
		TopSlot->SetItem(ItemName);
		break;
	case EEquipmentSlot::Legs:
		BotSlot->SetItem(ItemName);
		break;
	case EEquipmentSlot::RightHand:
		if (ItemData->GetWeaponType() == EWeaponType::TwoHanded)
		{
			if (ShieldSlot->GetItemName() != FName("Default"))
			{
				if (!Cast<AABPlayerController>(GetOwningPlayer())->UnEquipItem(ShieldSlot->GetItemName())) return false;
				
				//if (!UnEquipItem(EEquipmentSlot::LeftHand)) return false;
			}
		}
		WeaponSlot->SetItem(ItemName);
		break;
	case EEquipmentSlot::LeftHand:
		if (WeaponSlot->GetWeaponType() == EWeaponType::TwoHanded)
		{
			if (WeaponSlot->GetItemName() != FName("Default"))
			{
				if (!Cast<AABPlayerController>(GetOwningPlayer())->UnEquipItem(WeaponSlot->GetItemName())) return false;
				//if (!UnEquipItem(EEquipmentSlot::RightHand)) return false;
			}
		}
		ShieldSlot->SetItem(ItemName);
		break;
	}

	IncreaseStatOfEquip(ItemData);
	return true;
}

bool UInventoryWindow::UnEquipItem(EEquipmentSlot NewItemSlot)
{
	UABGameInstance* ABGameInstance = Cast<UABGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	FItemData* ItemData = nullptr;

	switch (NewItemSlot)
	{
	case EEquipmentSlot::Head:
		UE_LOG(LogTemp, Error, TEXT("??"));
		if (HeadSlot->GetItemName() == FName("Default")) return true;
		ItemData = ABGameInstance->GetItemData(HeadSlot->GetItemName());
		if (StoredInEmptySlot(HeadSlot->GetItemName()))
		{
			HeadSlot->SetItem(FName("Default"));
		}
		else
		{
			return false;
		}
		break;
	case EEquipmentSlot::Chest:
		if (TopSlot->GetItemName() == FName("Default")) return true;
		ItemData = ABGameInstance->GetItemData(TopSlot->GetItemName());
		if (StoredInEmptySlot(TopSlot->GetItemName()))
		{
			TopSlot->SetItem(FName("Default"));
		}
		else
		{
			return false;
		}
		break;
	case EEquipmentSlot::Legs:
		if (BotSlot->GetItemName() == FName("Default"))	return true;
		ItemData = ABGameInstance->GetItemData(BotSlot->GetItemName());
		if (StoredInEmptySlot(BotSlot->GetItemName()))
		{
			BotSlot->SetItem(FName("Default"));
		}
		else
		{
			return false;
		}
		break;
	case EEquipmentSlot::RightHand:
		if (WeaponSlot->GetItemName() == FName("Default"))	return true;
		ItemData = ABGameInstance->GetItemData(WeaponSlot->GetItemName());
		if (StoredInEmptySlot(WeaponSlot->GetItemName()))
		{
			WeaponSlot->SetItem(FName("Default"));
		}
		else
		{
			return false;
		}
		break;
	case EEquipmentSlot::LeftHand:
		if (ShieldSlot->GetItemName() == FName("Default"))	return true;
		ItemData = ABGameInstance->GetItemData(ShieldSlot->GetItemName());
		if (StoredInEmptySlot(ShieldSlot->GetItemName()))
		{
			ShieldSlot->SetItem(FName("Default"));
		}
		else
		{
			return false;
		}
		break;
	}

	DecreaseStatOfEquip(ItemData);
	return true;
}

void UInventoryWindow::UnEquipALLItemForSave(UABSaveGame* SaveSlot)
{
	UABGameInstance* ABGameInstance = Cast<UABGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	FItemData* ItemData = nullptr;
	if (HeadSlot->GetItemName() != FName("Default"))
	{
		ItemData = ABGameInstance->GetItemData(HeadSlot->GetItemName());
		DecreaseStatOfSaveSlot(ItemData, SaveSlot);
		SaveSlot->EquipmentItems.Add(HeadSlot->GetItemName());
	}
	if (TopSlot->GetItemName() != FName("Default"))
	{
		ItemData = ABGameInstance->GetItemData(TopSlot->GetItemName());
		DecreaseStatOfSaveSlot(ItemData, SaveSlot);
		SaveSlot->EquipmentItems.Add(TopSlot->GetItemName());
	}
	if (BotSlot->GetItemName() != FName("Default"))
	{
		ItemData = ABGameInstance->GetItemData(BotSlot->GetItemName());
		DecreaseStatOfSaveSlot(ItemData, SaveSlot);
		SaveSlot->EquipmentItems.Add(BotSlot->GetItemName());
	}
	if (WeaponSlot->GetItemName() != FName("Default"))
	{
		ItemData = ABGameInstance->GetItemData(WeaponSlot->GetItemName());
		DecreaseStatOfSaveSlot(ItemData, SaveSlot);
		SaveSlot->EquipmentItems.Add(WeaponSlot->GetItemName());
	}
	if (ShieldSlot->GetItemName() != FName("Default"))
	{
		ItemData = ABGameInstance->GetItemData(ShieldSlot->GetItemName());
		DecreaseStatOfSaveSlot(ItemData, SaveSlot);
		SaveSlot->EquipmentItems.Add(ShieldSlot->GetItemName());
	}
}

bool UInventoryWindow::PurchaceItem(FName ItemName)
{
	UABGameInstance* ABGameInstance = Cast<UABGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	FItemData* ItemData = ABGameInstance->GetItemData(ItemName);
	
	if (CurrentMoney < ItemData->GetPurchase())
	{
		return false;
	}
	
	if (!StoredInEmptySlot(ItemName))
	{
		return false;
	}
	DecrementMoney(ItemData->GetPurchase());

	return true;
}

void UInventoryWindow::SaveItems(UABSaveGame* SaveSlot)
{
	SaveSlot->Money = CurrentMoney;
	for (auto Items : Inventory->GetAllChildren())
	{
		UInventorySlot* Item = Cast<UInventorySlot>(Items);
		if (Item->GetItemType() != EItemType::None)
		{
			FInventorySlotData Data;
			Data.ItemName = Item->GetItemName();
			Data.Amount = Item->GetItemAmount();
			Data.Pos = Inventory->GetChildIndex(Items);
			SaveSlot->InventoryItems.Add(Data);
		}
	}

	UnEquipALLItemForSave(SaveSlot);
}

void UInventoryWindow::LoadItems(class UABSaveGame* LoadSlot)
{
	SetMoney(LoadSlot->Money);
	for (auto Items : LoadSlot->InventoryItems)
	{
		Cast<UInventorySlot>(Inventory->GetChildAt(Items.Pos))->SetItem(Items.ItemName, Items.Amount);
	}
	
	for (auto EItems : LoadSlot->EquipmentItems)
	{
		Cast<AABPlayerController>(GetOwningPlayer())->EquipItemForName(EItems);
	}
}

void UInventoryWindow::IncreaseStatOfEquip(FItemData* ItemData)
{
	if (ItemData != nullptr)
	{
		if (ItemData->GetHP() > 0)
		{
			Cast<APlayerCharacter>(GetOwningPlayerPawn())->GetCharacterStat()->IncreaseHP(ItemData->GetHP());
		}
		if (ItemData->GetSP() > 0)
		{
			Cast<APlayerCharacter>(GetOwningPlayerPawn())->GetCharacterStat()->IncreaseSP(ItemData->GetSP());
		}
		if (ItemData->GetAtk() > 0)
		{
			Cast<APlayerCharacter>(GetOwningPlayerPawn())->GetCharacterStat()->IncreaseATK(ItemData->GetAtk());
		}
		if (ItemData->GetDamage() > 0)
		{
			Cast<APlayerCharacter>(GetOwningPlayerPawn())->GetCharacterStat()->IncreaseAttack(ItemData->GetDamage());
		}
		if (ItemData->GetArmor() > 0)
		{
			Cast<APlayerCharacter>(GetOwningPlayerPawn())->GetCharacterStat()->IncreaseDefense(ItemData->GetArmor());
		}
	}
}
void UInventoryWindow::DecreaseStatOfEquip(FItemData* ItemData)
{
	if (ItemData != nullptr)
	{
		if (ItemData->GetHP() > 0)
		{
			Cast<APlayerCharacter>(GetOwningPlayerPawn())->GetCharacterStat()->DecreaseHP(ItemData->GetHP());
		}
		if (ItemData->GetSP() > 0)
		{
			Cast<APlayerCharacter>(GetOwningPlayerPawn())->GetCharacterStat()->DecreaseSP(ItemData->GetSP());
		}
		if (ItemData->GetAtk() > 0)
		{
			Cast<APlayerCharacter>(GetOwningPlayerPawn())->GetCharacterStat()->DecreaseATK(ItemData->GetAtk());
		}
		if (ItemData->GetDamage() > 0)
		{
			Cast<APlayerCharacter>(GetOwningPlayerPawn())->GetCharacterStat()->DecreaseDamage(ItemData->GetDamage());
		}
		if (ItemData->GetArmor() > 0)
		{
			Cast<APlayerCharacter>(GetOwningPlayerPawn())->GetCharacterStat()->DecreaseDEF(ItemData->GetArmor());
		}
	}
}

void UInventoryWindow::DecreaseStatOfSaveSlot(FItemData* ItemData, UABSaveGame* SaveSlot)
{
	if (ItemData->GetHP() > 0) 
	{
		SaveSlot->Hp = Cast<APlayerCharacter>(GetOwningPlayerPawn())->GetCharacterStat()->GetHpStat() - ItemData->GetHP();
	}
	if (ItemData->GetSP() > 0)
	{
		SaveSlot->Sp = Cast<APlayerCharacter>(GetOwningPlayerPawn())->GetCharacterStat()->GetSpStat() - ItemData->GetSP();
	}
	if (ItemData->GetAtk() > 0)
	{
		SaveSlot->Atk = Cast<APlayerCharacter>(GetOwningPlayerPawn())->GetCharacterStat()->GetAtkStat() - ItemData->GetAtk();
	}
	if (ItemData->GetDamage() > 0)
	{
		SaveSlot->Attack = Cast<APlayerCharacter>(GetOwningPlayerPawn())->GetCharacterStat()->GetAttack() - ItemData->GetDamage();
	}
	if (ItemData->GetArmor() > 0)
	{
		SaveSlot->Defense = Cast<APlayerCharacter>(GetOwningPlayerPawn())->GetCharacterStat()->GetDefense() - ItemData->GetArmor();
	}
}