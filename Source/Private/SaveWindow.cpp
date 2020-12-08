// Fill out your copyright notice in the Description page of Project Settings.


#include "SaveWindow.h"
#include "Button.h"
#include "Engine.h"
#include "ABPlayerController.h"
#include "ABSaveGame.h"

void USaveWindow::NativeConstruct()
{
	Super::NativeConstruct();

	SaveBt = Cast<UButton>(GetWidgetFromName(FName("SaveButton")));
	PotionAmountUpBT = Cast<UButton>(GetWidgetFromName(FName("PotionNButton")));
	PotionReinforceBT = Cast<UButton>(GetWidgetFromName(FName("PotionFButton")));
	ResumeBt = Cast<UButton>(GetWidgetFromName(FName("ResumeButton")));

	SaveBt->OnClicked.AddDynamic(this, &USaveWindow::Save);
	PotionAmountUpBT->OnClicked.AddDynamic(this, &USaveWindow::IncPotionTotalNum);
	PotionReinforceBT->OnClicked.AddDynamic(this, &USaveWindow::UpgradePotion);
	ResumeBt->OnClicked.AddDynamic(this, &USaveWindow::ResumeGame);
}

void USaveWindow::Save()
{
	Cast<AABPlayerController>(GetOwningPlayer())->SaveGame();
}

void USaveWindow::IncPotionTotalNum()
{
	Cast<AABPlayerController>(GetOwningPlayer())->UpgradePotionAmount();
}

void USaveWindow::UpgradePotion()
{
	Cast<AABPlayerController>(GetOwningPlayer())->UpgradePotionForce();
}

void USaveWindow::ResumeGame()
{
	Cast<AABPlayerController>(GetOwningPlayer())->OpenSaveMenu();
}