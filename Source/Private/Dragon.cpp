// Fill out your copyright notice in the Description page of Project Settings.

#include "Dragon.h"
#include "EnemyAnimInstance.h"
#include "AIController.h"
#include "TimerManager.h"
#include "Kismet/KismetMathLibrary.h"
#include "PlayerCharacter.h"
#include "Sound/SoundCue.h"
#include "Components/WidgetComponent.h"
#include "NavigationSystem.h"
#include "Engine/Classes/GameFramework/ProjectileMovementComponent.h"
#include "FireBall.h"

ADragon::ADragon()
{
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_Character(TEXT("/Game/Enemy/Boss/MountainDragon/Meshes/SK_MOUNTAIN_DRAGON.SK_MOUNTAIN_DRAGON"));
	if (SK_Character.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(SK_Character.Object);
	}

	/*
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	static ConstructorHelpers::FClassFinder<UAnimInstance> Enemy_Anim(TEXT("/Game/Enemy/Boss/MountainDragon/Dragon_Anim_BP.Dragon_Anim_BP_C"));
	if (Enemy_Anim.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(Enemy_Anim.Class);
	}
	*/
	CombatCollision2 = CreateDefaultSubobject<UBoxComponent>(TEXT("CombatCollision2"));
	//CombatCollision2->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName("Head"));
	CombatCollision3 = CreateDefaultSubobject<USphereComponent>(TEXT("CombatCollision3"));
	//CombatCollision3->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName("LHand"));
	CombatCollision4 = CreateDefaultSubobject<USphereComponent>(TEXT("CombatCollision4"));
	//CombatCollision4->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName("RHand"));
	CombatCollision5 = CreateDefaultSubobject<UBoxComponent>(TEXT("CombatCollision5"));
	//CombatCollision5->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName("Head"));
	AgroSphere2 = CreateDefaultSubobject<USphereComponent>(TEXT("AgroSphere2"));
	AgroSphere2->SetupAttachment(RootComponent);
	AgroSphere2->InitSphereRadius(900.0f);
	AgroSphere3 = CreateDefaultSubobject<USphereComponent>(TEXT("AgroSphere3"));
	AgroSphere3->SetupAttachment(RootComponent);
	AgroSphere3->InitSphereRadius(1600.0f);

}
void ADragon::BeginPlay()
{
	Super::BeginPlay();
	GetCharacterMovement()->MaxWalkSpeed = 300;
	AgroSphere2->OnComponentBeginOverlap.AddDynamic(this, &ADragon::AgroSphereOnOverlapBegin_2);
	AgroSphere2->OnComponentEndOverlap.AddDynamic(this, &ADragon::AgroSphereOnOverlapEnd_2);
	AgroSphere3->OnComponentBeginOverlap.AddDynamic(this, &ADragon::AgroSphereOnOverlapBegin_3);
	AgroSphere3->OnComponentEndOverlap.AddDynamic(this, &ADragon::AgroSphereOnOverlapEnd_3);

	CombatSphere->OnComponentBeginOverlap.AddDynamic(this, &ADragon::CombatSphereOnOverlapBegin);
	CombatSphere->OnComponentEndOverlap.AddDynamic(this, &ADragon::CombatSphereOnOverlapEnd);

	CombatCollision->OnComponentBeginOverlap.AddDynamic(this, &ADragon::CombatCollisionOnOverlapBegin);
	CombatCollision->OnComponentEndOverlap.AddDynamic(this, &ADragon::CombatCollisionOnOverlapEnd);

	CombatCollision2->OnComponentBeginOverlap.AddDynamic(this, &ADragon::CombatCollisionOnOverlapBegin2);
	CombatCollision2->OnComponentEndOverlap.AddDynamic(this, &ADragon::CombatCollisionOnOverlapEnd2);

	CombatCollision3->OnComponentBeginOverlap.AddDynamic(this, &ADragon::CombatCollisionOnOverlapBegin2);
	CombatCollision3->OnComponentEndOverlap.AddDynamic(this, &ADragon::CombatCollisionOnOverlapEnd2);

	CombatCollision4->OnComponentBeginOverlap.AddDynamic(this, &ADragon::CombatCollisionOnOverlapBegin2);
	CombatCollision4->OnComponentEndOverlap.AddDynamic(this, &ADragon::CombatCollisionOnOverlapEnd2);
	CombatCollision5->OnComponentBeginOverlap.AddDynamic(this, &ADragon::CombatCollisionOnOverlapBegin3);
	CombatCollision5->OnComponentEndOverlap.AddDynamic(this, &ADragon::CombatCollisionOnOverlapEnd3);
	DeActivateCollision();
	DeActivateCollision2();
	DeActivateCollision3();
	DeActivateCollision4();
	DeActivateCollision5();
	IsInGround = true;
	IsInAir = false;
}

