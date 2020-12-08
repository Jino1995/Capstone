// Fill out your copyright notice in the Description page of Project Settings.


#include "QuickSlot.h"
#include "TextBlock.h"

void UQuickSlot::NativeConstruct()
{
	Super::NativeConstruct();

	ForceText = Cast<UTextBlock>(GetWidgetFromName(FName("ForceTBlock")));
	ForceText->SetVisibility(ESlateVisibility::Visible);

	AmountText = Cast<UTextBlock>(GetWidgetFromName(FName("AmountTBlock")));
	AmountText->SetVisibility(ESlateVisibility::Visible);

	BaseHealingAmount = 50.f;

	OnePorceHealing = 50.f;

	SetMaxAmount(5);
	FillPotion();
	
	SetForceLv(0);
}

void UQuickSlot::FillPotion()
{
	CurrentAmount = MaxAmount;
	AmountText->SetText(FText::FromString(FString::FromInt(CurrentAmount)));
}

float UQuickSlot::UsePotion()
{
	if (CurrentAmount > 0)
	{
		CurrentAmount--;
		AmountText->SetText(FText::FromString(FString::FromInt(CurrentAmount)));
		return BaseHealingAmount + (OnePorceHealing*CurrentForce);
	}
	else
	{
		return 0;
	}
}

void UQuickSlot::SetForceLv(int32 NewForce)
{
	CurrentForce = NewForce;
	ForceText->SetText(FText::FromString(FString("+" + FString::FromInt(CurrentForce))));
}

void UQuickSlot::ForceLvUp()
{
	CurrentForce++;
	ForceText->SetText(FText::FromString(FString("+" + FString::FromInt(CurrentForce))));
}

void UQuickSlot::SetMaxAmount(int32 NewMaxAmount)
{
	MaxAmount = NewMaxAmount;
}

void UQuickSlot::MaxAmountLvUp()
{
	MaxAmount++;
	FillPotion();
}

void UQuickSlot::SetCurrentAmount(int32 NewAmount)
{
	CurrentAmount = NewAmount;
	AmountText->SetText(FText::FromString(FString::FromInt(CurrentAmount)));
}