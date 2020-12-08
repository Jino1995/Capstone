// Fill out your copyright notice in the Description page of Project Settings.


#include "Servarog.h"
#include "EnemyAnimInstance.h"
#include "AIController.h"
#include "TimerManager.h"
#include "Kismet/KismetMathLibrary.h"
#include "PlayerCharacter.h"
#include "Sound/SoundCue.h"
#include "Components/WidgetComponent.h"
#include "NavigationSystem.h"
#include "fire.h"
#include "fire2_right.h"
#include "fire2_left.h"
#include "Fire2.h"
#include "Fire3.h"
#include "NormalEnemy.h"
#include "Enemy.h"

AServarog::AServarog()
{
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_Character(TEXT("/Game/Enemy/Boss/ParagonSevarog/Characters/Heroes/Sevarog/Meshes/Sevarog.Sevarog"));
	if (SK_Character.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(SK_Character.Object);
	}

	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	static ConstructorHelpers::FClassFinder<UAnimInstance> Enemy_Anim(TEXT("/Game/Enemy/Boss/ParagonSevarog/Sevarog_Anim_BP.Sevarog_Anim_BP_C"));
	if (Enemy_Anim.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(Enemy_Anim.Class);
	}
	
	CombatCollision2 = CreateDefaultSubobject<UBoxComponent>(TEXT("CombatCollision2"));
	//CombatCollision2->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName("root"));
	CombatCollision3 = CreateDefaultSubobject<USphereComponent>(TEXT("CombatCollision3"));

	//CombatCollision3->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName("Weapon_01"));
	BodyParts = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BODY"));
	BodyParts->SetupAttachment(GetMesh());
	BodyParts->SetMasterPoseComponent(GetMesh());
}
void AServarog::BeginPlay()
{
	Super::BeginPlay();

	AgroSphere->OnComponentBeginOverlap.AddDynamic(this, &AServarog::AgroSphereOnOverlapBegin);
	AgroSphere->OnComponentEndOverlap.AddDynamic(this, &AServarog::AgroSphereOnOverlapEnd);
	CombatSphere->OnComponentBeginOverlap.AddDynamic(this, &AServarog::CombatSphereOnOverlapBegin);
	CombatSphere->OnComponentEndOverlap.AddDynamic(this, &AServarog::CombatSphereOnOverlapEnd);
	CombatCollision->OnComponentBeginOverlap.AddDynamic(this, &AServarog::CombatCollisionOnOverlapBegin);
	CombatCollision->OnComponentEndOverlap.AddDynamic(this, &AServarog::CombatCollisionOnOverlapEnd);
	CombatCollision2->OnComponentBeginOverlap.AddDynamic(this, &AServarog::CombatCollisionOnOverlapBegin);
	CombatCollision2->OnComponentEndOverlap.AddDynamic(this, &AServarog::CombatCollisionOnOverlapEnd);
	CombatCollision3->OnComponentBeginOverlap.AddDynamic(this, &AServarog::CombatCollisionOnOverlapBegin);
	CombatCollision3->OnComponentEndOverlap.AddDynamic(this, &AServarog::CombatCollisionOnOverlapEnd);
	DeActivateCollision();
	DeActivateCollision2();
	DeActivateCollision3();

	CurrentPattern = EServarogPattern::PTN_Warrior;
	
	ABAnim->PlaySkillMontage(FName("Start"));
}

void AServarog::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (HuntTarget && EnemyMovementStatus != EEnemyMovementStatus::EMS_Attacking && (EnemyMovementStatus == EEnemyMovementStatus::EMS_Skill&&CurrentPattern == EServarogPattern::PTN_Mage))
	{
		FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), HuntTarget->GetActorLocation());
		FRotator LookAtRotationYaw(0.f, LookAtRotation.Yaw, 0.f);

		FRotator InterpRotation = FMath::RInterpTo(GetActorRotation(), LookAtRotationYaw, DeltaTime, 5.0f);

		SetActorRotation(InterpRotation);
	}
	if (Health <= HeroChangeHealth && !herochanged) {
		GetWorldTimerManager().SetTimer(ChangePatternTimer, this, &AServarog::ChangePattern, 2.0f);
		herochanged = true;

	}
}