void ADragon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	DeltaTime2 = DeltaTime;
	//공격중, 행동중이지 않을 때 적 방향으로 회전
	if (AgroTarget && !bAttacking && !IsTurnning && !IsDead)
	{
		FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), AgroTarget->GetActorLocation());
		FRotator LookAtRotationYaw(0.f, LookAtRotation.Yaw, 0.f);

		FRotator InterpRotation = FMath::RInterpTo(GetActorRotation(), LookAtRotationYaw, DeltaTime, 5.0f);
		SetActorRotation(InterpRotation);
	}

}

void ADragon::PostInitializeComponents()
{
	CombatCollision2->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName("Head"));
	CombatCollision3->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName("LHand"));
	CombatCollision4->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName("RHand"));
	CombatCollision5->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName("Head2"));

	Super::PostInitializeComponents();


	if (ABAnim) {
		ABAnim->BindMontagesForDragon(AttackMTG);
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
		ABAnim->OnActivateCollision4.AddLambda([this]()->void {
			ActivateCollision4();
		});

		ABAnim->OnDeActivateCollision4.AddLambda([this]()->void {
			DeActivateCollision4();
		});
		ABAnim->OnActivateCollision5.AddLambda([this]()->void {
			ActivateCollision5();
		});

		ABAnim->OnDeActivateCollision5.AddLambda([this]()->void {
			DeActivateCollision5();
		});
		ABAnim->OnFireBall.AddLambda([this]()->void {
			FireFireBall();
		});
		ABAnim->OnDeathEnd.AddLambda([this]()->void {
			Cast<AABGameMode>(UGameplayStatics::GetGameMode(GetWorld()))->CheckKilledMonsterStatus(EMonsterType::Boss);

			GetMesh()->bPauseAnims = true;
			GetMesh()->bNoSkeletonUpdate = true;

			GetWorldTimerManager().SetTimer(DeathTimer, this, &AEnemy::Disappear, DeathDelay);
		});
		ABAnim->OnDeathEnd.AddLambda([this]()->void {
			GetMesh()->bPauseAnims = true;
			GetMesh()->bNoSkeletonUpdate = true;

			GetWorldTimerManager().SetTimer(DeathTimer, this, &AEnemy::Disappear, DeathDelay);
		});
		ABAnim->OnFlyEnd.AddLambda([this]()->void {
			FlyToAir_End();
		});
		ABAnim->OnLandEnd.AddLambda([this]()->void {
			LandToGround_End();
		});
		ABAnim->OnFlyStart.AddLambda([this]()->void {
			GetWorldTimerManager().SetTimer(FlyUpTimer, this, &ADragon::HeightIncrease, 0.1f, true);
		});
		ABAnim->OnLandStart.AddLambda([this]()->void {
			GetWorldTimerManager().SetTimer(FlyUpTimer, this, &ADragon::HeightDecrease, 0.1f, true);
		});
		ABAnim->OnFireBall_Air.AddLambda([this]()->void {
			FireFireBall_Air();
		});
	}
}
void ADragon::ActivateCollision2()
{
	CombatCollision2->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void ADragon::DeActivateCollision2()
{
	CombatCollision2->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}
void ADragon::ActivateCollision3()
{
	CombatCollision3->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void ADragon::DeActivateCollision3()
{
	CombatCollision3->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}
void ADragon::ActivateCollision4()
{
	CombatCollision4->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void ADragon::DeActivateCollision4()
{
	CombatCollision4->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}
void ADragon::ActivateCollision5()
{
	CombatCollision5->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void ADragon::DeActivateCollision5()
{
	CombatCollision5->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ADragon::Pattern_Selector()
{
	if (AgroTarget)
	{
		float SkillCoolTime = FMath::FRandRange(1, 3);
		GetWorldTimerManager().SetTimer(SkillTimer, this, &ADragon::Pattern_Selector, SkillCoolTime, true);
		if (!IsPatternPlaying) {
			FRotator BetweenAngle = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), AgroTarget->GetActorLocation());
			FRotator EnemyLocation = UKismetMathLibrary::NormalizedDeltaRotator(GetActorRotation(), BetweenAngle);
			if (UKismetMathLibrary::InRange_FloatFloat(EnemyLocation.Yaw, -180.0f, -20.0f)) {
				GetWorldTimerManager().SetTimer(TurnningWaitTimer, this, &ADragon::TurnEnd, 0.1f);
				IsPatternPlaying = true;
			}
			else if (UKismetMathLibrary::InRange_FloatFloat(EnemyLocation.Yaw, 20.0f, 180.0f)) {
				GetWorldTimerManager().SetTimer(TurnningWaitTimer, this, &ADragon::TurnEnd, 0.1f);
				IsPatternPlaying = true;
			}
		}
		if (!IsPatternPlaying&&IsInGround) {

			int percent = rand() % 100;
			//다가가서 근접공격

			if (InCombatSphere && InAgroSphere2 && InAgroSphere3) { //근거리
				if (percent < 100) {						//100%확률로
					//근접공격
					MeleeAttackOn = true;
					SelectAttack();
					GetWorldTimerManager().SetTimer(MoveTimer, this, &ADragon::MoveTimeOut, 10.0f);
					IsPatternPlaying = true;
				}
				else if (90 > percent&&percent >= 80) {	//10%확률로
					//파이어볼
					FireBall();
					IsPatternPlaying = true;
				}
				else {									//10%확률로
					//브레스
					FireBreath();
					IsPatternPlaying = true;
				}
			}
			else if (!InCombatSphere && InAgroSphere2 && InAgroSphere3) {	//중거리
				if (percent < 40) {						//50%확률로
					//브레스
					FireBreath();
					IsPatternPlaying = true;
				}
				else if (60 > percent&&percent >= 40) {	//20%확률로
					//파이어볼
					FireBall();
					IsPatternPlaying = true;
				}
				else {									//30%확률로
					//근거리까지 이동 후 근접공격
					MeleeAttackOn = true;
					MoveToTarget();
					GetWorldTimerManager().SetTimer(MoveTimer, this, &ADragon::MoveTimeOut, 10.0f);
					IsPatternPlaying = true;
				}


			}
			else if (!InCombatSphere && !InAgroSphere2 && InAgroSphere3) {	//장거리
				if (percent < 70) {						//70%확률로
					//파이어볼
					FireBall();
					IsPatternPlaying = true;
				}
				else if (90 > percent&&percent >= 70) {	//20%확률로
					//중거리까지 이동 후 브레스
					MoveToTarget();
					GetWorldTimerManager().SetTimer(MoveTimer, this, &ADragon::MoveTimeOut, 5.0f);
					FireBreathOn = true;
					IsPatternPlaying = true;
				}
				else {									//10%확률로
					//근거리까지 이동 후 근접
					MeleeAttackOn = true;
					MoveToTarget();
					GetWorldTimerManager().SetTimer(MoveTimer, this, &ADragon::MoveTimeOut, 10.0f);
					IsPatternPlaying = true;
				}


			}

		}
		else if (!IsPatternPlaying&&IsInAir) {
			FireBall_Air();
			IsPatternPlaying = true;
		}
	}
}
void ADragon::TurnEnd() {
	IsPatternPlaying = false;
}
void ADragon::MoveToTarget()
{
	FAIMoveRequest MoveRequest;
	MoveRequest.SetAcceptanceRadius(0.0f);
	MoveRequest.SetGoalActor(AgroTarget);
	FNavPathSharedPtr NavPath;
	AIController->MoveTo(MoveRequest, &NavPath);
}
void ADragon::MoveTimeOut() {
	AIController->StopMovement();
	IsPatternPlaying = false;
}

void ADragon::AgroSphereOnOverlapBegin_2(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && Alive())
	{
		APlayerCharacter* Main = Cast<APlayerCharacter>(OtherActor);
		if (Main)
		{
			InAgroSphere2 = true;
			if (FireBreathOn) {
				GetWorldTimerManager().SetTimer(AttackWaitTimer, this, &ADragon::FireBreath, 0.1f, true);
			}

		}
	}
}

void ADragon::AgroSphereOnOverlapEnd_2(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		APlayerCharacter* Main = Cast<APlayerCharacter>(OtherActor);
		if (Main)
		{
			InAgroSphere2 = false;
		}
	}
}
void ADragon::AgroSphereOnOverlapBegin_3(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && Alive())
	{
		APlayerCharacter* Main = Cast<APlayerCharacter>(OtherActor);
		if (Main)
		{
			AgroTarget = Main;
			InAgroSphere3 = true;
			HuntTarget = AgroTarget;
			float SkillCoolTime = FMath::FRandRange(1, 3);
			GetWorldTimerManager().SetTimer(SkillTimer, this, &ADragon::Pattern_Selector, SkillCoolTime, true);
			GetWorldTimerManager().SetTimer(FlyTimer, this, &ADragon::FlyToAir, 3.0f);
		}
	}
}

