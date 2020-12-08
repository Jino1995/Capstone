// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TrainerWindow.generated.h"

/**
 * 
 */
UCLASS()
class RPG_PJT_API UTrainerWindow : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;

private:
	UPROPERTY()
	class UButton* LvUp_BT;

	UPROPERTY()
	UButton* Exit_BT;

	UPROPERTY()
	class UTextBlock* NextLvCost;

	UPROPERTY()
	class UCharacterStatComponent* CurrentCharacterStat;

public:
	void BindCharacterStat(class UCharacterStatComponent* NewCharacterStat);

	UFUNCTION()
	void ClickedLvUpButton();

	UFUNCTION()
	void ClickedExitButton();

};
