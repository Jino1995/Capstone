// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DamageText.generated.h"

/**
 * 
 */
UCLASS()
class RPG_PJT_API UDamageText : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;

private:
	UPROPERTY()
	class UCanvasPanel* Canvas;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
	TSubclassOf<class UUserWidget> tAsset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
	TSubclassOf<class UUserWidget> MoneyTextAsset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
	TSubclassOf<class UUserWidget> HealingTextAsset;

public:
	void CreateDamageText(int32 Damage);
	void CreateMoneyText(int32 Money);
	void CreateHealingText(int32 Heal);
};