void ADragon::AgroSphereOnOverlapEnd_3(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		APlayerCharacter* Main = Cast<APlayerCharacter>(OtherActor);
		if (Main)
		{
			InAgroSphere3 = false;
			if (IsInGround)
				MoveToTarget();
		}
	}
}
void ADragon::CombatSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		APlayerCharacter* Main = Cast<APlayerCharacter>(OtherActor);
		if (Main)
		{
			InCombatSphere = true;
			if (MeleeAttackOn) {
				GetWorldTimerManager().SetTimer(AttackWaitTimer, this, &ADragon::SelectAttack, 0.1f, true);
			}
		}
	}
}

void ADragon::CombatSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		APlayerCharacter* Main = Cast<APlayerCharacter>(OtherActor);
		if (Main)
		{
			InCombatSphere = false;

		}
	}
}

void ADragon::CombatCollisionOnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		Player = Cast<APlayerCharacter>(OtherActor);
		if (Player)
		{
			if (DamageTypeClass)
			{
				UGameplayStatics::ApplyDamage(Player, EnemyDamage, AIController, this, DamageTypeClass);
			}
		}
	}
}
void ADragon::CombatCollisionOnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}

void ADragon::CombatCollisionOnOverlapBegin2(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		Player = Cast<APlayerCharacter>(OtherActor);
		if (Player)
		{
			if (DamageTypeClass)
			{
				UGameplayStatics::ApplyDamage(Player, EnemyDamage, AIController, this, DamageTypeClass);
			}
		}
	}
}
void ADragon::CombatCollisionOnOverlapEnd2(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}
void ADragon::CombatCollisionOnOverlapBegin3(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		Player = Cast<APlayerCharacter>(OtherActor);
		if (Player)
		{

			if (DamageTypeClass)
			{
				UGameplayStatics::ApplyDamage(Player, EnemyDamage, AIController, this, DamageTypeClass);
			}
		}
	}
}
void ADragon::FlyToAir() {
	if (IsPatternPlaying) {
		GetWorldTimerManager().SetTimer(FlyTimer, this, &ADragon::FlyToAir, 0.1f, true);
	}
	else {
		IsPatternPlaying = true;
		MeleeAttackOn = false;
		FireBreathOn = false;
		bAttacking = false;
		ABAnim->PlayAttackMontage();
		ABAnim->JumpToAttackMontageSection_2(5);
	}


}
void ADragon::HeightIncrease() {	//2.1초	210번
	FVector loc2 = FVector(0.0f, 0.0f, 0.0001f);

	float Z = 0;
	Z = Z + 10;
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, GetActorLocation().Z + loc2.Z), FRotator(0.0f, -90.0f, 0.0f));
	if (GetActorLocation().Z > 500) {
		GetWorldTimerManager().ClearTimer(FlyUpTimer);
		GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, GetActorLocation().Z), FRotator(0.0f, -90.0f, 0.0f));
	}

}
void ADragon::FlyToAir_End() {
	IsInAir = true;
	IsInGround = false;
	IsPatternPlaying = false;

	GetWorldTimerManager().ClearTimer(FlyUpTimer);
	GetWorldTimerManager().SetTimer(FlyTimer, this, &ADragon::LandToGround, 10.0f);
}
void ADragon::LandToGround() {
	if (IsPatternPlaying) {
		GetWorldTimerManager().SetTimer(FlyTimer, this, &ADragon::LandToGround, 0.1f, true);
	}
	else {
		IsPatternPlaying = true;
		ABAnim->PlayAttackMontage();
		ABAnim->JumpToAttackMontageSection_2(6);
	}
}
void ADragon::HeightDecrease() {	//2.45초 24번
	FVector loc2 = FVector(0.0f, 0.0f, -1.0f);
	float Z = 0;
	Z = Z + 10;

	FVector loc = FMath::VInterpTo(GetActorLocation(), GetActorLocation() + loc2, DeltaTime2, 5.0f);
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, loc.Z), FRotator(0.0f, -90.0f, 0.0f));
	if (GetActorLocation().Z < -405) {
		GetWorldTimerManager().ClearTimer(FlyUpTimer);
		GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, GetActorLocation().Z), FRotator(0.0f, -90.0f, 0.0f));
	}
}
void ADragon::LandToGround_End() {
	IsInAir = false;
	IsInGround = true;
	IsPatternPlaying = false;
	MeleeAttackOn = false;
	FireBreathOn = false;
	bAttacking = false;
	GetWorldTimerManager().ClearTimer(FlyUpTimer);
	GetWorldTimerManager().SetTimer(FlyTimer, this, &ADragon::FlyToAir, 20.0f);
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -405.912659f), FRotator(0.0f, -90.0f, 0.0f));



}
void ADragon::Damage() {
	UGameplayStatics::ApplyDamage(Player, EnemyDamage, AIController, this, DamageTypeClass);
}
void ADragon::CombatCollisionOnOverlapEnd3(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	GetWorldTimerManager().ClearTimer(DamageTimer);

}
void ADragon::SelectAttack()
{
	float Dis = FVector::Distance(this->GetActorLocation(), AgroTarget->GetActorLocation());
	FRotator BetweenAngle = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), AgroTarget->GetActorLocation());
	FRotator EnemyLocation = UKismetMathLibrary::NormalizedDeltaRotator(GetActorRotation(), BetweenAngle);
	if (Dis > 650) {
		Attack();
	}
	else if (UKismetMathLibrary::InRange_FloatFloat(EnemyLocation.Yaw, -20.0f, 0.0f)) {
		Attack_right();
		FAIMoveRequest MoveRequest;
		MoveRequest.SetAcceptanceRadius(0.0f);
		MoveRequest.SetGoalLocation(GetActorLocation() + GetActorForwardVector() * 100.0f);
		FNavPathSharedPtr NavPath;
		AIController->MoveTo(MoveRequest, &NavPath);
	}
	else if (UKismetMathLibrary::InRange_FloatFloat(EnemyLocation.Yaw, 0.0f, 20.0f)) {
		Attack_left();
		FAIMoveRequest MoveRequest;
		MoveRequest.SetAcceptanceRadius(0.0f);
		MoveRequest.SetGoalLocation(GetActorLocation() + GetActorForwardVector() * 100.0f);
		FNavPathSharedPtr NavPath;
		AIController->MoveTo(MoveRequest, &NavPath);
	}
	else {
		Attack_left();
	}
}

