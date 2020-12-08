// Fill out your copyright notice in the Description page of Project Settings.


#include "ABGameInstance.h"

UABGameInstance::UABGameInstance()
{
	static ConstructorHelpers::FObjectFinder<UDataTable> DT_ABCHARACTER(TEXT("/Game/Blueprint/item/ItemDataTable.ItemDataTable"));
	if (DT_ABCHARACTER.Succeeded())
	{
		ItemTable = DT_ABCHARACTER.Object;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Name : %s , DataTable loaded fail."), *GetName());
	}

	static ConstructorHelpers::FObjectFinder<UDataTable> STAT_DT(TEXT("/Game/Blueprint/CharacterStat/CharacterDataTable.CharacterDataTable"));
	if (STAT_DT.Succeeded())
	{
		CharacterStatTable = STAT_DT.Object;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Name : %s , DataTable loaded fail."), *GetName());
	}

	static ConstructorHelpers::FObjectFinder<UDataTable> STAGEINFO_DT(TEXT("/Game/Blueprint/StageInfo/StageInfoDataTable.StageInfoDataTable"));
	if (STAGEINFO_DT.Succeeded())
	{
		StageInfoTable = STAGEINFO_DT.Object;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Name : %s , DataTable loaded fail."), *GetName());
	}
}

void UABGameInstance::Init()
{
	Super::Init();
}

FItemData* UABGameInstance::GetItemData(FName ItemName)
{
	return ItemTable->FindRow<FItemData>(ItemName, TEXT(""));
}

FCharacterData* UABGameInstance::GetCharacterData(int32 Level)
{
	return CharacterStatTable->FindRow<FCharacterData>(*FString::FromInt(Level), TEXT(""));
}

FStageInfo* UABGameInstance::GetStageInfo(FString StageName)
{
	return StageInfoTable->FindRow<FStageInfo>(FName(*StageName), TEXT(""));
}

