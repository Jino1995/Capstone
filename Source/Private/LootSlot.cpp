// Fill out your copyright notice in the Description page of Project Settings.


#include "LootSlot.h"
#include "Border.h"
#include "TextBlock.h"
#include "ABGameInstance.h"
#include "ABPlayerController.h"
#include "HUDOverlay.h"
#include "GridPanel.h"
#include "PlayerCharacter.h"
#include "ItemPot.h"
#include "LootWindow.h"

void ULootSlot::NativeConstruct()
{
	Super::NativeConstruct();

	ItemImage = Cast<UBorder>(GetWidgetFromName(FName("Image")));
	ItemAmount = Cast<UTextBlock>(GetWidgetFromName(FName("Amount")));
	ItemName = Cast<UTextBlock>(GetWidgetFromName(FName("NameText")));
}

FReply ULootSlot::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FEventReply reply;
	reply.NativeReply = Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);

	if (InMouseEvent.IsMouseButtonDown(EKeys::RightMouseButton) == true)
	{
		if (Cast<AABPlayerController>(GetOwningPlayer())->GetHUDOverlay()->GetInventoryWindow()->StoredInEmptySlot(IName, IAmount))
		{
			int32 Row = Cast<UGridPanel>(GetParent())->GetChildIndex(this);
			Cast<APlayerCharacter>(GetOwningPlayerPawn())->GetTargetItemPot()->RemoveItem(Row);
			RemoveFromParent();
		}
	}

	return reply.NativeReply;
}

void ULootSlot::SetItem(FName Name, int32 Amount)
{
	UABGameInstance* ABGameInstance = Cast<UABGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	FItemData* ItemData = ABGameInstance->GetItemData(Name);

	ItemImage->SetBrushFromTexture(ItemData->GetIcon());
	
	IAmount = Amount;
	if (ItemData->GetIsStackable())
	{
		ItemAmount->SetText(FText::FromString(FString::FromInt(Amount)));
	}
	else
	{
		ItemAmount->RemoveFromParent();
	}

	IName = ItemData->GetName();
	ItemName->SetText(FText::FromName(IName));
}
