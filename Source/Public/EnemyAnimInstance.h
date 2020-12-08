// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"
#include "Animation/AnimInstance.h"
#include "Servarog.h"
#include "EnemyAnimInstance.generated.h"

/**
 * 
 */

DECLARE_MULTICAST_DELEGATE(FOnActivateCollisionDelegate);
DECLARE_MULTICAST_DELEGATE(FOnDeActivateCollisionDelegate);
DECLARE_MULTICAST_DELEGATE(FOnAttackEndDelegate);
DECLARE_MULTICAST_DELEGATE(FOnDeathEndDelegate);
DECLARE_MULTICAST_DELEGATE(FOnEndBlockedDelegate);

/* Servarog */
DECLARE_MULTICAST_DELEGATE(FOnEndChangeDelegate);
DECLARE_MULTICAST_DELEGATE(FOnEndSkillDelegate);
DECLARE_MULTICAST_DELEGATE(FOnSkill1HitCheckDelegate);
DECLARE_MULTICAST_DELEGATE(FOnSummonDelegate);
DECLARE_MULTICAST_DELEGATE(FOnSummonDelegate2);

UCLASS()
class RPG_PJT_API UEnemyAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UEnemyAnimInstance();

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement", Meta=(AllowPrivateAccess=true))
	float CurrentPawnSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement", Meta = (AllowPrivateAccess = true))
	bool IsDead;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement", Meta = (AllowPrivateAccess = true))
	bool IsInAir;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement", Meta = (AllowPrivateAccess = true))
	class AEnemy* Enemy;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement", Meta = (AllowPrivateAccess = true))
	bool AttackBlocked;

	int32 AttackNum;
	int32 MAttackNum;

public:
	FOnActivateCollisionDelegate OnActivateCollision;
	FOnDeActivateCollisionDelegate OnDeActivateCollision;
	FOnActivateCollisionDelegate OnActivateCollision2;
	FOnDeActivateCollisionDelegate OnDeActivateCollision2;
	FOnActivateCollisionDelegate OnActivateCollision3;
	FOnDeActivateCollisionDelegate OnDeActivateCollision3;
	FOnActivateCollisionDelegate OnActivateCollision4;
	FOnDeActivateCollisionDelegate OnDeActivateCollision4;
	FOnActivateCollisionDelegate OnActivateCollision5;
	FOnDeActivateCollisionDelegate OnDeActivateCollision5;
	FOnAttackEndDelegate OnAttackEnd;
	FOnDeathEndDelegate OnDeathEnd;
	FOnEndBlockedDelegate OnBlockedEnd;

	UFUNCTION()
	void AnimNotify_ActivateCollision();

	UFUNCTION()
	void AnimNotify_DeActivateCollision();

	UFUNCTION()
		void AnimNotify_ActivateCollision2();

	UFUNCTION()
		void AnimNotify_DeActivateCollision2();

	UFUNCTION()
		void AnimNotify_ActivateCollision3();

	UFUNCTION()
		void AnimNotify_DeActivateCollision3();
	UFUNCTION()
		void AnimNotify_ActivateCollision4();

	UFUNCTION()
		void AnimNotify_DeActivateCollision4();
	UFUNCTION()
		void AnimNotify_ActivateCollision5();

	UFUNCTION()
		void AnimNotify_DeActivateCollision5();

	UFUNCTION()
	void AnimNotify_AttackEnd();

	UFUNCTION()
	void AnimNotify_DeathEnd();
	
	UFUNCTION()
	void AnimNotify_BlockedEnd();

	void PlayAttackMontage();
	void JumpToAttackMontageSection(int32 TotalATKNum);
	void JumpToAttackMontageSection_2(int32 TotalATKNum);
	void PlayMovingAttackMontage();
	void JumpToMovingAttackMontageSection(int32 TotalATKNum);
	
	void JumpToDeathMontageSection();
	void JumpToBlockMontageSection();

	void PlayBlockedMontage();

	FName GetAttackMontageSectionName(int32 Section);
	FName GetMAttackMontageSectionName(int32 Section);

	FORCEINLINE UAnimMontage* GetAttackMontage() { return AttackMontage; }

	void BindMontages(UAnimMontage* Attack, UAnimMontage* MAttack);


	/* Servarog Anim */
	void BindMontagesForServarog(UAnimMontage* Attack, UAnimMontage* MAttack, UAnimMontage* Skill, UAnimMontage* ChangePattern, UAnimMontage* HeroSkill);
	void BindMontagesForDragon(UAnimMontage* Attack);
	void PlaySkillMontage(FName CurrentPattern);
	void PlayChangePatternMontage(FName NextPattern);

	FOnEndChangeDelegate OnEndChange;
	FOnEndSkillDelegate OnEndSkill;
	FOnEndSkillDelegate OnEndSkill2;
	FOnSkill1HitCheckDelegate OnSkill1HitCheck;
	FOnSummonDelegate OnSummon;

	FOnSummonDelegate2 OnSummon2;
	FOnSummonDelegate2 OnSummon3;
	FOnSummonDelegate2 OnSummon4_1;
	FOnSummonDelegate2 OnSummon4_2;
	FOnSummonDelegate2 OnSummon5;
	FOnSummonDelegate2 OnFire;
	FOnSummonDelegate2 OnFireBall;
	FOnSummonDelegate2 OnFireBall_Air;
	FOnSummonDelegate2 OnFlyEnd;
	FOnSummonDelegate2 OnLandEnd;
	FOnSummonDelegate2 OnFlyStart;
	FOnSummonDelegate2 OnLandStart;

	UFUNCTION()
		void AnimNotify_EndChange();

	UFUNCTION()
		void AnimNotify_EndSkill();
	UFUNCTION()
		void AnimNotify_EndSkill2();

	UFUNCTION()
		void AnimNotify_Skill1HitCheck();
	
	UFUNCTION()
		void AnimNotify_Summon();

	UFUNCTION()
		void AnimNotify_Summon2();
	UFUNCTION()
		void AnimNotify_Summon3();
	UFUNCTION()
		void AnimNotify_Summon4_1();
	UFUNCTION()
		void AnimNotify_Summon4_2();
	UFUNCTION()
		void AnimNotify_Summon5();
	UFUNCTION()
		void AnimNotify_Fire();
	UFUNCTION()
		void AnimNotify_FireBall();
	UFUNCTION()
		void AnimNotify_FireBall_Air();
	UFUNCTION()
		void AnimNotify_FlyEnd();
	UFUNCTION()
		void AnimNotify_LandEnd();
	UFUNCTION()
		void AnimNotify_FlyStart();
	UFUNCTION()
		void AnimNotify_LandStart();

private:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	UAnimMontage* AttackMontage;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	UAnimMontage* MovingAttackMontage;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	UAnimMontage* SkillMontage;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
		UAnimMontage* HeroSkillMontage;
	
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
		UAnimMontage* ChangePatternMontage;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
		UAnimMontage* BlockedMontage;
};
