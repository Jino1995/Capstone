// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"
#include "GameFramework/SaveGame.h"
#include "ABSaveGame.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType)
struct FInventorySlotData : public FTableRowBase
{
	GENERATED_BODY()

public:
	FInventorySlotData() : ItemName(FName("Default")), Amount(0), Pos(0){}

	UPROPERTY(VisibleAnywhere)
	FName ItemName;

	UPROPERTY(VisibleAnywhere)
	int32 Amount;

	UPROPERTY(VisibleAnywhere)
	int32 Pos;

};

USTRUCT(BlueprintType)
struct FPotionData : public FTableRowBase
{
	GENERATED_BODY()

public:
	FPotionData() : Reinforce_Amount(0), Reinforce_Force(0), CurrentAmount(0){}

	UPROPERTY()
	int32 Reinforce_Amount;
	
	UPROPERTY()
	int32 Reinforce_Force;

	UPROPERTY(VisibleAnywhere)
	int32 CurrentAmount;
};

UCLASS()
class RPG_PJT_API UABSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere)
	int32 Level;

	UPROPERTY(VisibleAnywhere)
	float CurrentHealth;

	UPROPERTY(VisibleAnywhere)
	float CurrentStamina;

	UPROPERTY(VisibleAnywhere)
	int32 Attack;

	UPROPERTY(VisibleAnywhere)
	int32 Defense;

	UPROPERTY(VisibleAnywhere)
	int32 Hp;

	UPROPERTY(VisibleAnywhere)
	int32 Sp;

	UPROPERTY(VisibleAnywhere)
	int32 Atk;

	UPROPERTY(VisibleAnywhere)
	int32 StatPoint;

	UPROPERTY(VisibleAnywhere)
	TArray<FInventorySlotData> InventoryItems;

	UPROPERTY(VisibleAnywhere)
	int32 Money;

	UPROPERTY(VisibleAnywhere)
	TArray<FName> EquipmentItems;

	UPROPERTY(VisibleAnywhere)
	FPotionData PotionStatus;

	UPROPERTY(VisibleAnywhere)
	FVector Position;

	UPROPERTY(VisibleAnywhere)
	FRotator Rotation;

	UPROPERTY(VisibleAnywhere)
	FString LevelName;
};