void AServarog::PostInitializeComponents()
{
	CombatCollision2->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName("AttackSocket2"));
	CombatCollision3->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName("Weapon_01"));
	Super::PostInitializeComponents();
	
	ABAnim->BindMontagesForServarog(AttackMTG, MovingAttackMTG, SkillMTG, ChangePatternMTG, HeroSkillMTG);

	ABAnim->OnAttackEnd.AddLambda([this]()->void {
		AttackEnd();
	});

	ABAnim->OnActivateCollision.AddLambda([this]()->void {
		ActivateCollision();
	});

	ABAnim->OnDeActivateCollision.AddLambda([this]()->void {
		DeActivateCollision();
	});

	ABAnim->OnActivateCollision2.AddLambda([this]()->void {
		ActivateCollision2();
	});

	ABAnim->OnDeActivateCollision2.AddLambda([this]()->void {
		DeActivateCollision2();
	});
	ABAnim->OnActivateCollision3.AddLambda([this]()->void {
		ActivateCollision3();
	});

	ABAnim->OnDeActivateCollision3.AddLambda([this]()->void {
		DeActivateCollision3();
	});

	ABAnim->OnDeathEnd.AddLambda([this]()->void {
		Cast<AABGameMode>(UGameplayStatics::GetGameMode(GetWorld()))->CheckKilledMonsterStatus(EMonsterType::Boss);
		GetMesh()->bPauseAnims = true;
		GetMesh()->bNoSkeletonUpdate = true;

		GetWorldTimerManager().SetTimer(DeathTimer, this, &AEnemy::Disappear, DeathDelay);
	});

	ABAnim->OnEndSkill.AddLambda([this]()->void {
		SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Idle);
		MoveToTarget();
		bAttackCoolTime = false;
		bAttacking = false;

		float SkillCoolTime = FMath::FRandRange(MinSkillCoolTime, MaxSkillCoolTime);
		GetWorldTimerManager().SetTimer(SkillTimer, this, &AServarog::PlaySkill, SkillCoolTime, true);
	});
	
	ABAnim->OnEndSkill2.AddLambda([this]()->void {
		SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Idle);
		MoveToTarget();
		bAttackCoolTime = false;
		bAttacking = false;

		float SkillCoolTime = FMath::FRandRange(MinSkillCoolTime, MaxSkillCoolTime);
		GetWorldTimerManager().SetTimer(SkillTimer, this, &AServarog::PlaySkill, SkillCoolTime / 2, true);
	});
	
	ABAnim->OnSkill1HitCheck.AddLambda([this]()->void {
		
	});

	ABAnim->OnEndChange.AddLambda([this]()->void {
		SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Idle);
		MoveToTarget();
		bAttackCoolTime = false;
		bAttacking = false;

		float SkillCoolTime = FMath::FRandRange(MinSkillCoolTime, MaxSkillCoolTime);
		GetWorldTimerManager().SetTimer(SkillTimer, this, &AServarog::PlaySkill, SkillCoolTime, true);

		float ChangePatternTime = FMath::FRandRange(MinChangePatternTime, MaxChangePatternTime);
		GetWorldTimerManager().SetTimer(ChangePatternTimer, this, &AServarog::ChangePattern, ChangePatternTime, true);

		switch (CurrentPattern)
		{
		case EServarogPattern::PTN_Warrior:
			BodyParts->SetSkeletalMesh(WarriorPattern);
			break;
		case EServarogPattern::PTN_Summoner:
			BodyParts->SetSkeletalMesh(SummonerPattern);
			break;
		case EServarogPattern::PTN_Mage:
			BodyParts->SetSkeletalMesh(MagePatterns);
			break;
		case EServarogPattern::EMS_Hero:
			BodyParts->SetSkeletalMesh(HeroPattern);
			break;
		}
	});

	ABAnim->OnSummon.AddLambda([this]()->void {
		SummonMonster();
	});
	
	ABAnim->OnSummon2.AddLambda([this]()->void {
		SummonFire();
	});

	ABAnim->OnSummon3.AddLambda([this]()->void {
		SummonMonster_Hero();
	});

	ABAnim->OnSummon4_1.AddLambda([this]()->void {
		SummonAttack_right();
	});

	ABAnim->OnSummon4_2.AddLambda([this]()->void {
		SummonAttack_left();
	});

	ABAnim->OnSummon5.AddLambda([this]()->void {
		SummonFireStrom();
	});

	ABAnim->OnFire.AddLambda([this]()->void {
		SendFireStrom();
	});
	
}

