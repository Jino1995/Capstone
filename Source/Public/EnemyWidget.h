// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EnemyWidget.generated.h"

/**
 * 
 */
UCLASS()
class RPG_PJT_API UEnemyWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void BindCharacterStat(class AEnemy* Enemy);

protected:
	virtual void NativeConstruct() override;
	void UpdateHPWidget();

private:
	TWeakObjectPtr<class AEnemy> WidgetOwner;

	UPROPERTY()
	class UProgressBar* HPProgressBar;

	FTimerHandle DamageTimer;
	FTimerHandle HealTimer;
};
