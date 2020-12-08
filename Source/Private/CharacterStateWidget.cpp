// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterStateWidget.h"
#include "TextBlock.h"
#include "CharacterStatComponent.h"
#include "CharacterHPBar.h"
#include "CharacterSPBar.h"

void UCharacterStateWidget::BindCharacterStat(class UCharacterStatComponent* NewCharacterStat)
{
	LvText = Cast<UTextBlock>(GetWidgetFromName(FName("LevelText")));
	
	CurrentCharacterStat = NewCharacterStat;
	CurrentCharacterStat->OnLevelChanged.AddLambda([this]()-> void {
		LvText->SetText(FText::FromString(FString::FromInt(CurrentCharacterStat->GetLevel())));
	});

	HPBar = Cast<UCharacterHPBar>(GetWidgetFromName(FName("HealthBar")));
	SPBar = Cast<UCharacterSPBar>(GetWidgetFromName(FName("StaminaBar")));
	
	HPBar->BindCharacterStat(CurrentCharacterStat);
	SPBar->BindCharacterStat(CurrentCharacterStat);
}