void AServarog::ActivateCollision2()
{
	CombatCollision2->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AServarog::DeActivateCollision2()
{
	CombatCollision2->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}
void AServarog::ActivateCollision3()
{
	CombatCollision3->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AServarog::DeActivateCollision3()
{
	CombatCollision3->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AServarog::StartToPatrol()
{
	if (EnemyMovementStatus != EEnemyMovementStatus::EMS_Patrol && AIController)
	{
		GetCharacterMovement()->MaxWalkSpeed = PatrolSpeed;
		EnemyMovementStatus = EEnemyMovementStatus::EMS_Patrol;
		GetWorldTimerManager().SetTimer(PatrolTimer, this, &AServarog::MoveToPatrol, CheckPatrolTime, true);
	}
}

void AServarog::MoveToPatrol()
{
	FAIMoveRequest MoveRequest;
	MoveRequest.SetAcceptanceRadius(0.0f);
	FNavPathSharedPtr NavPath;

	if (PatrolResult)
	{
		if (PatrolResult.Code == EPathFollowingRequestResult::RequestSuccessful)
		{
			PatrolResult = AIController->MoveTo(MoveRequest, &NavPath);
		}
		else if (PatrolResult.Code == EPathFollowingRequestResult::AlreadyAtGoal)
		{
			CurrentPatrolNum = (CurrentPatrolNum + 1) % PatrolPoints.Num();
			MoveRequest.SetGoalLocation(PatrolPoints[CurrentPatrolNum]);
			PatrolResult = AIController->MoveTo(MoveRequest, &NavPath);
		}
	}
	else
	{
		MoveRequest.SetGoalLocation(PatrolPoints[CurrentPatrolNum]);
		PatrolResult = AIController->MoveTo(MoveRequest, &NavPath);
	}
}

void AServarog::CheckToTargetInHuntRange()
{
	if (AgroTarget)
	{
		FRotator BetweenAngle = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), AgroTarget->GetActorLocation());
		FRotator EnemyLocation = UKismetMathLibrary::NormalizedDeltaRotator(GetActorRotation(), BetweenAngle);
		bool bHuntZone = (UKismetMathLibrary::InRange_FloatFloat(EnemyLocation.Yaw, -180.0f, -90.0f) || UKismetMathLibrary::InRange_FloatFloat(EnemyLocation.Yaw, 90.0f, 180.0f));

		if (!bHuntZone)
		{
			GetWorldTimerManager().ClearTimer(HuntTimer);
			GetWorldTimerManager().ClearTimer(PatrolTimer);
			GetCharacterMovement()->MaxWalkSpeed = HuntSpeed;
			
			float SkillCoolTime = FMath::FRandRange(MinSkillCoolTime, MaxSkillCoolTime);
			GetWorldTimerManager().SetTimer(SkillTimer, this, &AServarog::PlaySkill, SkillCoolTime);

			float ChangePatternTime = FMath::FRandRange(MinChangePatternTime, MaxChangePatternTime);
			GetWorldTimerManager().SetTimer(ChangePatternTimer, this, &AServarog::ChangePattern, ChangePatternTime);

			MoveToTarget();
		}
	}
}

void AServarog::MoveToTarget()
{
	if (EnemyMovementStatus == EEnemyMovementStatus::EMS_Patrol)
	{
		GetWorldTimerManager().ClearTimer(PatrolTimer);
	}
	if (EnemyMovementStatus != EEnemyMovementStatus::EMS_Attacking && EnemyMovementStatus != EEnemyMovementStatus::EMS_ChangePattern && EnemyMovementStatus != EEnemyMovementStatus::EMS_Skill)
	{
		HuntTarget = AgroTarget;
		EnemyMovementStatus = EEnemyMovementStatus::EMS_MoveToTarget;
		FAIMoveRequest MoveRequest;
		MoveRequest.SetAcceptanceRadius(0.0f);
		MoveRequest.SetGoalActor(HuntTarget);
		FNavPathSharedPtr NavPath;
		PatrolResult = AIController->MoveTo(MoveRequest, &NavPath);
	}
}

void AServarog::AgroSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && Alive())
	{
		APlayerCharacter* Main = Cast<APlayerCharacter>(OtherActor);
		if (Main)
		{
			AgroTarget = Main;
			GetWorldTimerManager().SetTimer(HuntTimer, this, &AServarog::CheckToTargetInHuntRange, CheckHuntZoneTime, true);
		}
	}
}

