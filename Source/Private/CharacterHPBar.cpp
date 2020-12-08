// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterHPBar.h"
#include "CharacterStatComponent.h"
#include "TextBlock.h"
#include "ProgressBar.h"

void UCharacterHPBar::BindCharacterStat(class UCharacterStatComponent* NewCharacterStat)
{
	CurrentTxt = Cast<UTextBlock>(GetWidgetFromName(FName("CurrentText")));
	MaxTxt = Cast<UTextBlock>(GetWidgetFromName(FName("MaxText")));
	Bar = Cast<UProgressBar>(GetWidgetFromName(FName("ProgressBar")));

	CurrentCharacterStat = NewCharacterStat;
	CurrentCharacterStat->OnMaxHPChanged.AddLambda([this]()-> void {
		MaxTxt->SetText(FText::FromString(FString::FromInt(CurrentCharacterStat->GetMHealth())));
		PercentUpdate();
	});

	CurrentCharacterStat->OnCurrentHPChanged.AddLambda([this]()-> void {
		CurrentTxt->SetText(FText::FromString(FString::FromInt(CurrentCharacterStat->GetCHealth())));
		PercentUpdate();
	});

}

void UCharacterHPBar::PercentUpdate()
{
	Bar->SetPercent(CurrentCharacterStat->GetCHealth() / CurrentCharacterStat->GetMHealth());
}
