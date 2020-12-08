// Fill out your copyright notice in the Description page of Project Settings.


#include "ABGameMode.h"
#include "PlayerCharacter.h"
#include "ABPlayerController.h"
#include "NavigationSystem.h"
#include "ABGameInstance.h"
#include "Enemy.h"
#include "LevelTransitionVolume.h"
#include "SaveActor.h"

AABGameMode::AABGameMode()
{

}

void AABGameMode::SetGameState()
{
	LevelNav = UNavigationSystemV1::GetNavigationSystem(GetWorld());

	UABGameInstance* ABGameInstance = Cast<UABGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	FString CurrentMap = UGameplayStatics::GetCurrentLevelName(GetWorld());
	StageInfo = *(ABGameInstance->GetStageInfo(CurrentMap));

	Kill_MonsterNum = 0;
	Kill_BossNum = 0;
}

void AABGameMode::CheckKilledMonsterStatus(EMonsterType MonsterType)
{
	if ((MonsterType == EMonsterType::Normal && StageInfo.GetMonsterNum() == -1) || (MonsterType == EMonsterType::Boss && StageInfo.GetBossNum() == -1))
	{
		return;
	}

	switch(MonsterType)
	{
	case EMonsterType::Normal:
		Kill_MonsterNum++;
		break;
	case EMonsterType::Boss:
		Kill_BossNum++;
		break;
	}

	if (StageInfo.GetMonsterNum() == Kill_MonsterNum || StageInfo.GetBossNum() == Kill_BossNum)
	{
		StageClear();
	}
}

void AABGameMode::StageClear()
{
	FString CurrentMap = UGameplayStatics::GetCurrentLevelName(GetWorld());
	if (CurrentMap == FString("FinalStage"))
	{
		OnGameClear.Broadcast();
	}
	else
	{
		FNavLocation SpawnLocation;
		LevelNav->GetRandomPointInNavigableRadius(Player->GetActorLocation(), 500.0f, SpawnLocation);
		FVector SpawnVector = FVector(SpawnLocation.Location.X + 100.f, SpawnLocation.Location.Y + 100.f, Player->GetActorLocation().Z - 88.f);
		auto SpawnSaveActor = GetWorld()->SpawnActor<ASaveActor>(TSaveActor, SpawnVector, FRotator::ZeroRotator);
		if (!SpawnSaveActor)
		{
			StageClear();
		}
		else
		{
			LevelNav->GetRandomPointInNavigableRadius(Player->GetActorLocation(), 500.0f, SpawnLocation);
			SpawnVector = FVector(SpawnLocation.Location.X+100.f, SpawnLocation.Location.Y+100.f, Player->GetActorLocation().Z - 88.f);
			auto SpawnActor = GetWorld()->SpawnActor<ALevelTransitionVolume>(SpawnPortal_TSub, SpawnVector, FRotator::ZeroRotator);
			if (!SpawnActor)
			{
				StageClear();
			}
			else
			{
				OnStageClear.Broadcast();
			}
		}

	}
}

void AABGameMode::SetPlayer(APlayerCharacter* TPlayer)
{
	Player = TPlayer;
}