void AServarog::AgroSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		APlayerCharacter* Main = Cast<APlayerCharacter>(OtherActor);
		if (Main)
		{
			SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Idle);
			if (AIController)
				AIController->StopMovement();
		}
	}
}

void AServarog::CombatSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		APlayerCharacter* Main = Cast<APlayerCharacter>(OtherActor);
		if (Main)
		{
			bOverlappingCombatSphere = true;
			if (EnemyMovementStatus != EEnemyMovementStatus::EMS_Patrol)
			{
				GetWorldTimerManager().SetTimer(AttackTimer, this, &AServarog::SelectAttack, 0.1, true);
			}
		}
	}
}

void AServarog::CombatSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		APlayerCharacter* Main = Cast<APlayerCharacter>(OtherActor);
		if (Main)
		{
			bOverlappingCombatSphere = false;
			if (!bAttacking && EnemyMovementStatus != EEnemyMovementStatus::EMS_Patrol)
			{
				MoveToTarget();
			}
			GetWorldTimerManager().ClearTimer(AttackTimer);
		}
	}
}

void AServarog::CombatCollisionOnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor);
		if (Player)
		{
			if (DamageTypeClass)
			{
				UGameplayStatics::ApplyDamage(Player, EnemyDamage, AIController, this, DamageTypeClass);
				DeActivateCollision();
			}
		}
	}
}

void AServarog::CombatCollisionOnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}

void AServarog::SelectAttack()
{
	if (EnemyMovementStatus != EEnemyMovementStatus::EMS_Skill && EnemyMovementStatus != EEnemyMovementStatus::EMS_ChangePattern)
	{
		if (MovingAttackMTG)
		{
			if (rand() % 100 > MovingAttackActivationProbability)
			{
				Attack();
			}
			else
			{
				MovingAttack();
			}
		}
		else
		{
			Attack();
		}
	}
}

void AServarog::Attack()
{
	if (Alive() && !bAttackCoolTime && !bAttacking)
	{
		bAttacking = true;
		if (AIController)
		{
			AIController->StopMovement();
		}
		SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Attacking);

		if (ABAnim)
		{
			ABAnim->PlayAttackMontage();
			ABAnim->JumpToAttackMontageSection(AttackNum);
		}
	}
}

void AServarog::MovingAttack()
{
	if (Alive() && !bAttackCoolTime && !bAttacking)
	{
		bAttacking = true;
		SetEnemyMovementStatus(EEnemyMovementStatus::EMS_MovingAttacking);

		GetWorldTimerManager().SetTimer(MAttackTimer, this, &AServarog::MoveToTargetForMovingAttack, 0.1, true);
		if (ABAnim)
		{
			if (CurrentPattern != EServarogPattern::EMS_Hero) {
				ABAnim->PlayMovingAttackMontage();//MovingAttackNum
				ABAnim->JumpToMovingAttackMontageSection(1);
			}
			else {
				ABAnim->PlayMovingAttackMontage();
				ABAnim->JumpToMovingAttackMontageSection(2);
			}
		}
	}
}

