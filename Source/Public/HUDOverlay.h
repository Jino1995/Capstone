// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ABGameInstance.h"
#include "InventoryWindow.h"
#include "LootWindow.h"
#include "SaveWindow.h"
#include "TraderWindow.h"
#include "QuickSlot.h"
#include "TrainerWindow.h"
#include "HUDOverlay.generated.h"

/**
 * 
 */
UCLASS()
class RPG_PJT_API UHUDOverlay : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets", Meta = (AllowPrivateAccess = true))
	UInventoryWindow* InventoryWindow;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets", Meta = (AllowPrivateAccess = true))
	ULootWindow* LootWindow;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets", Meta = (AllowPrivateAccess = true))
	UTraderWindow* TradeWindow;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets", Meta = (AllowPrivateAccess = true))
	UTrainerWindow* TrainerWindow;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets", Meta = (AllowPrivateAccess = true))
	UQuickSlot* QuickSlot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets", Meta = (AllowPrivateAccess = true))
	class UCharacterStateWidget* StateWidget;

public:
	FORCEINLINE void OpenInventoryWindow() { InventoryWindow->SetVisibility(ESlateVisibility::Visible); }
	FORCEINLINE void CloseInventoryWindow() { InventoryWindow->SetVisibility(ESlateVisibility::Hidden); }
	FORCEINLINE UInventoryWindow* GetInventoryWindow() { return InventoryWindow; }
	FORCEINLINE ULootWindow* GetLootWindow() { return LootWindow; }
	FORCEINLINE UQuickSlot* GetQuickSlot() { return QuickSlot; }

	void OpenLootWindow(class AItemPot* TargetItemPot);
	void CloseLootWindow();

	void OpenTradeWindow(TArray<FName> TraderItems);
	void CloseTradeWindow();

	void OpenTrainerWindow();
	void CloseTrainerWindow();

	void BindCharacterStat(class UCharacterStatComponent* StatComp);

	float CheckPotion();
	void FillPotion();

	void UpgradePotionNum();
	void UpgradePotionForce();

	void SaveCurrentStatus(UABSaveGame* NewSave);
	void LoadStatus(UABSaveGame* LoadData);
};
