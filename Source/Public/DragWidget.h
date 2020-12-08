// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"
#include "Blueprint/DragDropOperation.h"
#include "DragWidget.generated.h"

/**
 * 
 */
UCLASS()
class RPG_PJT_API UDragWidget : public UDragDropOperation
{
	GENERATED_BODY()
	
public:
	UPROPERTY()
	UUserWidget* WidgetToDrag;

	FVector2D DropWindowOffset;
};
