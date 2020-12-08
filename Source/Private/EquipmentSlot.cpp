// Fill out your copyright notice in the Description page of Project Settings.


#include "EquipmentSlot.h"
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
#include "InventorySlot.h"
#include "InventoryWindow.h"
#include "LootWindow.h"
#include "Overlay.h"

void UEquipmentSlot::NativeConstruct()
{
	Super::NativeConstruct();

	ItemIcon = Cast<UImage>(GetWidgetFromName(FName("ItemIcon")));

	UABGameInstance* ABGameInstance = Cast<UABGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	ItemData = ABGameInstance->GetItemData("Default");
}

bool UEquipmentSlot::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
	if (Cast<ULootWindow>(Cast<UDragWidget>(InOperation)->WidgetToDrag))
	{
		return false;
	}

	UInventorySlot* DraggedItem = Cast<UInventorySlot>(Cast<UDragWidget>(InOperation)->WidgetToDrag);
	if (DraggedItem->GetItemType() == EItemType::Equipment && DraggedItem->GetEquipSlot() == SlotType && Cast<APlayerCharacter>(GetOwningPlayerPawn())->GetCombatMode() == ECombatMode::E_Normal_Mode)
	{
		Cast<AABPlayerController>(GetOwningPlayer())->EquipItem(DraggedItem);
	};

	return true;
}

FReply UEquipmentSlot::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FEventReply reply;
	reply.NativeReply = Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
	
	if (ItemData->GetItemType() != EItemType::None)
	{
		if (InMouseEvent.IsMouseButtonDown(EKeys::RightMouseButton) == true)
		{
			if (Cast<APlayerCharacter>(GetOwningPlayerPawn())->GetCombatMode() == ECombatMode::E_Normal_Mode && !(Cast<APlayerCharacter>(GetOwningPlayerPawn())->IsEquipping()))
			{
				Cast<AABPlayerController>(GetOwningPlayer())->UnEquipItem(ItemData->GetName());
			}
		}
	}

	return reply.NativeReply;
}

void UEquipmentSlot::SetItem(FName ItemName)
{
	UABGameInstance* ABGameInstance = Cast<UABGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));

	ItemData = ABGameInstance->GetItemData(ItemName);
	ItemIcon->SetBrushFromTexture(ItemData->GetIcon());

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

void UEquipmentSlot::SetIcon(UTexture2D* NewIcon)
{
	Cast<UImage>(GetWidgetFromName(FName("ItemIcon")))->SetBrushFromTexture(NewIcon);
}