void AServarog::AttackEnd()
{
	if (GetWorldTimerManager().IsTimerActive(MAttackTimer))
	{
		GetWorldTimerManager().ClearTimer(MAttackTimer);
	}

	bAttacking = false;
	bAttackCoolTime = true;

	float AttackCoolTime = FMath::FRandRange(MinAttackCoolTime, MaxAttackCoolTime);
	GetWorldTimerManager().SetTimer(AttackCoolTimeTimer, this, &AServarog::TurnOnAttackCoolTime, AttackCoolTime);
	SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Idle);
	MoveToTarget();
}

float AServarog::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	if ((Health -= DamageAmount) <= 0.0f)
	{
		if (AgroTarget->GetLockOnTarget() == this)
		{
			AgroTarget->RemoveEnemyToTarget(this);
			AgroTarget->TargetingLockOn();
		}
		Die();
	}
	OnHPChanged.Broadcast();
	HPBarWidget->SetVisibility(true);
	ShowDamageText(DamageAmount);
	if (!HuntTarget)
	{
		HuntTarget = AgroTarget;
		MoveToTarget();
	}
	return DamageAmount;
}

void AServarog::TurnOnAttackCoolTime()
{
	bAttackCoolTime = false;
	GetWorldTimerManager().ClearTimer(AttackCoolTimeTimer);
}

void AServarog::Die()
{
	IsDead = true;
	AIController->StopMovement();
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CombatCollision2->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CombatCollision3->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AgroSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CombatSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Dead);
}

void AServarog::MoveToTargetForMovingAttack()
{
	AIController->MoveToActor(HuntTarget);
}

void AServarog::ChangePattern()
{
	if (EnemyMovementStatus != EEnemyMovementStatus::EMS_MovingAttacking)
	{
		AIController->StopMovement();
		EnemyMovementStatus = EEnemyMovementStatus::EMS_ChangePattern;
	
		GetWorldTimerManager().ClearTimer(ChangePatternTimer);
		GetWorldTimerManager().ClearTimer(SkillTimer);
		if (Health > HeroChangeHealth) {
			switch (rand() % PatternNum)
			{
			case 0:
				if (CurrentPattern == EServarogPattern::PTN_Warrior) ChangePattern();
				else
				{
					CurrentPattern = EServarogPattern::PTN_Warrior;
					ABAnim->PlayChangePatternMontage(FName("Warrior"));
				}
				break;
			case 1:
				if (CurrentPattern == EServarogPattern::PTN_Summoner) ChangePattern();
				else
				{
					CurrentPattern = EServarogPattern::PTN_Summoner;
					ABAnim->PlayChangePatternMontage(FName("Summoner"));
				}
				break;
			case 2:
				if (CurrentPattern == EServarogPattern::PTN_Mage) ChangePattern();
				else
				{
					CurrentPattern = EServarogPattern::PTN_Mage;
					ABAnim->PlayChangePatternMontage(FName("Mage"));
				}
				break;
			}
		}
		else {
			if (CurrentPattern != EServarogPattern::EMS_Hero) {
				CurrentPattern = EServarogPattern::EMS_Hero;

				ABAnim->PlayChangePatternMontage(FName("Hero"));
			}
			else {
				float SkillCoolTime = FMath::FRandRange(MinSkillCoolTime, MaxSkillCoolTime);
				GetWorldTimerManager().SetTimer(SkillTimer, this, &AServarog::PlaySkill, SkillCoolTime);
				SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Idle);
				MoveToTarget();
			}
		}


	}
	else if (ABAnim->IsAnyMontagePlaying()) {
		GetWorldTimerManager().SetTimer(ChangePatternTimer, this, &AServarog::ChangePattern, 2.0f);
	}
	else {
		float ChangePatternTime = FMath::FRandRange(MinChangePatternTime, MaxChangePatternTime);
		GetWorldTimerManager().SetTimer(ChangePatternTimer, this, &AServarog::ChangePattern, 4.0f);
	}
}

