// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAnimInstance.h"
#include "Enemy.h"
#include "Dragon.h"

UEnemyAnimInstance::UEnemyAnimInstance()
{

	AttackBlocked = false;
}

void UEnemyAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	auto Pawn = TryGetPawnOwner();
	
	Enemy = Cast<AEnemy>(Pawn);
	if (Enemy)
	{
		auto DEnemy = Cast<ADragon>(Pawn);
		if (DEnemy)
		{
			IsInAir = DEnemy->GetIsInAir();
		}
		CurrentPawnSpeed = Enemy->GetVelocity().Size();
		IsDead = Enemy->IsDead;
		AttackBlocked = Enemy->bBlocked;
	}
}

void UEnemyAnimInstance::AnimNotify_ActivateCollision()
{
	OnActivateCollision.Broadcast();
}

void UEnemyAnimInstance::AnimNotify_DeActivateCollision()
{
	OnDeActivateCollision.Broadcast();
}

void UEnemyAnimInstance::AnimNotify_ActivateCollision2()
{
	OnActivateCollision2.Broadcast();
}
void UEnemyAnimInstance::AnimNotify_DeActivateCollision2()
{
	OnDeActivateCollision2.Broadcast();
}
void UEnemyAnimInstance::AnimNotify_ActivateCollision3()
{
	OnActivateCollision3.Broadcast();
}

void UEnemyAnimInstance::AnimNotify_DeActivateCollision3()
{
	OnDeActivateCollision3.Broadcast();
}
void UEnemyAnimInstance::AnimNotify_ActivateCollision4()
{
	OnActivateCollision4.Broadcast();
}

void UEnemyAnimInstance::AnimNotify_DeActivateCollision4()
{
	OnDeActivateCollision4.Broadcast();
}

void UEnemyAnimInstance::AnimNotify_ActivateCollision5()
{
	OnActivateCollision5.Broadcast();
}

void UEnemyAnimInstance::AnimNotify_DeActivateCollision5()
{
	OnDeActivateCollision5.Broadcast();
}

void UEnemyAnimInstance::AnimNotify_AttackEnd()
{
	OnAttackEnd.Broadcast();
}

void UEnemyAnimInstance::AnimNotify_DeathEnd()
{
	OnDeathEnd.Broadcast();
}

void UEnemyAnimInstance::AnimNotify_BlockedEnd()
{
	OnBlockedEnd.Broadcast();
}

void UEnemyAnimInstance::PlayAttackMontage()
{
	Montage_Play(AttackMontage, 1.0f);
}

void UEnemyAnimInstance::JumpToAttackMontageSection(int32 TotalATKNum)
{
	Montage_JumpToSection(GetAttackMontageSectionName(rand() % TotalATKNum), AttackMontage);
}

void UEnemyAnimInstance::JumpToAttackMontageSection_2(int32 Num)
{
	Montage_JumpToSection(GetAttackMontageSectionName(Num), AttackMontage);
}

void UEnemyAnimInstance::PlayMovingAttackMontage()
{
	Montage_Play(MovingAttackMontage, 1.0f);
}
void UEnemyAnimInstance::JumpToMovingAttackMontageSection(int32 TotalATKNum)
{
	Montage_JumpToSection(FName(*FString::Printf(TEXT("%d"), TotalATKNum)), MovingAttackMontage);
}

void UEnemyAnimInstance::JumpToDeathMontageSection()
{
	Montage_JumpToSection(FName("Death"), AttackMontage);
}

void UEnemyAnimInstance::JumpToBlockMontageSection()
{
	Montage_JumpToSection(FName("Block"), AttackMontage);
}

void UEnemyAnimInstance::PlayBlockedMontage()
{
	Montage_Play(BlockedMontage, 1.0f);
}

void UEnemyAnimInstance::BindMontages(UAnimMontage* Attack, UAnimMontage* MAttack)
{
	AttackMontage = Attack;
	MovingAttackMontage = MAttack;
}

void UEnemyAnimInstance::BindMontagesForServarog(UAnimMontage* Attack, UAnimMontage* MAttack, UAnimMontage* Skill, UAnimMontage* ChangePattern, UAnimMontage* HeroSkill)
{
	AttackMontage = Attack;
	MovingAttackMontage = MAttack;
	SkillMontage = Skill;
	ChangePatternMontage = ChangePattern;
	HeroSkillMontage = HeroSkill;
}

