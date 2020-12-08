// Fill out your copyright notice in the Description page of Project Settings.


#include "NormalEnemy.h"
#include "EnemyAnimInstance.h"
#include "NavigationSystem.h"
#include "AIController.h"
#include "TimerManager.h"
#include "Kismet/KismetMathLibrary.h"
#include "PlayerCharacter.h"
#include "Sound/SoundCue.h"
#include "Components/WidgetComponent.h"
#include "ABGameMode.h"
#include "Skill.h"
#include "Enemy.h"

ANormalEnemy::ANormalEnemy()
{
	//AIController = Cast<AAIController>(GetController());
	AIControllerClass = AAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	ParticleSystem1 = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("MovementParticles"));
	ParticleSystem1->SetupAttachment(GetMesh(), FName("root3"));
	ParticleSystem1->bAutoActivate = false;
	ParticleSystem1->SetRelativeLocation(FVector(-20.0f, 0.0f, 20.0f));
	static ConstructorHelpers::FObjectFinder<UParticleSystem> ParticleAsset(TEXT("/Game/Enemy/Boss/ParagonSevarog/FX/Particles/Abilities/Ultimate/FX/P_UltActivate_2.P_UltActivate_2"));


	if (ParticleAsset.Succeeded())
	{
		ParticleSystem1->SetTemplate(ParticleAsset.Object);

	}

}

void ANormalEnemy::BeginPlay()
{
	Super::BeginPlay();

	AgroSphere->OnComponentBeginOverlap.AddDynamic(this, &ANormalEnemy::AgroSphereOnOverlapBegin);
	AgroSphere->OnComponentEndOverlap.AddDynamic(this, &ANormalEnemy::AgroSphereOnOverlapEnd);
	CombatSphere->OnComponentBeginOverlap.AddDynamic(this, &ANormalEnemy::CombatSphereOnOverlapBegin);
	CombatSphere->OnComponentEndOverlap.AddDynamic(this, &ANormalEnemy::CombatSphereOnOverlapEnd);
	CombatCollision->OnComponentBeginOverlap.AddDynamic(this, &ANormalEnemy::CombatCollisionOnOverlapBegin);
	CombatCollision->OnComponentEndOverlap.AddDynamic(this, &ANormalEnemy::CombatCollisionOnOverlapEnd);
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AIController = Cast<AAIController>(GetController());
	StartToPatrol();
}

void ANormalEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (HuntTarget && EnemyMovementStatus != EEnemyMovementStatus::EMS_Attacking && EnemyMovementStatus != EEnemyMovementStatus::EMS_Dead)
	{
		FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), HuntTarget->GetActorLocation());
		FRotator LookAtRotationYaw(0.f, LookAtRotation.Yaw, 0.f);

		FRotator InterpRotation = FMath::RInterpTo(GetActorRotation(), LookAtRotationYaw, DeltaTime, 5.0f);

		SetActorRotation(InterpRotation);
	}
}

void ANormalEnemy::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	ABAnim->BindMontages(AttackMTG, MovingAttackMTG);
	ABAnim->OnAttackEnd.AddLambda([this]()->void {
		AttackEnd();
	});

	ABAnim->OnActivateCollision.AddLambda([this]()->void {
		ActivateCollision();
	});

	ABAnim->OnDeActivateCollision.AddLambda([this]()->void {
		DeActivateCollision();
	});

	ABAnim->OnDeathEnd.AddLambda([this]()->void {
		Cast<AABGameMode>(UGameplayStatics::GetGameMode(GetWorld()))->CheckKilledMonsterStatus(EMonsterType::Normal);
		GetMesh()->bPauseAnims = true;
		GetMesh()->bNoSkeletonUpdate = true;

		GetWorldTimerManager().SetTimer(DeathTimer, this, &AEnemy::Disappear, DeathDelay);
	});
	
}

void ANormalEnemy::StartToPatrol()
{
	if (EnemyMovementStatus != EEnemyMovementStatus::EMS_Patrol && AIController)
	{
		GetCharacterMovement()->MaxWalkSpeed = PatrolSpeed;
		EnemyMovementStatus = EEnemyMovementStatus::EMS_Patrol;
		GetWorldTimerManager().SetTimer(PatrolTimer, this, &ANormalEnemy::MoveToPatrol, CheckPatrolTime, true);
	}
}

void ANormalEnemy::MoveToPatrol()
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
			CurrentPatrolNum = (CurrentPatrolNum+1) % PatrolPoints.Num();
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

void ANormalEnemy::CheckToTargetInHuntRange()
{
	if (AgroTarget)
	{
		FRotator BetweenAngle = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), AgroTarget->GetActorLocation());
		FRotator EnemyLocation = UKismetMathLibrary::NormalizedDeltaRotator(GetActorRotation(), BetweenAngle);
		bool bHuntZone = (UKismetMathLibrary::InRange_FloatFloat(EnemyLocation.Yaw, -180.0f, -90.0f) || UKismetMathLibrary::InRange_FloatFloat(EnemyLocation.Yaw, 90.0f, 180.0f));

		if (!bHuntZone)
		{
			GetWorldTimerManager().ClearTimer(PatrolTimer);
			GetWorldTimerManager().ClearTimer(HuntTimer);
			GetCharacterMovement()->MaxWalkSpeed = HuntSpeed;
			MoveToTarget();
		}
	}
}

