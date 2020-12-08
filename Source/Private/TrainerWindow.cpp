// Fill out your copyright notice in the Description page of Project Settings.


#include "TrainerWindow.h"
#include "Button.h"
#include "CharacterStatComponent.h"
#include "ABPlayerController.h"
#include "TextBlock.h"

void UTrainerWindow::NativeConstruct()
{
	Super::NativeConstruct();

	LvUp_BT = Cast<UButton>(GetWidgetFromName(FName("LvUpButton")));
	LvUp_BT->OnClicked.AddDynamic(this, &UTrainerWindow::ClickedLvUpButton);

	Exit_BT = Cast<UButton>(GetWidgetFromName(FName("ExitButton")));
	Exit_BT->OnClicked.AddDynamic(this, &UTrainerWindow::ClickedExitButton);
	
	NextLvCost = Cast<UTextBlock>(GetWidgetFromName(FName("LvCostTextBox")));
}

void UTrainerWindow::BindCharacterStat(class UCharacterStatComponent* NewCharacterStat)
{
	CurrentCharacterStat = NewCharacterStat;

	CurrentCharacterStat->OnLevelUpCostChanged.AddLambda([this]()-> void {
		NextLvCost->SetText(FText::FromString(FString::FromInt(CurrentCharacterStat->GetLevelUpCost())));
	});
}

void UTrainerWindow::ClickedLvUpButton()
{
	if (Cast<AABPlayerController>(GetOwningPlayer())->CheckUpEnoughMoney(CurrentCharacterStat->GetLevelUpCost()))
	{
		CurrentCharacterStat->LevelUp(1);
	}
}

void UTrainerWindow::ClickedExitButton()
{
	UE_LOG(LogTemp, Error, TEXT("Clicked BT2"));
	Cast<AABPlayerController>(GetOwningPlayer())->CloseTrainerWindow();
}