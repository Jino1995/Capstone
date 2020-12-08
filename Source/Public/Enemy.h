// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"
#include "GameFramework/Character.h"
#include "Enemy.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnHPChangedDelegate);

UENUM(BlueprintType)
enum class EEnemyMovementStatus :uint8
{
	EMS_Idle				UMETA(DeplayName = "Idle"),
	EMS_Patrol				UMETA(DeplayName = "Patrol"),
	EMS_MoveToTarget		UMETA(DeplayName = "MoveToTarget"),
	EMS_Attacking			UMETA(DeplayName = "Attacking"),
	EMS_MovingAttacking		UMETA(DeplayName = "MovingAttacking"),
	EMS_Skill				UMETA(DeplayName = "Skill"),
	EMS_ChangePattern		UMETA(DeplayName = "ChangePattern"),
	EMS_Dead				UMETA(DeplayName = "Dead"),
	EMS_MAX					UMETA(DeplayName = "DefaultMax"),
};

UENUM(BlueprintType)
enum class EMonsterType :uint8
{
	Normal,
	Boss
};

UCLASS()
class RPG_PJT_API AEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemy();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	EEnemyMovementStatus EnemyMovementStatus;

	FORCEINLINE void SetEnemyMovementStatus(EEnemyMovementStatus Status) { EnemyMovementStatus = Status; }
	FORCEINLINE EEnemyMovementStatus GetEnemyMovementStatus() { return EnemyMovementStatus; }

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	class USphereComponent* AgroSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	USphereComponent* CombatSphere;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	class AAIController* AIController;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float Health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float MaxHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float EnemyDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float IsDead;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	class UParticleSystem* HitParticles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	class USoundCue* HitSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	USoundCue* SwingSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	class UBoxComponent* CombatCollision;

	UPROPERTY(VisibleAnywhere, Category = "Weapon")
	UStaticMeshComponent* EnemyWeapon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float DeathDelay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Type", Meta=(AllowPrivateAccess=true))
	EMonsterType MonsterType;

	bool bStrafing;

	FOnHPChangedDelegate OnHPChanged;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void PostInitializeComponents() override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void MoveToTarget(class APlayerCharacter* Target);

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI")
	bool bOverlappingCombatSphere;

	void ActivateCollision();
	void DeActivateCollision();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bAttacking;

	virtual void Attack();

	virtual void AttackEnd();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
	class UAnimMontage* CombatMontage;

	FTimerHandle DeathTimer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float AttackMinTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float AttackMaxTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	TSubclassOf<UDamageType> DamageTypeClass;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	
	virtual void Die();

	bool Alive();

	void Disappear();

	bool GetIsStrafing();

protected:
	UPROPERTY()
	class UEnemyAnimInstance* ABAnim;

	UPROPERTY(VisibleAnywhere, Category="ABC", Meta=(AllowPrivateAccess=true))
	class APlayerCharacter* TargetE;

	UPROPERTY(VisibleAnywhere, Category = "ABC", Meta = (AllowPrivateAccess = true))
	class APlayerCharacter* HuntTarget;

	UPROPERTY(VisibleAnywhere, Category = UI)
	class UWidgetComponent* HPBarWidget;

	UPROPERTY(VisibleAnywhere, Category = UI)
	class UWidgetComponent* DamageText;

	UPROPERTY(VisibleAnywhere, Category = UI)
	class UWidgetComponent* TargetPointWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stat", Meta=(AllowPrivateAccess=true))
	float Exp;

	
public:
	UPROPERTY(VisibleAnywhere, Category = "ABC", Meta = (AllowPrivateAccess = true))
	class APlayerCharacter* AgroTarget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Combat")
	bool bBlocked;

	
	void BlockedAttack();
	void SuccessAttack(APlayerCharacter* DamagedPlayer);

	void ShowTargetPoint(bool bShow);

	void ShowDamageText(int32 Damage);
	void ShowMoneyText(int32 Money);
};
