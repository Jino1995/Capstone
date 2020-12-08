// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ABGameInstance.h"
#include "InventorySlot.generated.h"

UENUM(BlueprintType)
enum class ESlotType : uint8
{
	Inventory,
	Trade
};

UCLASS()
class RPG_PJT_API UInventorySlot : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

private:
	FItemData* ItemData;

	class UTextBlock* AmountText;
	class UImage* ItemIcon;

	int32 ItemAmount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InventorySlot", Meta = (AllowPrivateAccess = true))
	TSubclassOf<class UToolTipWidget> ToolTipWidgetAsset;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InventorySlot", Meta = (AllowPrivateAccess = true))
	class UToolTipWidget* ItemInfo;

	EItemType ItemType;

	ESlotType SlotType;
private:
	void UseConsumeTypeItem();
	void UseEquipTypeItem();

public:
	void SetItem(FName ItemName, int32 Amount=1);
	void ChangeSlotTypeToTrade() { SlotType = ESlotType::Trade; }
	void SetIcon(UTexture2D* NewIcon);
	void SetItemAmount(int32 AddAmount);
	void AddAmount(int32 AddAmount);

	FORCEINLINE EItemType GetItemType() { return ItemType; }
	FORCEINLINE EEquipmentSlot GetEquipSlot() { return ItemData->GetEquipmentSlot(); }
	FORCEINLINE FName GetItemName() { return ItemData->GetName(); }
	FORCEINLINE int32 GetItemAmount() { return ItemAmount; }
	FORCEINLINE int32 GetItemSalePrice() { return ItemData->GetSale()*ItemAmount; }
};
