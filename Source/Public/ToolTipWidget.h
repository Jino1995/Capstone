// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ABGameInstance.h"
#include "ToolTipWidget.generated.h"

/**
 * 
 */
UCLASS()
class RPG_PJT_API UToolTipWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

private:
	UPROPERTY()
	class UTextBlock* Name_Text;

	UPROPERTY()
	class UTextBlock* ItemType_Text;

	UPROPERTY()
	class UTextBlock* Damage_Text;

	UPROPERTY()
	class UTextBlock* Armor_Text;

	UPROPERTY()
	class UTextBlock* HP_Text;

	UPROPERTY()
	class UTextBlock* SP_Text;

	UPROPERTY()
	class UTextBlock* ATK_Text;
	
	UPROPERTY()
	class UTextBlock* DEF_Text;

	UPROPERTY()
	class UTextBlock* UseInfo_Text;

	UPROPERTY()
	class UTextBlock* Description_Text;

	UPROPERTY()
	class UTextBlock* StackSize_Text;

	UPROPERTY()
	class UTextBlock* Quality_Text;

	UPROPERTY()
	class UTextBlock* Price_Text;

	UPROPERTY()
	class UImage* TopBox;

public:
	void SetItemToolTip(FItemData& ItemData, bool bTraderSlot=false);
	void ChangeColorByItemQuality(EItemQuality ItemQuality);
};