void ADragon::Attack()
{
	GetWorldTimerManager().ClearTimer(MoveTimer);
	GetWorldTimerManager().ClearTimer(AttackWaitTimer);
	MeleeAttackOn = false;
	if (true)
	{
		bAttacking = true;
		if (AIController)
		{
			AIController->StopMovement();
		}

		if (ABAnim)
		{
			ABAnim->PlayAttackMontage();
			ABAnim->JumpToAttackMontageSection_2(0);
		}
	}
}
void ADragon::Attack_left()
{
	GetWorldTimerManager().ClearTimer(MoveTimer);
	GetWorldTimerManager().ClearTimer(AttackWaitTimer);
	MeleeAttackOn = false;
	if (true) 
	{
		bAttacking = true;
		if (AIController)
		{
			AIController->StopMovement();
		}
		if (ABAnim)
		{
			ABAnim->PlayAttackMontage();
			ABAnim->JumpToAttackMontageSection_2(1);
		}
	}
}
void ADragon::Attack_right()
{
	GetWorldTimerManager().ClearTimer(MoveTimer);
	GetWorldTimerManager().ClearTimer(AttackWaitTimer);
	MeleeAttackOn = false;
	if (true) 
	{
		bAttacking = true;
		if (AIController)
		{
			AIController->StopMovement();
		}

		if (ABAnim)
		{
			ABAnim->PlayAttackMontage();
			ABAnim->JumpToAttackMontageSection_2(2);
		}
	}
}