void AServarog::PlaySkill()
{
	if (EnemyMovementStatus != EEnemyMovementStatus::EMS_MovingAttacking)
	{
		EnemyMovementStatus = EEnemyMovementStatus::EMS_Skill;
	
		GetWorldTimerManager().ClearTimer(SkillTimer);
		switch (CurrentPattern)
		{
		case EServarogPattern::PTN_Warrior:
			ABAnim->PlaySkillMontage(FName("Warrior"));
			if (ABAnim) {
				FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), AgroTarget->GetActorLocation());
				FRotator LookAtRotationYaw(0.f, LookAtRotation.Yaw - 35, 0.f);
				SetActorRotation(LookAtRotationYaw);
			}
			break;
		case EServarogPattern::PTN_Mage:
			ABAnim->PlaySkillMontage(FName("Mage"));
			break;
		case EServarogPattern::PTN_Summoner:
			ABAnim->PlaySkillMontage(FName("Summoner"));
			break;
		case EServarogPattern::EMS_Hero:
			int r = (rand() % 5) + 1;
			if (firenum > 2) {
				ABAnim->PlaySkillMontage(FName("Hero4"));
			}
			else if (r == 1) {
				ABAnim->PlaySkillMontage(FName("Hero1"));
				if (ABAnim) {
					FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), AgroTarget->GetActorLocation());
					FRotator LookAtRotationYaw(0.f, LookAtRotation.Yaw - 35, 0.f);
					SetActorRotation(LookAtRotationYaw);
				}
			}
			else if (r > 1 && r < 5) {
				ABAnim->PlaySkillMontage(FName("Hero3"));
			}
			else
				ABAnim->PlaySkillMontage(FName("Hero2"));
			break;
		}
		AIController->StopMovement();
	}
	else if (ABAnim->IsAnyMontagePlaying()) {
		GetWorldTimerManager().SetTimer(SkillTimer, this, &AServarog::PlaySkill, 3.0f);
	}
}

void AServarog::SummonMonster()
{
	EnemyMovementStatus = EEnemyMovementStatus::EMS_Skill;
	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetNavigationSystem(GetWorld());
	if (NavSystem != nullptr)
	{
		FVector SpawnVector = FVector(GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z);
		auto Skeleton = GetWorld()->SpawnActor<ANormalEnemy>(SpawnMonsterList, SpawnVector, FRotator::ZeroRotator);
		if (Skeleton)
		{
			Skeleton->ParticleOn();
			Skeleton->AgroSphere->InitSphereRadius(1300.0f);
			Skeleton->SpawnDefaultController();
			Skeleton->AgroTarget = AgroTarget;
			Skeleton->ANormalEnemy::MoveToTarget();
		}
		else
		{
			SummonMonster();
		}
	}
}

