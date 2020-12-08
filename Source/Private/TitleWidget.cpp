// Fill out your copyright notice in the Description page of Project Settings.


#include "TitleWidget.h"
#include "Button.h"
#include "Engine.h"
#include "Kismet/KismetSystemLibrary.h"
#include "ABSaveGame.h"

void UTitleWidget::NativeConstruct()
{
	NewGameBt = Cast<UButton>(GetWidgetFromName(FName("NewGameButton")));
	NewGameBt->OnClicked.AddDynamic(this, &UTitleWidget::NewGame);

	ContinueBt = Cast<UButton>(GetWidgetFromName(FName("ContinueGameButton")));
	ContinueBt->OnClicked.AddDynamic(this, &UTitleWidget::LoadGame);

	ExitBt = Cast<UButton>(GetWidgetFromName(FName("EndGameButton")));
	ExitBt->OnClicked.AddDynamic(this, &UTitleWidget::EndGame);

	auto LoadGameData = Cast<UABSaveGame>(UGameplayStatics::LoadGameFromSlot(FString("First"), 0));
	if (!IsValid(LoadGameData))
	{
		ContinueBt->RemoveFromParent();
	}
}

void UTitleWidget::NewGame()
{
	auto LoadGameData = Cast<UABSaveGame>(UGameplayStatics::LoadGameFromSlot(FString("First"), 0));
	if (IsValid(LoadGameData))
	{
		UGameplayStatics::DeleteGameInSlot(FString("First"), 0);
	}
	UGameplayStatics::OpenLevel(GetWorld(), FName("Stage1"), true);
}

void UTitleWidget::LoadGame()
{
	auto LoadGameData = Cast<UABSaveGame>(UGameplayStatics::LoadGameFromSlot(FString("First"), 0));
	FName LevelName(*LoadGameData->LevelName);
	UGameplayStatics::OpenLevel(GetWorld(), LevelName, true);
}

void UTitleWidget::EndGame()
{
	UKismetSystemLibrary::QuitGame(GetWorld(), nullptr, EQuitPreference::Quit, false);
}