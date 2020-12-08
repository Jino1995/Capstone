// Fill out your copyright notice in the Description page of Project Settings.


#include "TraderWindow.h"
#include "UniformGridPanel.h"
#include "TraderSlot.h"
#include "Button.h"
#include "ABPlayerController.h"
#include "InventorySlot.h"

void UTraderWindow::NativeConstruct()
{
	Super::NativeConstruct();

	TraderInventory = Cast<UUniformGridPanel>(GetWidgetFromName(FName("TraderItems")));
	TraderItemNum = 0;

	UserInventory = Cast<UUniformGridPanel>(GetWidgetFromName(FName("UserItems")));

	Sell_BT = Cast<UButton>(GetWidgetFromName(FName("SellButton")));
	Sell_BT->OnClicked.AddDynamic(this, &UTraderWindow::ActivateSellBT);

	Close_BT = Cast<UButton>(GetWidgetFromName(FName("CancelButton")));
	Close_BT->OnClicked.AddDynamic(this, &UTraderWindow::ActivateCloseBT);

	for (auto ItemSlot : UserInventory->GetAllChildren())
	{
		Cast<UInventorySlot>(ItemSlot)->SetItem(FName("Default"));
		Cast<UInventorySlot>(ItemSlot)->ChangeSlotTypeToTrade();
	}
}

void UTraderWindow::EmptyAllSlots()
{
	for (auto ItemSlot : TraderInventory->GetAllChildren())
	{
		if (Cast<UTraderSlot>(ItemSlot)->IsEmptySlot())
		{
			return;
		}

		Cast<UTraderSlot>(ItemSlot)->SetItem(FName("Default"));
	}

	for (auto ItemSlot : UserInventory->GetAllChildren())
	{
		Cast<UInventorySlot>(ItemSlot)->SetItem(FName("Default"));
	}
}

void UTraderWindow::FillSlots(TArray<FName> TraderItems)
{
	int32 i = 0;
	for (auto Item : TraderItems)
	{
		Cast<UTraderSlot>(TraderInventory->GetChildAt(i))->SetItem(Item);
		i++;
	}
	
}

void UTraderWindow::ActivateSellBT()
{
	int32 SaleMoney = 0;

	for (auto ItemSlot : UserInventory->GetAllChildren())
	{
		if (Cast<UInventorySlot>(ItemSlot)->GetItemName() != FName("Default"))
		{
			SaleMoney += Cast<UInventorySlot>(ItemSlot)->GetItemSalePrice();
			Cast<UInventorySlot>(ItemSlot)->SetItem(FName("Default"));
		}
	}

	if (SaleMoney != 0)
	{
		Cast<AABPlayerController>(GetOwningPlayer())->RequestIncMoney(SaleMoney);
	}
}

void UTraderWindow::ActivateCloseBT()
{
	for (auto ItemSlot : UserInventory->GetAllChildren())
	{
		if (Cast<UInventorySlot>(ItemSlot)->GetItemName() != FName("Default"))
		{
			return;
		}
	}
	EmptyAllSlots();
	Cast<AABPlayerController>(GetOwningPlayer())->CloseTradeWindow();
}