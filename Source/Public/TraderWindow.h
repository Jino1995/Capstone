// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TraderWindow.generated.h"

/**
 * 
 */
UCLASS()
class RPG_PJT_API UTraderWindow : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;

private:
	UPROPERTY()
	class UUniformGridPanel* TraderInventory;

	UPROPERTY()
	UUniformGridPanel* UserInventory;

	UPROPERTY()
	class UButton* Sell_BT;

	UPROPERTY()
	UButton* Close_BT;

	int32 TraderItemNum;
	
public:
	void EmptyAllSlots();
	void FillSlots(TArray<FName> TraderItems);

	UFUNCTION()
	void ActivateSellBT();

	UFUNCTION()
	void ActivateCloseBT();
};
