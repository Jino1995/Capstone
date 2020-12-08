// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ABGameInstance.h"
#include "TraderSlot.generated.h"

/**
 * 
 */
UCLASS()
class RPG_PJT_API UTraderSlot : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InventorySlot", Meta = (AllowPrivateAccess = true))
	TSubclassOf<class UToolTipWidget> ToolTipWidgetAsset;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InventorySlot", Meta = (AllowPrivateAccess = true))
	class UToolTipWidget* ItemInfo;

	FItemData* ItemData;
	class UImage* ItemIcon;

public:
	void SetItem(FName ItemName);
	bool IsEmptySlot();
};
