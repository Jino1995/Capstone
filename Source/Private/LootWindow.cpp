// Fill out your copyright notice in the Description page of Project Settings.


#include "LootWindow.h"
#include "WidgetBlueprintLibrary.h"
#include "DragWidget.h"
#include "GridPanel.h"
#include "Button.h"
#include "TextBlock.h"
#include "ABPlayerController.h"
#include "LootSlot.h"
#include "InventorySlot.h"
#include "ItemPot.h"

void ULootWindow::NativeConstruct()
{
	Super::NativeConstruct();

	Inventory_Box = Cast<UGridPanel>(GetWidgetFromName(FName("InventoryBox")));
	Close_Button = Cast<UButton>(GetWidgetFromName(FName("CloseButton")));
	Close_Button->OnClicked.AddDynamic(this, &ULootWindow::SetHidden);

	TitleName = Cast<UTextBlock>(GetWidgetFromName(FName("Looting Items")));

	TopItemIndex = 0;
}

void ULootWindow::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
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

FReply ULootWindow::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FEventReply reply;
	reply.NativeReply = Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);

	if (InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton) == true)
	{
		reply = UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton);
	}
	else if (InMouseEvent.IsMouseButtonDown(EKeys::RightMouseButton) == true)
	{
		UE_LOG(LogTemp, Error, TEXT("It is lmouse"));
	}

	return reply.NativeReply;
}

void ULootWindow::SetTitle(FName ItemPotName)
{
	TitleName->SetText(FText::FromName(ItemPotName));
}

void ULootWindow::SetHidden()
{
	SetVisibility(ESlateVisibility::Hidden);
}

void ULootWindow::AddItem(FName ItemName, int32 Amount)
{
	UABGameInstance* ABGameInstance = Cast<UABGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	FItemData* NewItemData = ABGameInstance->GetItemData(ItemName);
	
	if (NewItemData->GetIsStackable())
	{
		int Row = 0;
		for (auto& Item : CurrentItemPot->GetItemPotInventory())
		{
			if (Item.Key == ItemName)
			{
				Item.Value += Amount;
				if (Item.Value <= NewItemData->GetMaxStackSize())
				{
					Cast<ULootSlot>(Inventory_Box->GetChildAt(Row))->SetItemAmount(Item.Value);	
				}
				else
				{
					ULootSlot* ItemSlot = CreateWidget<ULootSlot>(GetOwningPlayer(), LootSlotAsset);
					int32 Remainder = Item.Value % NewItemData->GetMaxStackSize();
					
					Item.Value = NewItemData->GetMaxStackSize();
					Cast<ULootSlot>(Inventory_Box->GetChildAt(Row))->SetItemAmount(Item.Value);

					Inventory_Box->AddChildToGrid(ItemSlot, TopItemIndex, 0);
					ItemSlot->SetItem(ItemName, Remainder);
					ItemSlot->SetItemAmount(Remainder);
					CurrentItemPot->GetItemPotInventory().Add(TPair<FName, int32>(ItemName, Amount));
					TopItemIndex++;
				}
				return;
			}

			Row++;
		}
		ULootSlot* ItemSlot = CreateWidget<ULootSlot>(GetOwningPlayer(), LootSlotAsset);
		Inventory_Box->AddChildToGrid(ItemSlot, TopItemIndex, 0);
		ItemSlot->SetItem(ItemName, Amount);
		ItemSlot->SetItemAmount(Amount);
		TopItemIndex++;
		
		CurrentItemPot->GetItemPotInventory().Add(TPair<FName, int32>(ItemName, Amount));
	}
	else
	{
		ULootSlot* ItemSlot = CreateWidget<ULootSlot>(GetOwningPlayer(), LootSlotAsset);
		CurrentItemPot->GetItemPotInventory().Add(TPair<FName, int32>(ItemName, Amount));
		Inventory_Box->AddChildToGrid(ItemSlot, TopItemIndex, 0);
		ItemSlot->SetItem(ItemName, 1);
		ItemSlot->SetItemAmount(Amount);
		TopItemIndex++;
	}
}

void ULootWindow::InitInventory(AItemPot* TargetItemPot)
{
	if (CurrentItemPot != TargetItemPot)
	{
		TopItemIndex = 0;
		CurrentItemPot = TargetItemPot;
		
		for (auto& ItemInfo : TargetItemPot->GetItemPotInventory())
		{
			ULootSlot* ItemSlot = CreateWidget<ULootSlot>(GetOwningPlayer(), LootSlotAsset);
			Inventory_Box->AddChildToGrid(ItemSlot, TopItemIndex, 0);
			TopItemIndex++;
			ItemSlot->SetItem(ItemInfo.Key, ItemInfo.Value);
		}
	}
}

void ULootWindow::ClearInventory()
{
	CurrentItemPot = nullptr;

	Inventory_Box->ClearChildren();
}