// Fill out your copyright notice in the Description page of Project Settings.


#include "ABAnimInstance.h"
#include "PlayerCharacter.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

UABAnimInstance::UABAnimInstance()
{
	CurrentPawnSpeed = 0.0f;
	IsInAir = false;
	IsDead = false;
}

void UABAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	
	auto Pawn = TryGetPawnOwner();
	if (::IsValid(Pawn) && !IsDead)
	{
		CurrentPawnSpeed = Pawn->GetVelocity().Size();
		Character = Cast<APlayerCharacter>(Pawn);
		if (Character)
		{
			IsInAir = Character->GetCharacterMovement()->IsFalling();
			IsEquipping = Character->IsEquipping();
			IsDead = Character->IsDead();
			IsTargeting = Character->IsTargeting();

			Axis_X = Character->GetInputAxisValue("MoveForward");
			Axis_Y = Character->GetInputAxisValue("MoveRight");
		}
	}
}

void UABAnimInstance::PlayAttackMontage()
{
	Montage_Play(AttackMontage, 1.0f);
}

void UABAnimInstance::PlayStrongAttackMontage()
{
	Montage_Play(StrongAttackMontage, 1.0f);
}

void UABAnimInstance::JumpToAttackMontageSection(int32 NewSection)
{
	Montage_JumpToSection(GetAttackMontageSectionName(NewSection), AttackMontage);
}

void UABAnimInstance::JumpToStrongAttackMontageSection(int32 NewSection)
{
	Montage_JumpToSection(GetAttackMontageSectionName(NewSection), StrongAttackMontage);
}

void UABAnimInstance::PlayRollMontage()
{	
	Montage_Play(RollMontage, 1.0f);
}

void UABAnimInstance::PlayBackDodgeMontage()
{
	Montage_Play(BackDodgeMontage, 1.0f);
}

void UABAnimInstance::PlayEquippingMontage()
{
	Montage_Play(EquippingMontage, 1.0f);
}

void UABAnimInstance::JumpToEquippingMontageSection()
{
	Montage_JumpToSection(FName("Equip"), EquippingMontage);
}

void UABAnimInstance::JumpToUnEquippingMontageSection()
{
	Montage_JumpToSection(FName("UnEquip"), EquippingMontage);

}

void UABAnimInstance::PlayBlockingMontage()
{
	Montage_Play(BlockingMontage, 1.0f);
}

void UABAnimInstance::JumpToBlockingMontageStartSection()
{
	Montage_JumpToSection(FName("Start"), BlockingMontage);
}

void UABAnimInstance::JumpToBlockingMontageLoopSection()
{
	Montage_JumpToSection(FName("Loop"), BlockingMontage);
}

void UABAnimInstance::JumpToBlockingMontageBlockSection()
{
	Montage_JumpToSection(FName("Guard"), BlockingMontage);
}

void UABAnimInstance::JumpToBlockingMontageDamageSection()
{
	Montage_JumpToSection(FName("Damage"), BlockingMontage);
}

void UABAnimInstance::JumpToBlockingMontageEndSection()
{
	Montage_JumpToSection(FName("End"), BlockingMontage);
}

void UABAnimInstance::PlayDamageMontage()
{
	Montage_Play(DamageOrDeadMontage, 1.0f);
	Montage_JumpToSection(FName("Damage"), DamageOrDeadMontage);
}

void UABAnimInstance::PlayDeadMontage()
{
	Montage_Play(DamageOrDeadMontage, 1.0f);
	Montage_JumpToSection(FName("Dead"), DamageOrDeadMontage);
}

void UABAnimInstance::PlaySavePoseMontage()
{
	Montage_Play(SavePoseMontage, 1.0f);
}

void UABAnimInstance::PlayDrinkingMontage()
{
	Montage_Play(DrinkingMontage, 1.0f);
}

void UABAnimInstance::AnimNotify_NextAttackCheck()
{
	OnNextAttackCheck.Broadcast();
}

void UABAnimInstance::AnimNotify_EndAttack()
{
	OnEndAttack.Broadcast();
}

void UABAnimInstance::AnimNotify_EndRolling()
{
	OnEndRolling.Broadcast();
}

void UABAnimInstance::AnimNotify_EndEquiping()
{
	OnEndEquiping.Broadcast();
}

void UABAnimInstance::AnimNotify_EndUnEquipping()
{
	OnEndUnEquipping.Broadcast();
}

void UABAnimInstance::AnimNotify_ChangeWeaponPosition()
{
	OnChangeWeaponPosition.Broadcast();
}

void UABAnimInstance::AnimNotify_ActivateCollision()
{
	OnActivateCollision.Broadcast();
}

void UABAnimInstance::AnimNotify_DeActivateCollision()
{
	OnDeActivateCollision.Broadcast();
}

void UABAnimInstance::AnimNotify_PlaySwingSound()
{
	OnPlaySwingSound.Broadcast();
}

void UABAnimInstance::AnimNotify_DamageEnd()
{
	OnDamageEnd.Broadcast();
}

void UABAnimInstance::AnimNotify_DeadEnd()
{
	OnDeadEnd.Broadcast();
}

void UABAnimInstance::AnimNotify_BlockingStart()
{
	OnBlockingStart.Broadcast();
}

void UABAnimInstance::AnimNotify_BlockingEnd()
{
	OnBlockingEnd.Broadcast();
}

void UABAnimInstance::AnimNotify_BlockDamageEnd()
{
	OnBlockDamageEnd.Broadcast();
}

void UABAnimInstance::AnimNotify_ShieldHitEnd()
{
	OnShieldHitEnd.Broadcast();
}

void UABAnimInstance::AnimNotify_DeActivateBlockCollision()
{
	OnDeActivateBlockCollision.Broadcast();
}

void UABAnimInstance::AnimNotify_LandingStart()
{
	OnLandingStart.Broadcast();
}

void UABAnimInstance::AnimNotify_JumpEnd()
{
	OnJumpEnd.Broadcast();
}

void UABAnimInstance::AnimNotify_EndSavePose()
{
	OnEndSavePose.Broadcast();
}

void UABAnimInstance::AnimNotify_Healing()
{
	OnHealing.Broadcast();
}

void UABAnimInstance::AnimNotify_DrinkingEnd()
{
	OnDrinkingEnd.Broadcast();
}

void UABAnimInstance::AnimNotify_Fire()
{
	OnFire.Broadcast();
}

FName UABAnimInstance::GetAttackMontageSectionName(int32 Section)
{
	return FName(*FString::Printf(TEXT("Attack%d"), Section));
}

void UABAnimInstance::SetBaseAnimMode()
{
	RollMontage = OneHandRollingMontage;
	DamageOrDeadMontage = OneHandDamageOrDeadMontage;
	BackDodgeMontage = OneHandBackDodgeMontage;
}