void ANormalEnemy::MoveToTarget()
{
	if (EnemyMovementStatus != EEnemyMovementStatus::EMS_Dead)
	{
		if (EnemyMovementStatus == EEnemyMovementStatus::EMS_Patrol)
		{
			GetWorldTimerManager().ClearTimer(PatrolTimer);
		}
		if (EnemyMovementStatus != EEnemyMovementStatus::EMS_Attacking)
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
}

void ANormalEnemy::AgroSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && Alive())
	{
		APlayerCharacter* Main = Cast<APlayerCharacter>(OtherActor);
		if (Main && !AgroTarget)
		{
			AgroTarget = Main;
			GetWorldTimerManager().SetTimer(HuntTimer, this, &ANormalEnemy::CheckToTargetInHuntRange, CheckHuntZoneTime, true);
		}
	}
}

void ANormalEnemy::AgroSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		APlayerCharacter* Main = Cast<APlayerCharacter>(OtherActor);
		/*
		if (Main)
		{
			SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Idle);
			if (AIController)
				AIController->StopMovement();
			AgroTarget = nullptr;
		}
		*/
	}
}

void ANormalEnemy::CombatSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		APlayerCharacter* Main = Cast<APlayerCharacter>(OtherActor);
		if (Main)
		{
			if (!AgroTarget)
			{
				AgroTarget = Main;
				GetWorldTimerManager().SetTimer(HuntTimer, this, &ANormalEnemy::CheckToTargetInHuntRange, CheckHuntZoneTime, true);
			}
			bOverlappingCombatSphere = true;
		
			if (EnemyMovementStatus != EEnemyMovementStatus::EMS_Patrol)
			{
				GetWorldTimerManager().SetTimer(AttackTimer, this, &ANormalEnemy::SelectAttack, 0.1, true);
			}
		}
	}
}

void ANormalEnemy::CombatSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		APlayerCharacter* Main = Cast<APlayerCharacter>(OtherActor);
		if (Main)
		{
			bOverlappingCombatSphere = false;
	
			if(!bAttacking && EnemyMovementStatus != EEnemyMovementStatus::EMS_Patrol)
			{
				MoveToTarget();
			}
			GetWorldTimerManager().ClearTimer(AttackTimer);
		}
	}
}

void ANormalEnemy::CombatCollisionOnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
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

void ANormalEnemy::CombatCollisionOnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}

void ANormalEnemy::SelectAttack()
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

void ANormalEnemy::Attack()
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

void ANormalEnemy::MovingAttack()
{
	if (Alive() && !bAttackCoolTime && !bAttacking)
	{
		bAttacking = true;
		SetEnemyMovementStatus(EEnemyMovementStatus::EMS_MovingAttacking);
		
		GetWorldTimerManager().SetTimer(MAttackTimer, this, &ANormalEnemy::MoveToTargetForMovingAttack, 0.1, true);
		if (ABAnim)
		{
			ABAnim->PlayMovingAttackMontage();
			ABAnim->JumpToMovingAttackMontageSection(MovingAttackNum);
		}
	}
}

void ANormalEnemy::AttackEnd()
{
	if (GetWorldTimerManager().IsTimerActive(MAttackTimer))
	{
		GetWorldTimerManager().ClearTimer(MAttackTimer);
	}

	bAttacking = false;
	bAttackCoolTime = true;
	
	float AttackCoolTime = FMath::FRandRange(MinAttackCoolTime, MaxAttackCoolTime);
	GetWorldTimerManager().SetTimer(AttackCoolTimeTimer, this, &ANormalEnemy::TurnOnAttackCoolTime, AttackCoolTime);

	SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Idle);
	MoveToTarget();
}

float ANormalEnemy::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	if(!AgroTarget)
	{
		AgroTarget = Cast<APlayerCharacter>(DamageCauser);

	}
	if ((Health -= DamageAmount) <= 0.0f)
	{
		if (AgroTarget)
		{
			if (AgroTarget->GetLockOnTarget() == this)
			{
				AgroTarget->RemoveEnemyToTarget(this);
				AgroTarget->TargetingLockOn();
			}
		}
		Die();
	}
	else
	{
		if (!HuntTarget)
		{
			GetCharacterMovement()->MaxWalkSpeed = HuntSpeed;
			MoveToTarget();
		}
	}
	OnHPChanged.Broadcast();
	HPBarWidget->SetVisibility(true);
	ShowDamageText(DamageAmount);
	return DamageAmount;	
}

void ANormalEnemy::TurnOnAttackCoolTime()
{
	bAttackCoolTime = false;
	GetWorldTimerManager().ClearTimer(AttackCoolTimeTimer);
}

void ANormalEnemy::Die()
{
	IsDead = true;
	AIController->StopMovement();
	AgroTarget->GainMoney(Money);
	AgroSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CombatSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Dead);
}

void ANormalEnemy::MoveToTargetForMovingAttack()
{
	AIController->MoveToActor(HuntTarget);
}

void ANormalEnemy::ParticleOn() {
	ParticleSystem1->Activate(true);
	GetWorldTimerManager().SetTimer(ParticleOffTimer, this, &ANormalEnemy::ParticleOff, 6.0f);
}

void ANormalEnemy::ParticleOff() {
	ParticleSystem1->Deactivate();
}