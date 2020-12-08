// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SaveWindow.generated.h"

/**
 * 
 */
UCLASS()
class RPG_PJT_API USaveWindow : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;

private:
	UPROPERTY()
	class UButton* SaveBt;

	UPROPERTY()
	UButton* PotionAmountUpBT;

	UPROPERTY()
	UButton* PotionReinforceBT;

	UPROPERTY()
	UButton* ResumeBt;

public:
	UFUNCTION()
	void Save();

	UFUNCTION()
	void IncPotionTotalNum();

	UFUNCTION()
	void UpgradePotion();

	UFUNCTION()
	void ResumeGame();
};
