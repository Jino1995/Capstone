// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StatToolTipWidget.generated.h"

/**
 * 
 */
UCLASS()
class RPG_PJT_API UStatToolTipWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

private:
	UPROPERTY()
	class UTextBlock* DEFPercentText;

public:
	void SetDEFPercent(float CurrentDEF);
};
