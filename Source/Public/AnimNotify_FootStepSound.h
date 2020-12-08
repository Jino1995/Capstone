// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_FootStepSound.generated.h"

/**
 * 
 */
UCLASS(const, hidecategories = Object, collapsecategories, meta = (DisplayName = "Play FootStep Sound"))
class RPG_PJT_API UAnimNotify_FootStepSound : public UAnimNotify
{
	GENERATED_BODY()
		
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify", meta = (DisplayName = "PMB Sound List"))
		TSubclassOf<class UFootStepSoundList> SoundList_TS;

	UPROPERTY()
		class UFootStepSoundList* SoundList;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify", meta = (ExposeOnSpawn = true))
		float VolumeMultiplier = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify", meta = (ExposeOnSpawn = true))
		float PitchMultiplier = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify")
		uint32 SoundAttach : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify")
		float CheckDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify")
		FName SocketName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify")
		uint32 ShowDebugLine : 1;

public:
	UAnimNotify_FootStepSound();
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

public:
	void FootStep(USkeletalMeshComponent* MeshComp);
	
};
