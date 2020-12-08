// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ABGameInstance.h"
#include "EquipmentSlot.generated.h"

/**
 * 
 */
UCLASS()
class RPG_PJT_API UEquipmentSlot : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

private:
	FItemData* ItemData;
	class UInventoryWindow* InvWindow;
	class UImage* ItemIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InventorySlot", Meta = (AllowPrivateAccess = true))
	TSubclassOf<class UToolTipWidget> ToolTipWidgetAsset;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InventorySlot", Meta = (AllowPrivateAccess = true))
	class UToolTipWidget* ItemInfo;

	EEquipmentSlot SlotType;

public:
	void SetItem(FName ItemName);
	void SetIcon(UTexture2D* NewIcon);

	FORCEINLINE FName GetItemName() { return ItemData->GetName(); }
	FORCEINLINE EWeaponType GetWeaponType() { return ItemData->GetWeaponType(); }
	FORCEINLINE void SetSlotType(EEquipmentSlot SlotTypeName, UInventoryWindow* Window) { SlotType = SlotTypeName; InvWindow = Window; }
};