void UEnemyAnimInstance::BindMontagesForDragon(UAnimMontage* Attack)
{
	AttackMontage = Attack;
}

FName UEnemyAnimInstance::GetAttackMontageSectionName(int32 Section)
{
	Section++;
	return FName(*FString::Printf(TEXT("Attack%d"), Section));
}

FName UEnemyAnimInstance::GetMAttackMontageSectionName(int32 Section)
{
	Section++;
	return FName(*FString::Printf(TEXT("MAttack%d"), Section));
}

void UEnemyAnimInstance::PlaySkillMontage(FName CurrentPattern)
{
	if (CurrentPattern == FName("Hero1"))
	{
		UE_LOG(LogTemp, Error, TEXT("heroSkill"));
		Montage_Play(HeroSkillMontage, 1.0f);
		Montage_JumpToSection("0", HeroSkillMontage);
	}
	else if (CurrentPattern == FName("Hero2")) 
	{
		UE_LOG(LogTemp, Error, TEXT("heroSkill"));
		Montage_Play(HeroSkillMontage, 1.0f);
		Montage_JumpToSection("1", HeroSkillMontage); //1
	}
	else if (CurrentPattern == FName("Hero3")) {
		Montage_Play(HeroSkillMontage, 1.0f);
		Montage_JumpToSection("2", HeroSkillMontage);
	}
	else if (CurrentPattern == FName("Hero4")) {
		Montage_Play(HeroSkillMontage, 1.0f);
		Montage_JumpToSection("3", HeroSkillMontage);
	}
	else if (CurrentPattern != FName("Hero"))
	{
		Montage_Play(SkillMontage, 1.0f);
		Montage_JumpToSection(CurrentPattern, SkillMontage);
	}
}

void UEnemyAnimInstance::PlayChangePatternMontage(FName NextPattern)
{
	if (NextPattern != FName("Hero")) {
		Montage_Play(ChangePatternMontage, 1.0f);
		Montage_JumpToSection(NextPattern, ChangePatternMontage);
	}
	else {
		Montage_Play(ChangePatternMontage, 1.0f);
		Montage_JumpToSection(FName("Hero"), ChangePatternMontage);
	}
}

void UEnemyAnimInstance::AnimNotify_EndChange()
{
	OnEndChange.Broadcast();
}

void UEnemyAnimInstance::AnimNotify_EndSkill()
{
	OnEndSkill.Broadcast();
}

void UEnemyAnimInstance::AnimNotify_EndSkill2()
{
	OnEndSkill2.Broadcast();
}

void UEnemyAnimInstance::AnimNotify_Skill1HitCheck()
{
	OnSkill1HitCheck.Broadcast();
}

void UEnemyAnimInstance::AnimNotify_Summon()
{
	OnSummon.Broadcast();
}

void UEnemyAnimInstance::AnimNotify_Summon2()
{
	OnSummon2.Broadcast();
}
void UEnemyAnimInstance::AnimNotify_Summon3()
{
	OnSummon3.Broadcast();
}
void UEnemyAnimInstance::AnimNotify_Summon4_1()
{
	OnSummon4_1.Broadcast();
}
void UEnemyAnimInstance::AnimNotify_Summon4_2()
{
	OnSummon4_2.Broadcast();
}
void UEnemyAnimInstance::AnimNotify_Summon5()
{
	OnSummon5.Broadcast();
}
void UEnemyAnimInstance::AnimNotify_Fire()
{
	OnFire.Broadcast();
}
void UEnemyAnimInstance::AnimNotify_FireBall()
{
	OnFireBall.Broadcast();
}
void UEnemyAnimInstance::AnimNotify_FireBall_Air()
{
	OnFireBall_Air.Broadcast();
}
void UEnemyAnimInstance::AnimNotify_FlyEnd()
{
	OnFlyEnd.Broadcast();
}
void UEnemyAnimInstance::AnimNotify_LandEnd()
{
	OnLandEnd.Broadcast();
}
void UEnemyAnimInstance::AnimNotify_FlyStart()
{
	OnFlyStart.Broadcast();
}
void UEnemyAnimInstance::AnimNotify_LandStart()
{
	OnLandStart.Broadcast();
}
