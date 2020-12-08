// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy.h"
#include "AIController.h"
#include "Servarog.generated.h"

UENUM(BlueprintType)
enum class EServarogPattern :uint8
{
	PTN_Warrior,
	PTN_Mage,	
	PTN_Summoner,
	EMS_Hero	
};
/**
 * 
 */
UCLASS()
class RPG_PJT_API AServarog : public AEnemy
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AttackMontage, Meta = (AllowPrivateAccess = true))
	UAnimMontage* AttackMTG;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AttackMontage, Meta = (AllowPrivateAccess = true))
	UAnimMontage* MovingAttackMTG;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AttackMontage, Meta = (AllowPrivateAccess = true))
	UAnimMontage* SkillMTG;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AttackMontage, Meta = (AllowPrivateAccess = true))
	UAnimMontage* HeroSkillMTG;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AttackMontage, Meta = (AllowPrivateAccess = true))
	UAnimMontage* ChangePatternMTG;

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Patrol, Meta = (AllowPrivateAccess = true))
		float MaxSkillCoolTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Patrol, Meta = (AllowPrivateAccess = true))
		float MinSkillCoolTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Patrol, Meta = (AllowPrivateAccess = true))
		float MaxChangePatternTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Patrol, Meta = (AllowPrivateAccess = true))
		float MinChangePatternTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AttackMontage, Meta = (AllowPrivateAccess = true))
		int32 MovingAttackActivationProbability;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Speed, Meta = (AllowPrivateAccess = true))
		int32 PatrolSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Speed, Meta = (AllowPrivateAccess = true))
		int32 HuntSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Pattern, Meta = (AllowPrivateAccess = true))
	int32 PatternNum;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Pattern, Meta = (AllowPrivateAccess = true))
	EServarogPattern CurrentPattern;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Patterns", Meta = (AllowPrivateAccess = true))
	USkeletalMeshComponent* BodyParts;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Patterns", Meta = (AllowPrivateAccess = true))
		USkeletalMesh* WarriorPattern;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Patterns", Meta = (AllowPrivateAccess = true))
		USkeletalMesh* MagePatterns;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Patterns", Meta = (AllowPrivateAccess = true))
		USkeletalMesh* SummonerPattern;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Patterns", Meta = (AllowPrivateAccess = true))
		USkeletalMesh* HeroPattern;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpawnMonsters", Meta = (AllowPrivateAccess = true))
	TSubclassOf<AEnemy> SpawnMonsterList;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpawnMonsters", Meta = (AllowPrivateAccess = true))
	TSubclassOf<AEnemy> SpawnMonsterList2;

	struct FPathFollowingRequestResult PatrolResult;
	struct FPathFollowingRequestResult HuntResult;

	bool bAttackCoolTime;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	AServarog();
	virtual void Tick(float DeltaTime) override;
	virtual void PostInitializeComponents() override;

	bool herochanged = false;

	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	TSubclassOf<class Afire> ProjectileClass;
	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	TSubclassOf<class Afire2_right> ProjectileClass2;
	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	TSubclassOf<class Afire2_left> ProjectileClass3;
	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	TSubclassOf<class Afire3> ProjectileClass4;

	void ActivateCollision2();

	void DeActivateCollision2();

	void ActivateCollision3();

	void DeActivateCollision3();

	FTimerHandle AttackTimer;
	FTimerHandle PatrolTimer;
	FTimerHandle HuntTimer;
	FTimerHandle AttackCoolTimeTimer;
	FTimerHandle MAttackTimer;

	FTimerHandle ChangePatternTimer;
	FTimerHandle SkillTimer;

	void SelectAttack();

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	class UBoxComponent* CombatCollision2;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	class USphereComponent* CombatCollision3;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float HeroChangeHealth = 50;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	void TurnOnAttackCoolTime();

	virtual void Die() override;

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

	Afire3* Projectile4[5];
	int firenum = 0;
};
