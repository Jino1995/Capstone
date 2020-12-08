// Fill out your copyright notice in the Description page of Project Settings.


#include "TraderSlot.h"
#include "ABGameInstance.h"
#include "PlayerCharacter.h"
#include "ABPlayerController.h"
#include "Image.h"
#include "ToolTipWidget.h"
#include "CanvasPanel.h"

void UTraderSlot::NativeConstruct()
{
	Super::NativeConstruct();

	ItemIcon = Cast<UImage>(GetWidgetFromName(FName("ItemIcon")));

	SetItem(FName("Default"));
}

FReply UTraderSlot::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FEventReply reply;
	reply.NativeReply = Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);

	if (InMouseEvent.IsMouseButtonDown(EKeys::RightMouseButton) == true && ItemData->GetName()!=FName("Default"))
	{
		Cast<AABPlayerController>(GetOwningPlayer())->RequestPurchaseItem(ItemData->GetName());
	}

	return reply.NativeReply;
}

void UTraderSlot::SetItem(FName ItemName)
{
	UABGameInstance* ABGameInstance = Cast<UABGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	ItemData = ABGameInstance->GetItemData(ItemName);
	
	ItemIcon->SetBrushFromTexture(ItemData->GetIcon());

	ItemInfo = CreateWidget<UToolTipWidget>(GetOwningPlayer(), ToolTipWidgetAsset);
	if (ItemData->GetItemType() != EItemType::None)
	{
		Cast<UCanvasPanel>(GetWidgetFromName("Canvas"))->AddChild(ItemInfo);
		ItemInfo->SetItemToolTip(*ItemData, true);
		ItemInfo->RemoveFromParent();

		SetToolTip(ItemInfo);
	}
	else
	{
		SetToolTip(nullptr);
	}
}

bool UTraderSlot::IsEmptySlot()
{
	if (ItemData->GetName() == FName("Default"))
	{
		return true;
	}
	else
	{
		return false;
	}
}