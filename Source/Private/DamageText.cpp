// Fill out your copyright notice in the Description page of Project Settings.


#include "DamageText.h"
#include "CanvasPanel.h"
#include "TextBlock.h"

void UDamageText::NativeConstruct()
{
	Super::NativeConstruct();
	Canvas = Cast<UCanvasPanel>(GetWidgetFromName(FName("CanvasP")));
}

void UDamageText::CreateDamageText(int32 Damage)
{
	auto DText = CreateWidget<UUserWidget>(GetOwningPlayer(), tAsset);
	Canvas->AddChildToCanvas(DText);
	Cast<UTextBlock>(DText->GetWidgetFromName(FName("TxtBlock")))->SetText(FText::FromString(FString::FromInt(Damage)));
}

void UDamageText::CreateMoneyText(int32 Money)
{
	auto MText = CreateWidget<UUserWidget>(GetOwningPlayer(), MoneyTextAsset);
	Canvas->AddChildToCanvas(MText);
	Cast<UTextBlock>(MText->GetWidgetFromName(FName("TxtBlock")))->SetText(FText::FromString(FString::FromInt(Money)));
}

void UDamageText::CreateHealingText(int32 Heal)
{
	auto HText = CreateWidget<UUserWidget>(GetOwningPlayer(), HealingTextAsset);
	Canvas->AddChildToCanvas(HText);
	Cast<UTextBlock>(HText->GetWidgetFromName(FName("TxtBlock")))->SetText(FText::FromString(FString::FromInt(Heal)));
}