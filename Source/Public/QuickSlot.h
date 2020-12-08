// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "QuickSlot.generated.h"

/**
 * 
 */
UCLASS()
class RPG_PJT_API UQuickSlot : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;

private:
	class UTextBlock* ForceText;
	class UTextBlock* AmountText;

	int32 MaxAmount;
	int32 CurrentAmount;

	int32 CurrentForce;

	float BaseHealingAmount;

	float OnePorceHealing;

public:
	void SetForceLv(int32 NewForce);
	void ForceLvUp();

	void SetMaxAmount(int32 NewMaxAmount);
	void MaxAmountLvUp();

	void FillPotion();
	
	float UsePotion();

	void SetCurrentAmount(int32 NewAmount);

	int32 GetMaxAmount() { return MaxAmount; }
	int32 GetCurrentForce() { return CurrentForce; }
	int32 GetCurrentAmount() { return CurrentAmount; }
};
