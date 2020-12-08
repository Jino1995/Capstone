// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Enemy.h"
#include "ABGameInstance.h"
#include "ABGameMode.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnStageClearDelegate);
DECLARE_MULTICAST_DELEGATE(FOnGameClearDelegate);
/**
 * 
 */
UCLASS()
class RPG_PJT_API AABGameMode : public AGameModeBase
{
	GENERATED_BODY()

	AABGameMode();

private:
	UPROPERTY()
	class UNavigationSystemV1* LevelNav;

	UPROPERTY()
	struct	FStageInfo StageInfo;

	UPROPERTY()
	class APlayerCharacter* Player;

	int32 Kill_MonsterNum;
	int32 Kill_BossNum;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
	TSubclassOf<class ALevelTransitionVolume> SpawnPortal_TSub;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
	TSubclassOf<class ASaveActor> TSaveActor;

public:
	FOnStageClearDelegate OnStageClear;
	FOnGameClearDelegate OnGameClear;

public:
	void SetGameState();

	void CheckKilledMonsterStatus(EMonsterType MonsterType);

	void StageClear();

	void SetPlayer(APlayerCharacter* TPlayer);

};
