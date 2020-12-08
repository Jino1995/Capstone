// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy.h"
#include "Dragon.generated.h"

/**
 * 
 */
UCLASS()
class RPG_PJT_API ADragon : public AEnemy
{
	GENERATED_BODY()
	
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AttackMontage, Meta = (AllowPrivateAccess = true))
	UAnimMontage* AttackMTG;

	float DeltaTime2;

	bool bAttackCoolTime;
	bool InCombatSphere;
	bool InAgroSphere2;
	bool InAgroSphere3;
	bool IsPatternPlaying;
	bool MeleeAttackOn;
	bool FireBreathOn;
	bool IsTurnning;
	bool IsInGround;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI, Meta = (AllowPrivateAccess = true))
	bool IsInAir;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	ADragon();
	virtual void Tick(float DeltaTime) override;
	virtual void PostInitializeComponents() override;

	bool Alive();

	bool herochanged = false;

	UPROPERTY(BlueprintReadWrite)
	float aaaa;

	void ActivateCollision2();
	void DeActivateCollision2();
	void ActivateCollision3();
	void DeActivateCollision3();
	void ActivateCollision4();
	void DeActivateCollision4();
	void ActivateCollision5();
	void DeActivateCollision5();
	FTimerHandle AttackTimer;
	FTimerHandle PatrolTimer;
	FTimerHandle HuntTimer;
	FTimerHandle AttackCoolTimeTimer;
	FTimerHandle MAttackTimer;
	FTimerHandle MoveTimer;
	FTimerHandle ChangePatternTimer;
	FTimerHandle SkillTimer;
	FTimerHandle DamageTimer;
	FTimerHandle TurnningWaitTimer;
	FTimerHandle FlyTimer;
	FTimerHandle FlyUpTimer;
	FTimerHandle AttackWaitTimer;	//조금 더 접근

	void SelectAttack();

	virtual void Attack() override;
	void Attack_right();
	void Attack_left();
	void MovingAttack();
	virtual void AttackEnd() override;
	void MoveTimeOut();

	void StartToPatrol();
	void MoveToPatrol();
	void CheckToTargetInHuntRange();
	void MoveToTarget();
	void Pattern_Selector();
	void TurnEnd();
	void FireBreath();
	void FireBall();

	void FireBall_Air();

	void FireFireBall();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	class USphereComponent* AgroSphere2;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	class USphereComponent* AgroSphere3;

	UFUNCTION()
	virtual void AgroSphereOnOverlapBegin_2(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	virtual void AgroSphereOnOverlapEnd_2(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	UFUNCTION()
	virtual void AgroSphereOnOverlapBegin_3(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	virtual void AgroSphereOnOverlapEnd_3(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);



	UFUNCTION()
	virtual void CombatSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	virtual void CombatSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	virtual void CombatCollisionOnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	virtual void CombatCollisionOnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	UFUNCTION()
	virtual void CombatCollisionOnOverlapBegin2(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	virtual void CombatCollisionOnOverlapEnd2(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	UFUNCTION()
	virtual void CombatCollisionOnOverlapBegin3(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void FlyToAir();
	void HeightIncrease();
	void HeightDecrease();
	void FlyToAir_End();
	void LandToGround();
	void LandToGround_End();
	void Damage();

	UFUNCTION()
		virtual void CombatCollisionOnOverlapEnd3(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	APlayerCharacter* Player;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
		class UBoxComponent* CombatCollision2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
		class USphereComponent* CombatCollision3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
		class USphereComponent* CombatCollision4;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
		class UBoxComponent* CombatCollision5;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
		float HeroChangeHealth = 50;

	UPROPERTY(EditDefaultsOnly, Category = Projectile)
		TSubclassOf<class AFireBall> ProjectileClass;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	void TurnOnAttackCoolTime();

	virtual void Die() override;

	void FireFireBall_Air();

	void MoveToTargetForMovingAttack();

	void PlaySkill();
	void ChangePattern();

	void SummonMonster();

	void SummonMonster_Hero();

	void SummonAttack_right();
	void SummonAttack_left();

	void SummonFire();

	void SummonFireStrom();

	void SendFireStrom();

	int firenum = 0;

	FORCEINLINE bool GetIsInAir() { return IsInAir; }
};

