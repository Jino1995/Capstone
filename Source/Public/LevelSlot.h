// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LevelSlot.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnIncreaseHPStatDelegate);
DECLARE_MULTICAST_DELEGATE(FOnIncreaseSPStatDelegate);
DECLARE_MULTICAST_DELEGATE(FOnIncreaseATKStatDelegate);

/**
 * 
 */
UCLASS()
class RPG_PJT_API ULevelSlot : public UUserWidget
{
	GENERATED_BODY()
	
private:
	class UTextBlock* LvTxt;
	UTextBlock* CurrentExpTxt;
	UTextBlock* NextExpTxt;

	UTextBlock* HPPoint;
	UTextBlock* SPPoint;
	UTextBlock* ATKPoint;
	UTextBlock* StatPoint;

	UTextBlock* TotalHP;
	UTextBlock* TotalSP;
	UTextBlock* TotalATK;
	UTextBlock* TotalDEF;

	UTextBlock* DEFText;

	class UButton* HPPointBt;
	UButton* SPPointBt;
	UButton* ATKPointBt;

	class UCharacterStatComponent* CurrentCharacterStat;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
	TSubclassOf<class UStatToolTipWidget> StatToolTipWidgetAsset;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	class UStatToolTipWidget* StatInfo;

	UFUNCTION()
	void ClickedHPPoint();

	UFUNCTION()
	void ClickedSPPoint();

	UFUNCTION()
	void ClickedATKPoint();

public:
	void BindCharacterStat(class UCharacterStatComponent* NewCharacterStat);
};
