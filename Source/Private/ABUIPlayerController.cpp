// Fill out your copyright notice in the Description page of Project Settings.


#include "ABUIPlayerController.h"
#include "Blueprint/UserWidget.h"

void AABUIPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (UIWidgetClass)
	{
		UIWidgetInstance = CreateWidget<UUserWidget>(this, UIWidgetClass);
	}

	UIWidgetInstance->AddToViewport();
	bShowMouseCursor = true;
}
