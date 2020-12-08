// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"
#include "GameFramework/Actor.h"
#include "Sound/SoundCue.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Skill.generated.h"

UCLASS()
class RPG_PJT_API ASkill : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASkill();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
	USkeletalMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere, Category = Movement)
	UProjectileMovementComponent* ProjectileMovementComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
	USphereComponent* CollisionRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
	USoundCue* SkillHitSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
	UParticleSystem* SkillParticle;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", Meta=(AllowPrivateAccess=true))
	TSubclassOf<UDamageType> DamageTypeClass;

	UPROPERTY()
	AController* SkillInstigator;

	UPROPERTY()
	class APlayerCharacter* SkillOwner;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack", Meta = (AllowPrivateAccess = true))
	float SkillDamagePercent;

public:
	FTimerHandle DestroyTimer;

	UFUNCTION()
	virtual void CombaOnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void CombaOnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	FORCEINLINE void SetSkillOwner(APlayerCharacter* OwnerCharacter) { SkillOwner = OwnerCharacter; }
	FORCEINLINE void SetInstigator(AController* Inst) { SkillInstigator = Inst; }

	void FireInDirection(const FVector& ShootDirection);

	void Dead();
};
