// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CharacterSPBar.generated.h"

/**
 * 
 */
UCLASS()
class RPG_PJT_API UCharacterSPBar : public UUserWidget
{
	GENERATED_BODY()
	
private:
	class UTextBlock* CurrentTxt;
	UTextBlock* MaxTxt;
	class UProgressBar* Bar;

	class UCharacterStatComponent* CurrentCharacterStat;

private:
	void PercentUpdate();

public:
	void BindCharacterStat(class UCharacterStatComponent* NewCharacterStat);
};
