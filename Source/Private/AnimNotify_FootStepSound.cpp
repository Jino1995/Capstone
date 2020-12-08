// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_FootStepSound.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Animation/AnimSequenceBase.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Sound/SoundBase.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "FootStepSoundList.h"



UAnimNotify_FootStepSound::UAnimNotify_FootStepSound() : Super()
{
	CheckDistance = 10.0f;
	SoundAttach = false;
	ShowDebugLine = false;
}


void UAnimNotify_FootStepSound::Notify(class USkeletalMeshComponent* MeshComp, class UAnimSequenceBase* Animation)
{
	FootStep(MeshComp);
}

void UAnimNotify_FootStepSound::FootStep(USkeletalMeshComponent* MeshComp)
{
	if (SoundList_TS == nullptr) return;
	SoundList = Cast<UFootStepSoundList>(SoundList_TS->GetDefaultObject());
	if (SoundList == nullptr) return;

	const FTransform MeshTransform = MeshComp->GetSocketTransform(SocketName);

	FVector LineTrace_Start = MeshTransform.GetLocation();
	FVector LineTrace_End = MeshTransform.GetLocation() - FVector::UpVector*CheckDistance;

	FHitResult HitResult;
	TArray<AActor*> Ignore;
	Ignore.Add(MeshComp->GetOwner());

	bool bDebug = false;
	bDebug = ShowDebugLine;

	EDrawDebugTrace::Type eDebug = EDrawDebugTrace::None;
	if (bDebug == true) eDebug = EDrawDebugTrace::ForDuration;

	bool bResult = UKismetSystemLibrary::LineTraceSingle(MeshComp->GetWorld(), LineTrace_Start, LineTrace_End,
		UEngineTypes::ConvertToTraceType(ECC_WorldStatic), true, Ignore, eDebug, HitResult, true);

	if (bResult == true)
	{
		if (HitResult.GetActor() == nullptr) return;

		USoundBase* pSound = SoundList->GetSound(HitResult.PhysMaterial->GetName());
		if (pSound == nullptr) return;

		if (pSound->IsLooping() == true)
		{
			UE_LOG(LogTemp, Warning, TEXT("This Notify Sound must be turn off Loop"));
			return;
		}

		if (SoundAttach)
		{
			UGameplayStatics::SpawnSoundAttached(pSound, MeshComp, SocketName, FVector(ForceInit), EAttachLocation::SnapToTarget, false, VolumeMultiplier, PitchMultiplier);
		}
		else
		{
			UGameplayStatics::PlaySoundAtLocation(MeshComp->GetWorld(), pSound, MeshComp->GetComponentLocation(), VolumeMultiplier, PitchMultiplier);
		}
	}

}
