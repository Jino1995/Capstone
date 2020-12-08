// Fill out your copyright notice in the Description page of Project Settings.


#include "InventorySlot.h"
#include "Image.h"
#include "ABGameInstance.h"
#include "TextBlock.h"
#include "PlayerCharacter.h"
#include "ToolTipWidget.h"
#include "CanvasPanel.h"
#include "WidgetLayoutLibrary.h"
#include "CanvasPanelSlot.h"
#include "WidgetBlueprintLibrary.h"
#include "DragWidget.h"
#include "ABPlayerController.h"
#include "LootWindow.h"
#include "CharacterStatComponent.h"

void UInventorySlot::NativeConstruct()
{
	Super::NativeConstruct();

	AmountText = Cast<UTextBlock>(GetWidgetFromName(FName("ItemAmount")));
	AmountText->SetVisibility(ESlateVisibility::Hidden);
	
	ItemIcon = Cast<UImage>(GetWidgetFromName(FName("ItemIcon")));

	UABGameInstance* ABGameInstance = Cast<UABGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	ItemData = ABGameInstance->GetItemData("Default");
	ItemType = ItemData->GetItemType();

	SlotType = ESlotType::Inventory;
}

bool UInventorySlot::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
	if (Cast<UInventorySlot>(Cast<UDragWidget>(InOperation)->WidgetToDrag))
	{
		int32 ChangeSlot1 = Cast<UUniformGridPanel>(GetParent())->GetChildIndex(this);
		int32 ChangeSlot2 = Cast<UUniformGridPanel>(GetParent())->GetChildIndex(Cast<UDragWidget>(InOperation)->WidgetToDrag);
		UE_LOG(LogTemp, Error, TEXT("%d : %d"), ChangeSlot1, ChangeSlot2);

		FName myItemName = ItemData->GetName();
		int32 Amount = ItemAmount;
		SetItem(Cast<UInventorySlot>(Cast<UDragWidget>(InOperation)->WidgetToDrag)->ItemData->GetName(), Cast<UInventorySlot>(Cast<UDragWidget>(InOperation)->WidgetToDrag)->GetItemAmount());
		Cast<UInventorySlot>(Cast<UDragWidget>(InOperation)->WidgetToDrag)->SetItem(myItemName, Amount);
	}

	return true;
}

void UInventorySlot::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);

	FVector2D DragWindowOffset = InGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());

	UDragWidget* oper = NewObject<UDragWidget>();
	oper->DefaultDragVisual = this;
	oper->WidgetToDrag = this;
	oper->DropWindowOffset = DragWindowOffset;
	oper->Pivot = EDragPivot::CenterCenter;
	OutOperation = oper;
}


FReply UInventorySlot::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FEventReply reply;
	reply.NativeReply = Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
	
	if (InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton) == true && ItemType != EItemType::None)
	{
		reply = UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton);
	}
	else if (InMouseEvent.IsMouseButtonDown(EKeys::RightMouseButton) == true && !(Cast<APlayerCharacter>(GetOwningPlayerPawn())->IsEquipping()) && SlotType == ESlotType::Inventory)
	{
		switch (ItemData->GetItemType())
		{
		case EItemType::Consumable:
			UseConsumeTypeItem();
			break;
		case EItemType::Equipment:
			UseEquipTypeItem();
			break;
		default:
			break;
		}
	}

	return reply.NativeReply;
}

void UInventorySlot::SetItem(FName ItemName, int32 Amount)
{
	UABGameInstance* ABGameInstance = Cast<UABGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));

	ItemData = ABGameInstance->GetItemData(ItemName);
	ItemAmount = Amount;
	ItemIcon->SetBrushFromTexture(ItemData->GetIcon());
	ItemType = ItemData->GetItemType();

	if (ItemData->GetIsStackable())
	{
;		AmountText->SetText(FText::FromString(FString::FromInt(ItemAmount)));
		AmountText->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		AmountText->SetVisibility(ESlateVisibility::Hidden);
	}

	ItemInfo = CreateWidget<UToolTipWidget>(GetOwningPlayer(), ToolTipWidgetAsset);
	if (ItemData->GetItemType() != EItemType::None)
	{
		Cast<UCanvasPanel>(GetWidgetFromName("Canvas"))->AddChild(ItemInfo);
		ItemInfo->SetItemToolTip(*ItemData);
		ItemInfo->RemoveFromParent();

		SetToolTip(ItemInfo);
	}
	else
	{
		SetToolTip(nullptr);
	}
}

void UInventorySlot::SetIcon(UTexture2D* NewIcon)
{
	Cast<UImage>(GetWidgetFromName(FName("ItemIcon")))->SetBrushFromTexture(NewIcon);
}

void UInventorySlot::SetItemAmount(int32 AddAmount) 
{
	ItemAmount = AddAmount;
	AmountText->SetText(FText::FromString(FString::FromInt(ItemAmount)));
}

void UInventorySlot::AddAmount(int32 AddAmount)
{
	ItemAmount += AddAmount;
	AmountText->SetText(FText::FromString(FString::FromInt(ItemAmount)));
}

void UInventorySlot::UseConsumeTypeItem()
{
	if (ItemAmount >= 2)
	{
		Cast<APlayerCharacter>(GetOwningPlayerPawn())->GetCharacterStat()->IncreaseCurrentHealth(ItemData->GetHeal());
		AddAmount(-1);
	}
	else if (ItemAmount == 1)
	{
		AmountText->SetVisibility(ESlateVisibility::Hidden);
		Cast<APlayerCharacter>(GetOwningPlayerPawn())->GetCharacterStat()->IncreaseCurrentHealth(ItemData->GetHeal());
		SetItem(FName("Default"));
	}
}

void UInventorySlot::UseEquipTypeItem()
{
	if (Cast<APlayerCharacter>(GetOwningPlayerPawn())->GetCombatMode() == ECombatMode::E_Normal_Mode)
	{
		Cast<AABPlayerController>(GetOwningPlayer())->EquipItem(this);
	}
}