void AServarog::SummonMonster_Hero()
{
	EnemyMovementStatus = EEnemyMovementStatus::EMS_Skill;
	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetNavigationSystem(GetWorld());
	if (NavSystem != nullptr)
	{
		FVector SpawnVector = FVector(GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z);
		auto Skeleton = GetWorld()->SpawnActor<ANormalEnemy>(SpawnMonsterList2, SpawnVector, FRotator::ZeroRotator);
		if (Skeleton)
		{
			Skeleton->ParticleOn();
			Skeleton->AgroSphere->InitSphereRadius(1300.0f);
			Skeleton->SpawnDefaultController();
			Skeleton->AgroTarget = AgroTarget;
			Skeleton->ANormalEnemy::MoveToTarget();
		}
		else
		{
			SummonMonster_Hero();
		}

	}
}
void AServarog::SummonAttack_right() {
	if (ProjectileClass2)
	{
		FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), AgroTarget->GetActorLocation());
		FRotator LookAtRotationYaw(0.f, 0.f, LookAtRotation.Yaw);


		FVector ALocation = GetActorLocation();
		FVector MuzzleLocation(ALocation.X, ALocation.Y, ALocation.Z);
		FRotator MuzzleRotation = GetActorRotation();;

		UWorld* World = GetWorld();
		if (World)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;

			Afire2_right* Projectile2 = World->SpawnActor<Afire2_right>(ProjectileClass2, MuzzleLocation, MuzzleRotation, SpawnParams);
			if (Projectile2)
			{
				FVector LaunchDirection = MuzzleRotation.Vector();
				Projectile2->FireInDirection(LaunchDirection);
				Projectile2->Fire_GetAIController(AIController);

			}
		}
	}
}
	
void AServarog::SummonAttack_left() {

	if (ProjectileClass3)
	{
		UE_LOG(LogTemp, Warning, TEXT("fire"));
		FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), AgroTarget->GetActorLocation());
		FRotator LookAtRotationYaw(0.f, 0.f, LookAtRotation.Yaw);


		FVector ALocation = GetActorLocation();
		FVector MuzzleLocation(ALocation.X, ALocation.Y, ALocation.Z);
		FRotator MuzzleRotation = GetActorRotation();;

		UWorld* World = GetWorld();
		if (World)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			Afire2_left* Projectile3 = World->SpawnActor<Afire2_left>(ProjectileClass3, MuzzleLocation, MuzzleRotation, SpawnParams);
			if (Projectile3)
			{
				FVector LaunchDirection = MuzzleRotation.Vector();
				Projectile3->FireInDirection(LaunchDirection);
				Projectile3->Fire_GetAIController(AIController);
			}
		}
	}
}
void AServarog::SummonFire() {
	EnemyMovementStatus = EEnemyMovementStatus::EMS_Skill;

	if (ProjectileClass)
	{
		FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), AgroTarget->GetActorLocation());
		FRotator LookAtRotationYaw(0.f, 0.f, LookAtRotation.Yaw);


		FVector ALocation = GetActorLocation();
		FVector MuzzleLocation(ALocation.X, ALocation.Y, ALocation.Z);
		FRotator MuzzleRotation = GetActorRotation();;

		UWorld* World = GetWorld();
		if (World)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			Afire* Projectile = World->SpawnActor<Afire>(ProjectileClass, MuzzleLocation, MuzzleRotation, SpawnParams);
			if (Projectile)
			{
				FVector LaunchDirection = MuzzleRotation.Vector();
				Projectile->FireInDirection(LaunchDirection);
				Projectile->Fire_GetAIController(AIController);
			}
		}
	}
}
void AServarog::SummonFireStrom() {
	EnemyMovementStatus = EEnemyMovementStatus::EMS_Skill;

	if (ProjectileClass4)
	{
		FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), AgroTarget->GetActorLocation());
		FRotator LookAtRotationYaw(0.f, 0.f, LookAtRotation.Yaw);


		FVector ALocation = GetActorLocation();
		FVector MuzzleLocation(ALocation.X, ALocation.Y, ALocation.Z);
		FRotator MuzzleRotation = GetActorRotation();

		UWorld* World = GetWorld();
		if (World)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			Projectile4[firenum] = World->SpawnActor<Afire3>(ProjectileClass4, MuzzleLocation, MuzzleRotation, SpawnParams);
			if (Projectile4)
			{
				Projectile4[firenum]->SetTarget(AgroTarget);
				Projectile4[firenum]->Fire_GetAIController(AIController);
			}
			firenum++;
		}
	}
}

void AServarog::SendFireStrom() {
	EnemyMovementStatus = EEnemyMovementStatus::EMS_Skill;
	for (int i = 0; i < firenum; i++) {
		Projectile4[i]->FireInDirection2();
	}
	firenum = 0;
}