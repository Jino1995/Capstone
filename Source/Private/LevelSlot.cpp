// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelSlot.h"
#include "CharacterStatComponent.h"
#include "TextBlock.h"
#include "Button.h"
#include "StatToolTipWidget.h"
#include "CanvasPanel.h"

void ULevelSlot::BindCharacterStat(UCharacterStatComponent* NewCharacterStat)
{
	LvTxt = Cast<UTextBlock>(GetWidgetFromName(FName("LevelText")));
	NextExpTxt = Cast<UTextBlock>(GetWidgetFromName(FName("NextExpText")));
	CurrentExpTxt = Cast<UTextBlock>(GetWidgetFromName(FName("CurrentExpText")));
	HPPoint = Cast<UTextBlock>(GetWidgetFromName(FName("HPStat")));
	SPPoint = Cast<UTextBlock>(GetWidgetFromName(FName("SPStat")));
	ATKPoint = Cast<UTextBlock>(GetWidgetFromName(FName("ATKStat")));
	StatPoint = Cast<UTextBlock>(GetWidgetFromName(FName("StatPointNum")));
	DEFText = Cast<UTextBlock>(GetWidgetFromName(FName("DEFNameText")));

	TotalHP = Cast<UTextBlock>(GetWidgetFromName(FName("TotalHPStat")));
	TotalSP = Cast<UTextBlock>(GetWidgetFromName(FName("TotalSPStat")));
	TotalATK = Cast<UTextBlock>(GetWidgetFromName(FName("TotalATKStat")));
	TotalDEF = Cast<UTextBlock>(GetWidgetFromName(FName("TotalDEFStat")));
	
	HPPointBt = Cast<UButton>(GetWidgetFromName(FName("HPStatButton")));
	SPPointBt = Cast<UButton>(GetWidgetFromName(FName("SPStatButton")));
	ATKPointBt = Cast<UButton>(GetWidgetFromName(FName("ATKStatButton")));


	HPPointBt->OnClicked.AddDynamic(this, &ULevelSlot::ClickedHPPoint);
	SPPointBt->OnClicked.AddDynamic(this, &ULevelSlot::ClickedSPPoint);
	ATKPointBt->OnClicked.AddDynamic(this, &ULevelSlot::ClickedATKPoint);

	HPPointBt->SetVisibility(ESlateVisibility::Hidden);
	SPPointBt->SetVisibility(ESlateVisibility::Hidden);
	ATKPointBt->SetVisibility(ESlateVisibility::Hidden);

	StatInfo = CreateWidget<UStatToolTipWidget>(GetOwningPlayer(), StatToolTipWidgetAsset);
	Cast<UCanvasPanel>(GetWidgetFromName("Canvas"))->AddChild(StatInfo);
	StatInfo->SetDEFPercent(0);
	StatInfo->RemoveFromParent();
	DEFText->SetToolTip(StatInfo);

	CurrentCharacterStat = NewCharacterStat;
	CurrentCharacterStat->OnLevelChanged.AddLambda([this]()->void {
		LvTxt->SetText(FText::FromString(FString::FromInt(CurrentCharacterStat->GetLevel())));
	//	NextExpTxt->SetText(FText::FromString(FString::FromInt(CurrentCharacterStat->GetNextExp())));
	});

	CurrentCharacterStat->OnMaxHPChanged.AddLambda([this]()->void {
		TotalHP->SetText(FText::FromString(FString::FromInt(CurrentCharacterStat->GetMHealth())));
	});

	CurrentCharacterStat->OnMaxSPChanged.AddLambda([this]()->void {
		TotalSP->SetText(FText::FromString(FString::FromInt(CurrentCharacterStat->GetMStamina())));
	});

	CurrentCharacterStat->OnAttackChanged.AddLambda([this]()->void {
		TotalATK->SetText(FText::FromString(FString::FromInt(CurrentCharacterStat->GetAttack())));
	});

	CurrentCharacterStat->OnDefenseChanged.AddLambda([this]()->void {
		TotalDEF->SetText(FText::FromString(FString::FromInt(CurrentCharacterStat->GetDefense())));
	});

	CurrentCharacterStat->OnHPStatChanged.AddLambda([this]()->void {
		HPPoint->SetText(FText::FromString(FString::FromInt(CurrentCharacterStat->GetHpStat())));
	});

	CurrentCharacterStat->OnSPStatChanged.AddLambda([this]()->void {
		SPPoint->SetText(FText::FromString(FString::FromInt(CurrentCharacterStat->GetSpStat())));
	});

	CurrentCharacterStat->OnATKStatChanged.AddLambda([this]()->void {
		ATKPoint->SetText(FText::FromString(FString::FromInt(CurrentCharacterStat->GetAtkStat())));
	});
/*
	CurrentCharacterStat->OnCurrentEXPChanged.AddLambda([this]()->void {
		CurrentExpTxt->SetText(FText::FromString(FString::FromInt(CurrentCharacterStat->GetCurrentExp())));
	});
*/
	CurrentCharacterStat->OnStatPointChanged.AddLambda([this]()->void {
		if (CurrentCharacterStat->GetStatPoint() == 0)
		{
			HPPointBt->SetVisibility(ESlateVisibility::Hidden);
			SPPointBt->SetVisibility(ESlateVisibility::Hidden);
			ATKPointBt->SetVisibility(ESlateVisibility::Hidden);
		}
		else if (HPPointBt->GetVisibility() == ESlateVisibility::Hidden)
		{
			HPPointBt->SetVisibility(ESlateVisibility::Visible);
			SPPointBt->SetVisibility(ESlateVisibility::Visible);
			ATKPointBt->SetVisibility(ESlateVisibility::Visible);
		}
		StatPoint->SetText(FText::FromString(FString::FromInt(CurrentCharacterStat->GetStatPoint())));
	});

	CurrentCharacterStat->OnDEFPercentChanged.AddLambda([this]()->void {
		StatInfo->SetDEFPercent(CurrentCharacterStat->GetDefPercent());
	});
}

void ULevelSlot::ClickedHPPoint()
{
	CurrentCharacterStat->UseStatPoint(FString("HP"));
}

void ULevelSlot::ClickedSPPoint()
{
	CurrentCharacterStat->UseStatPoint(FString("SP"));
}

void ULevelSlot::ClickedATKPoint()
{
	CurrentCharacterStat->UseStatPoint(FString("ATK"));
}

/*
ItemInfo = CreateWidget<UToolTipWidget>(GetOwningPlayer(), ToolTipWidgetAsset);
if (ItemData->GetItemType() != EItemType::None)
{
	Cast<UCanvasPanel>(GetWidgetFromName("Canvas"))->AddChild(ItemInfo);
	ItemInfo->SetEquipItemToolTip(*ItemData);
	ItemInfo->RemoveFromParent();

	SetToolTip(ItemInfo);
}
*/