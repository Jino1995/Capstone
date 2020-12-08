// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TextBlock.h"
#include "LootSlot.generated.h"

/**
 * 
 */
UCLASS()
class RPG_PJT_API ULootSlot : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

private:
	class UBorder* ItemImage;
	class UTextBlock* ItemAmount;
	class UTextBlock* ItemName;

	FName IName;
	int32 IAmount;
public:
	void SetItem(FName Name, int32 Amount);

	FORCEINLINE FName GetIName() { return IName; }
	FORCEINLINE void SetItemAmount(int32 Amount) { IAmount = Amount;  ItemAmount->SetText(FText::FromString(FString::FromInt(Amount))); }
};
