// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy.h"
#include "AIController.h"
#include "NormalEnemy.generated.h"

/**
 * 
 */
UCLASS()
class RPG_PJT_API ANormalEnemy : public AEnemy
{
	GENERATED_BODY()
	

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AttackMontage, Meta = (AllowPrivateAccess = true))
	UAnimMontage* AttackMTG;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AttackMontage, Meta = (AllowPrivateAccess = true))
	UAnimMontage* MovingAttackMTG;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AttackMontage, Meta = (AllowPrivateAccess = true))
	int32 AttackNum;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AttackMontage, Meta = (AllowPrivateAccess = true))
	int32 MovingAttackNum;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Patrol, Meta = (AllowPrivateAccess = true))
	TArray<FVector> PatrolPoints;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Patrol, Meta = (AllowPrivateAccess = true))
	int32 CurrentPatrolNum = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Patrol, Meta = (AllowPrivateAccess = true))
	float CheckPatrolTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Hunt, Meta = (AllowPrivateAccess = true))
	float CheckHuntZoneTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Patrol, Meta = (AllowPrivateAccess = true))
	float MaxAttackCoolTime;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Patrol, Meta = (AllowPrivateAccess = true))
	float MinAttackCoolTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AttackMontage, Meta = (AllowPrivateAccess = true))
	int32 MovingAttackActivationProbability;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Speed, Meta = (AllowPrivateAccess = true))
	int32 PatrolSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Speed, Meta = (AllowPrivateAccess = true))
	int32 HuntSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Reward, Meta = (AllowPrivateAccess = true))
	int32 Money;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon, Meta = (AllowPrivateAccess = true))
	UStaticMeshComponent* EnemyWeapon1;

	struct FPathFollowingRequestResult PatrolResult;
	struct FPathFollowingRequestResult HuntResult;

	bool bAttackCoolTime;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	ANormalEnemy();
	virtual void Tick(float DeltaTime) override;
	virtual void PostInitializeComponents() override;

	FTimerHandle AttackTimer;
	FTimerHandle PatrolTimer;
	FTimerHandle HuntTimer;
	FTimerHandle AttackCoolTimeTimer;
	FTimerHandle MAttackTimer;
	FTimerHandle ParticleOffTimer;
	void SelectAttack();

	class UParticleSystemComponent* ParticleSystem1;
	
	virtual void Attack() override;
	void MovingAttack();
	virtual void AttackEnd() override;

	void StartToPatrol();
	void MoveToPatrol();
	void CheckToTargetInHuntRange();
	void MoveToTarget();

	UFUNCTION()
	virtual void AgroSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	virtual void AgroSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	virtual void CombatSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	virtual void CombatSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	virtual void CombatCollisionOnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	virtual void CombatCollisionOnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	void TurnOnAttackCoolTime();

	virtual void Die() override;

	void MoveToTargetForMovingAttack();

	void ParticleOn();
	void ParticleOff();
};
