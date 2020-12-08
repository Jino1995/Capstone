// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterSPBar.h"
#include "CharacterStatComponent.h"
#include "TextBlock.h"
#include "ProgressBar.h"

void UCharacterSPBar::BindCharacterStat(class UCharacterStatComponent* NewCharacterStat)
{
	CurrentTxt = Cast<UTextBlock>(GetWidgetFromName(FName("CurrentText")));
	MaxTxt = Cast<UTextBlock>(GetWidgetFromName(FName("MaxText")));
	Bar = Cast<UProgressBar>(GetWidgetFromName(FName("ProgressBar")));

	CurrentCharacterStat = NewCharacterStat;
	CurrentCharacterStat->OnMaxSPChanged.AddLambda([this]()-> void {
		MaxTxt->SetText(FText::FromString(FString::FromInt(CurrentCharacterStat->GetMStamina())));
		PercentUpdate();
	});

	
	CurrentCharacterStat->OnCurrentSPChanged.AddLambda([this]()-> void {
		CurrentTxt->SetText(FText::FromString(FString::FromInt(CurrentCharacterStat->GetCStamina())));
		PercentUpdate();
	});

}

void UCharacterSPBar::PercentUpdate()
{
	Bar->SetPercent(CurrentCharacterStat->GetCStamina() / CurrentCharacterStat->GetMStamina());
}
