// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyWidget.h"
#include "Enemy.h"
#include "Components/ProgressBar.h"
#include "Blueprint/UserWidget.h"
#include "Components/WidgetComponent.h"
#include "TextBlock.h"
#include "TimerManager.h"

void UEnemyWidget::BindCharacterStat(AEnemy* Enemy)
{
	WidgetOwner = Enemy;
	WidgetOwner->OnHPChanged.AddUObject(this, &UEnemyWidget::UpdateHPWidget);
}

void UEnemyWidget::NativeConstruct()
{
	Super::NativeConstruct();
	HPProgressBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("PB_HPBar")));
	UpdateHPWidget();
}

void UEnemyWidget::UpdateHPWidget()
{
	if (WidgetOwner.IsValid())
	{
		if (HPProgressBar != nullptr)
		{
			HPProgressBar->SetPercent(WidgetOwner->Health/WidgetOwner->MaxHealth);
		}
	}
}