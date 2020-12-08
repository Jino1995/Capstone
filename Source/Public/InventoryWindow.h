// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"
#include "ABGameInstance.h"
#include "Blueprint/UserWidget.h"
#include "InventoryWindow.generated.h"

/**
 * 
 */
UCLASS()
class RPG_PJT_API UInventoryWindow : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

private:
	/**
	* 인벤토리창 슬롯
	*/
	UPROPERTY()
	class UUniformGridPanel* Inventory;

	UPROPERTY()
	class UTextBlock* MoneyTxt;

	int32 CurrentMoney;

public:
	void InitInventory();

	bool StoredInEmptySlot(FName ItemNam, int32 Amount=1);

	void SetMoney(int32 NewMoney);
	void IncrementMoney(int32 IncMoney);
	void DecrementMoney(int32 DecMoney);

	bool CheckDecrementMoney(int32 DecMoney);

private:
	/**
	* 장비창 슬롯
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InventorySlot", Meta = (AllowPrivateAccess = true))
	class UEquipmentSlot* HeadSlot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InventorySlot", Meta = (AllowPrivateAccess = true))
	UEquipmentSlot* TopSlot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InventorySlot", Meta = (AllowPrivateAccess = true))
	UEquipmentSlot* BotSlot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InventorySlot", Meta = (AllowPrivateAccess = true))
	UEquipmentSlot* WeaponSlot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InventorySlot", Meta = (AllowPrivateAccess = true))
	UEquipmentSlot* ShieldSlot;

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InventorySlot", Meta = (AllowPrivateAccess = true))
	class ULevelSlot* LvSlot;

public:
	bool ChangeEquipItem(class UInventorySlot* NewItem);

	bool EquipItem(FName ItemName);
	bool UnEquipItem(EEquipmentSlot NewItemSlot);

	void IncreaseStatOfEquip(FItemData* ItemData);
	void DecreaseStatOfEquip(FItemData* ItemData);

	void UnEquipALLItemForSave(class UABSaveGame* SaveSlot);
	void DecreaseStatOfSaveSlot(FItemData* ItemData, UABSaveGame* SaveSlot);

	ULevelSlot* GetLvSlot() { return LvSlot; }

	bool PurchaceItem(FName ItemName);

	void SaveItems(class UABSaveGame* SaveSlot);
	void LoadItems(class UABSaveGame* LoadSlot);

	FORCEINLINE UEquipmentSlot* GetShieldSlot() { return ShieldSlot; }
	FORCEINLINE UEquipmentSlot* GetWeaponSlot() { return WeaponSlot; }
};