void ADragon::AttackEnd() {
	IsPatternPlaying = false;
	IsTurnning = false;
	bAttacking = false;
}
float ADragon::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	if (IsInGround) {
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
		}
	}
	return DamageAmount;
}

void ADragon::FireBreath() {
	if (AIController)
	{
		AIController->StopMovement();
	}
	GetWorldTimerManager().ClearTimer(MoveTimer);
	GetWorldTimerManager().ClearTimer(AttackWaitTimer);
	FireBreathOn = false;
	bAttacking = true;
	if (ABAnim)
	{
		ABAnim->PlayAttackMontage();
		ABAnim->JumpToAttackMontageSection_2(3);
	}

}
void ADragon::FireBall() {
	if (AIController)
	{
		AIController->StopMovement();
	}
	GetWorldTimerManager().ClearTimer(MoveTimer);
	GetWorldTimerManager().ClearTimer(AttackWaitTimer);
	bAttacking = true;
	if (ABAnim)
	{
		ABAnim->PlayAttackMontage();
		ABAnim->JumpToAttackMontageSection_2(4);
	}

}
void ADragon::FireBall_Air() {
	if (AIController)
	{
		AIController->StopMovement();
	}
	bAttacking = true;
	if (ABAnim)
	{
		ABAnim->PlayAttackMontage();
		ABAnim->JumpToAttackMontageSection_2(7);
	}

}
void ADragon::FireFireBall() {
	if (ProjectileClass)
	{
		FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), AgroTarget->GetActorLocation());
		FRotator LookAtRotationYaw(0.f, 0.f, LookAtRotation.Yaw);
		FRotator test(0.f, 30.f, 0.0f);

		FVector ALocation = GetActorLocation();
		FVector MuzzleLocation(ALocation.X, ALocation.Y, ALocation.Z - 300);
		FRotator MuzzleRotation1 = GetActorRotation();
		FRotator MuzzleRotation2 = MuzzleRotation1 + test;
		FRotator MuzzleRotation3 = MuzzleRotation1 - test;
		FRotator MuzzleRotation4 = MuzzleRotation1 - test - test;
		FRotator MuzzleRotation5 = MuzzleRotation1 + test + test;
		UWorld* World = GetWorld();
		if (World)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			AFireBall* Projectile = World->SpawnActor<AFireBall>(ProjectileClass, MuzzleLocation + GetActorForwardVector() * 600.0f, MuzzleRotation1, SpawnParams);
			if (Projectile)
			{
				FVector LaunchDirection = MuzzleRotation1.Vector();
				Projectile->FireInDirection(LaunchDirection);
				Projectile->Fire_GetAIController(AIController);

			}
			AFireBall* Projectile2 = World->SpawnActor<AFireBall>(ProjectileClass, MuzzleLocation + GetActorForwardVector() * 600.0f, MuzzleRotation2, SpawnParams);
			if (Projectile2)
			{
				FVector LaunchDirection = MuzzleRotation2.Vector();
				Projectile2->FireInDirection(LaunchDirection);
				Projectile2->Fire_GetAIController(AIController);

			}
			AFireBall* Projectile3 = World->SpawnActor<AFireBall>(ProjectileClass, MuzzleLocation + GetActorForwardVector() * 600.0f, MuzzleRotation3, SpawnParams);
			if (Projectile3)
			{
				FVector LaunchDirection = MuzzleRotation3.Vector();
				Projectile3->FireInDirection(LaunchDirection);
				Projectile3->Fire_GetAIController(AIController);

			}
			AFireBall* Projectile4 = World->SpawnActor<AFireBall>(ProjectileClass, MuzzleLocation + GetActorForwardVector() * 600.0f, MuzzleRotation4, SpawnParams);
			if (Projectile4)
			{
				FVector LaunchDirection = MuzzleRotation4.Vector();
				Projectile4->FireInDirection(LaunchDirection);
				Projectile4->Fire_GetAIController(AIController);

			}
			AFireBall* Projectile5 = World->SpawnActor<AFireBall>(ProjectileClass, MuzzleLocation + GetActorForwardVector() * 600.0f, MuzzleRotation5, SpawnParams);
			if (Projectile5)
			{
				FVector LaunchDirection = MuzzleRotation5.Vector();
				Projectile5->FireInDirection(LaunchDirection);
				Projectile5->Fire_GetAIController(AIController);

			}
		}
	}
}
void ADragon::FireFireBall_Air() {

	if (ProjectileClass)
	{
		FVector ALocation = GetActorLocation();
		FVector MuzzleLocation(0.0f, 0.0f, 700.0f);
		FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation() + MuzzleLocation + GetActorForwardVector() * 500.0f, AgroTarget->GetActorLocation());
		FRotator LookAtRotationYaw(0.f, 0.f, LookAtRotation.Yaw);
		FRotator test(0.f, 30.f, 0.0f);


		FRotator MuzzleRotation1 = GetActorRotation();
		UWorld* World = GetWorld();
		if (World)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			AFireBall* Projectile = World->SpawnActor<AFireBall>(ProjectileClass, GetActorLocation() + MuzzleLocation + GetActorForwardVector() * 500.0f, LookAtRotation, SpawnParams);
			if (Projectile)
			{
				FVector LaunchDirection = LookAtRotation.Vector();
				Projectile->ProjectileMovementComponent->InitialSpeed = 1500.0f;
				Projectile->ProjectileMovementComponent->MaxSpeed = 1500.0f;
				Projectile->FireInDirection(LaunchDirection);
				Projectile->Fire_GetAIController(AIController);

			}
		}
	}
	if (ProjectileClass)
	{
		UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetNavigationSystem(GetWorld());
		if (NavSystem != nullptr)
		{
			FNavLocation SpawnLocation;
			NavSystem->GetRandomPointInNavigableRadius(AgroTarget->GetActorLocation(), 500.0f, SpawnLocation);
			FVector SpawnVector = FVector(SpawnLocation.Location.X, SpawnLocation.Location.Y, GetActorLocation().Z);

			FVector ALocation = GetActorLocation();
			FVector MuzzleLocation(0.0f, 0.0f, 700.0f);
			FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation() + MuzzleLocation + GetActorForwardVector() * 500.0f, SpawnVector);
			FRotator LookAtRotationYaw(0.f, 0.f, LookAtRotation.Yaw);
			FRotator test(0.f, 30.f, 0.0f);


			FRotator MuzzleRotation1 = GetActorRotation();
			UWorld* World = GetWorld();
			if (World)
			{
				FActorSpawnParameters SpawnParams;
				SpawnParams.Owner = this;
				AFireBall* Projectile = World->SpawnActor<AFireBall>(ProjectileClass, GetActorLocation() + MuzzleLocation + GetActorForwardVector() * 500.0f, LookAtRotation, SpawnParams);
				if (Projectile)
				{
					FVector LaunchDirection = LookAtRotation.Vector();
					Projectile->ProjectileMovementComponent->InitialSpeed = 1500.0f;
					Projectile->ProjectileMovementComponent->MaxSpeed = 1500.0f;
					Projectile->FireInDirection(LaunchDirection);
					Projectile->Fire_GetAIController(AIController);

				}
			}
		}
	}
	if (ProjectileClass)
	{
		UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetNavigationSystem(GetWorld());
		if (NavSystem != nullptr)
		{
			FNavLocation SpawnLocation;
			NavSystem->GetRandomPointInNavigableRadius(AgroTarget->GetActorLocation(), 500.0f, SpawnLocation);
			FVector SpawnVector = FVector(SpawnLocation.Location.X, SpawnLocation.Location.Y, GetActorLocation().Z);
			FVector ALocation = GetActorLocation();
			FVector MuzzleLocation(0.0f, 0.0f, 700.0f);
			FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation() + MuzzleLocation + GetActorForwardVector() * 500.0f, SpawnVector);
			FRotator LookAtRotationYaw(0.f, 0.f, LookAtRotation.Yaw);
			FRotator test(0.f, 30.f, 0.0f);


			FRotator MuzzleRotation1 = GetActorRotation();
			UWorld* World = GetWorld();
			if (World)
			{
				FActorSpawnParameters SpawnParams;
				SpawnParams.Owner = this;
				AFireBall* Projectile = World->SpawnActor<AFireBall>(ProjectileClass, GetActorLocation() + MuzzleLocation + GetActorForwardVector() * 500.0f, LookAtRotation, SpawnParams);
				if (Projectile)
				{
					FVector LaunchDirection = LookAtRotation.Vector();
					Projectile->ProjectileMovementComponent->InitialSpeed = 1500.0f;
					Projectile->ProjectileMovementComponent->MaxSpeed = 1500.0f;
					Projectile->FireInDirection(LaunchDirection);
					Projectile->Fire_GetAIController(AIController);

				}
			}
		}
	}
	if (ProjectileClass)
	{
		UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetNavigationSystem(GetWorld());
		if (NavSystem != nullptr)
		{
			FNavLocation SpawnLocation;
			NavSystem->GetRandomPointInNavigableRadius(AgroTarget->GetActorLocation(), 500.0f, SpawnLocation);
			FVector SpawnVector = FVector(SpawnLocation.Location.X, SpawnLocation.Location.Y, GetActorLocation().Z);

			FVector ALocation = GetActorLocation();
			FVector MuzzleLocation(0.0f, 0.0f, 700.0f);
			FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation() + MuzzleLocation + GetActorForwardVector() * 500.0f, SpawnVector);
			FRotator LookAtRotationYaw(0.f, 0.f, LookAtRotation.Yaw);
			FRotator test(0.f, 30.f, 0.0f);


			FRotator MuzzleRotation1 = GetActorRotation();
			UWorld* World = GetWorld();
			if (World)
			{
				FActorSpawnParameters SpawnParams;
				SpawnParams.Owner = this;
				AFireBall* Projectile = World->SpawnActor<AFireBall>(ProjectileClass, GetActorLocation() + MuzzleLocation + GetActorForwardVector() * 500.0f, LookAtRotation, SpawnParams);
				if (Projectile)
				{
					FVector LaunchDirection = LookAtRotation.Vector();
					Projectile->ProjectileMovementComponent->InitialSpeed = 1500.0f;
					Projectile->ProjectileMovementComponent->MaxSpeed = 1500.0f;
					Projectile->FireInDirection(LaunchDirection);
					Projectile->Fire_GetAIController(AIController);

				}
			}
		}
	}
}

void ADragon::Die()
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

bool ADragon::Alive()
{
	return GetEnemyMovementStatus() != EEnemyMovementStatus::EMS_Dead;
}