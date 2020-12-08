// Fill out your copyright notice in the Description page of Project Settings.


#include "StatToolTipWidget.h"
#include "TextBlock.h"

void UStatToolTipWidget::NativeConstruct()
{
	Super::NativeConstruct();

	DEFPercentText = Cast<UTextBlock>(GetWidgetFromName(FName("PercentText")));
}

void UStatToolTipWidget::SetDEFPercent(float CurrentDEF)
{
	DEFPercentText->SetText(FText::FromString(FString::FromInt(CurrentDEF)));
}