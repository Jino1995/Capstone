// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"
#include "Animation/AnimInstance.h"
#include "ABAnimInstance.generated.h"

/**
 * 
 */

DECLARE_MULTICAST_DELEGATE(FOnFireDelegate);

DECLARE_MULTICAST_DELEGATE(FOnNextAttackCheckDelegate);
DECLARE_MULTICAST_DELEGATE(FOnEndAttackDelegate);

DECLARE_MULTICAST_DELEGATE(FOnEndRollingDelegate);

DECLARE_MULTICAST_DELEGATE(FOnEndEquipingDelegate);
DECLARE_MULTICAST_DELEGATE(FOnEndUnEquippingDelegate);

DECLARE_MULTICAST_DELEGATE(FOnChangeWeaponPositionDelegate);

DECLARE_MULTICAST_DELEGATE(FOnActivateCollisionDelegate);
DECLARE_MULTICAST_DELEGATE(FOnDeActivateCollisionDelegate);

DECLARE_MULTICAST_DELEGATE(FOnPlaySwingSoundDelegate);

DECLARE_MULTICAST_DELEGATE(FOnBlockingStartDelegate);
DECLARE_MULTICAST_DELEGATE(FOnBlockingEndDelegate);
DECLARE_MULTICAST_DELEGATE(FOnBlockDamageEndDelegate);
DECLARE_MULTICAST_DELEGATE(FOnShieldHitEndDelegate);

DECLARE_MULTICAST_DELEGATE(FOnDeActivateBlockCollisionDelegate);

DECLARE_MULTICAST_DELEGATE(FOnLandingStartDelegate);
DECLARE_MULTICAST_DELEGATE(FOnJumpEndDelegate);

DECLARE_MULTICAST_DELEGATE(FOnDeadEndDelegate);
DECLARE_MULTICAST_DELEGATE(FOnDamageEndDelegate);

DECLARE_MULTICAST_DELEGATE(FOnEndSavePoseDelegate);

DECLARE_MULTICAST_DELEGATE(FOnHealingDelegate);
DECLARE_MULTICAST_DELEGATE(FOnDrinkingEndDelegate);


UCLASS()
class RPG_PJT_API UABAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UABAnimInstance();

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	void PlayAttackMontage();
	void PlayStrongAttackMontage();
	void JumpToAttackMontageSection(int32 NewSection);
	void JumpToStrongAttackMontageSection(int32 NewSection);

	void PlayRollMontage();
	
	void PlayBackDodgeMontage();

	void PlayEquippingMontage();
	void JumpToEquippingMontageSection();
	void JumpToUnEquippingMontageSection();


	void PlayBlockingMontage();
	void JumpToBlockingMontageStartSection();
	void JumpToBlockingMontageLoopSection();
	void JumpToBlockingMontageBlockSection();
	void JumpToBlockingMontageDamageSection();
	void JumpToBlockingMontageEndSection();

	void PlayDamageMontage();
	void PlayDeadMontage();

	void PlaySavePoseMontage();

	void PlayDrinkingMontage();

public:
	FOnNextAttackCheckDelegate OnNextAttackCheck;
	FOnEndAttackDelegate OnEndAttack;
	FOnEndRollingDelegate OnEndRolling;
	FOnEndEquipingDelegate OnEndEquiping;
	FOnEndUnEquippingDelegate OnEndUnEquipping;
	FOnChangeWeaponPositionDelegate OnChangeWeaponPosition;
	FOnActivateCollisionDelegate OnActivateCollision;
	FOnDeActivateCollisionDelegate OnDeActivateCollision;
	FOnPlaySwingSoundDelegate OnPlaySwingSound;
	FOnBlockingStartDelegate OnBlockingStart;
	FOnBlockingEndDelegate OnBlockingEnd;
	FOnBlockDamageEndDelegate OnBlockDamageEnd;
	FOnShieldHitEndDelegate OnShieldHitEnd;
	FOnDeActivateBlockCollisionDelegate OnDeActivateBlockCollision;
	FOnLandingStartDelegate OnLandingStart;
	FOnJumpEndDelegate OnJumpEnd;
	FOnDamageEndDelegate OnDamageEnd;
	FOnDeadEndDelegate OnDeadEnd;
	FOnEndSavePoseDelegate OnEndSavePose;
	FOnHealingDelegate OnHealing;
	FOnDrinkingEndDelegate OnDrinkingEnd;
	FOnFireDelegate OnFire;

