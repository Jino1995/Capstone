// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TitleWidget.generated.h"

/**
 * 
 */
UCLASS()
class RPG_PJT_API UTitleWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;

private:
	UPROPERTY()
	class UButton* NewGameBt;

	UPROPERTY()
	UButton* ContinueBt;

	UPROPERTY()
	UButton* ExitBt;

public:
	UFUNCTION()
	void NewGame();

	UFUNCTION()
	void LoadGame();

	UFUNCTION()
	void EndGame();
};
