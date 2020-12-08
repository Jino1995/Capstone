// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LootWindow.generated.h"

/**
 * 
 */
UCLASS()
class RPG_PJT_API ULootWindow : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	
private:
	UPROPERTY()
	class UGridPanel* Inventory_Box;

	UPROPERTY()
	class UButton* Close_Button;

	UPROPERTY()
	class UTextBlock* TitleName;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InventorySlot", Meta = (AllowPrivateAccess = true))
	TSubclassOf<class ULootSlot> LootSlotAsset;

	int32 TopItemIndex;

	UPROPERTY()
	class AItemPot* CurrentItemPot;

public:
	void SetTitle(FName ItemPotName);

	UFUNCTION()
	void SetHidden();

	UFUNCTION()
	void AddItem(FName ItemName, int32 Amount);

	void InitInventory(class AItemPot* TargetItemPot);
	void ClearInventory();

	FORCEINLINE UGridPanel* GetInventoryBox() { return Inventory_Box; }
	FORCEINLINE void SetTopItemIndex(int32 n) { TopItemIndex += n; }
};