public:
	UFUNCTION()
	void AnimNotify_NextAttackCheck();

	UFUNCTION()
	void AnimNotify_EndAttack();

	UFUNCTION()
	void AnimNotify_EndRolling();

	UFUNCTION()
	void AnimNotify_EndEquiping();

	UFUNCTION()
	void AnimNotify_EndUnEquipping();

	UFUNCTION()
	void AnimNotify_ChangeWeaponPosition();

	UFUNCTION()
	void AnimNotify_ActivateCollision();

	UFUNCTION()
	void AnimNotify_DeActivateCollision();

	UFUNCTION()
	void AnimNotify_PlaySwingSound();

	UFUNCTION()
	void AnimNotify_BlockingStart();

	UFUNCTION()
	void AnimNotify_BlockingEnd();
	
	UFUNCTION()
	void AnimNotify_BlockDamageEnd();

	UFUNCTION()
	void AnimNotify_ShieldHitEnd();

	UFUNCTION()
	void AnimNotify_DeActivateBlockCollision();

	UFUNCTION()
	void AnimNotify_LandingStart();

	UFUNCTION()
	void AnimNotify_JumpEnd();

	UFUNCTION()
	void AnimNotify_DamageEnd();

	UFUNCTION()
	void AnimNotify_DeadEnd();

	UFUNCTION()
	void AnimNotify_EndSavePose();

	UFUNCTION()
	void AnimNotify_Healing();

	UFUNCTION()
	void AnimNotify_DrinkingEnd();

	UFUNCTION()
	void AnimNotify_Fire();

	FName GetAttackMontageSectionName(int32 Section);

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pawn", Meta = (AllowPrivateAccess = true))
	float CurrentPawnSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pawn", Meta = (AllowPrivateAccess = true))
	bool IsInAir;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pawn", Meta = (AllowPrivateAccess = true))
	bool IsEquipping;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pawn", Meta = (AllowPrivateAccess = true))
	bool IsDead;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pawn", Meta = (AllowPrivateAccess = true))
	bool IsTargeting;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pawn", Meta = (AllowPrivateAccess = true))
	float Axis_X;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pawn", Meta = (AllowPrivateAccess = true))
	float Axis_Y;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pawn", Meta = (AllowPrivateAccess = true))
	class APlayerCharacter* Character;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Montage, Meta = (AllowPrivateAccess = true))
	UAnimMontage* AttackMontage;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Montage, Meta = (AllowPrivateAccess = true))
	UAnimMontage* StrongAttackMontage;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Montage, Meta = (AllowPrivateAccess = true))
	UAnimMontage* EquippingMontage;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Montage, Meta = (AllowPrivateAccess = true))
	UAnimMontage* BlockingMontage;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Montage, Meta = (AllowPrivateAccess = true))
	UAnimMontage* RollMontage;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Montage, Meta = (AllowPrivateAccess = true))
	UAnimMontage* BackDodgeMontage;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Montage, Meta = (AllowPrivateAccess = true))
	UAnimMontage* DamageOrDeadMontage;


private:
	/**
	* OneHand Common AnimMontage
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = OneHand, Meta = (AllowPrivateAccess = true))
	UAnimMontage* OneHandRollingMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = OneHand, Meta = (AllowPrivateAccess = true))
	UAnimMontage* OneHandDamageOrDeadMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = OneHand, Meta = (AllowPrivateAccess = true))
	UAnimMontage* OneHandBackDodgeMontage;
	//

	/**
	* TwoHand Common AnimMontage
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = TwoHand, Meta = (AllowPrivateAccess = true))
	UAnimMontage* TwoHandRollingMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = TwoHand, Meta = (AllowPrivateAccess = true))
	UAnimMontage* TwoHandDamageOrDeadMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = TwoHand, Meta = (AllowPrivateAccess = true))
	UAnimMontage* TwoHandBackDodgeMontage;
	//

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Montage, Meta = (AllowPrivateAccess = true))
	UAnimMontage* SavePoseMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Montage, Meta = (AllowPrivateAccess = true))
	UAnimMontage* DrinkingMontage;

public:
	void SetBaseAnimMode();

	FORCEINLINE void SetAttackMontage(UAnimMontage* NewMontage) { AttackMontage = NewMontage; }
	FORCEINLINE void SetStrongAttackMontage(UAnimMontage* NewMontage) { StrongAttackMontage = NewMontage; }
	FORCEINLINE void SetBlockingMontage(UAnimMontage* NewMontage) { BlockingMontage = NewMontage; }
	FORCEINLINE void SetEquippingMontage(UAnimMontage* NewMontage) { EquippingMontage = NewMontage; }

	FORCEINLINE UAnimMontage* GetBlockingMontage() { return BlockingMontage; }
};
