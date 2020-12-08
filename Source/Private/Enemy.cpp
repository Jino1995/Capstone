// Fill out your copyright notice in the Description page of Project Settings.

#include "Enemy.h"
#include "AIController.h"
#include "PlayerCharacter.h"
#include "Components/BoxComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Sound/SoundCue.h"
#include "EnemyAnimInstance.h"
#include "TimerManager.h"
#include "Kismet/KismetMathLibrary.h"
#include "ABPlayerController.h"
#include "Components/WidgetComponent.h"
#include "Blueprint/UserWidget.h"
#include "EnemyWidget.h"
#include "Shield.h"
#include "ABAnimInstance.h"
#include "Engine.h"
#include "DamageText.h"

// Sets default values
AEnemy::AEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -88.0f), FRotator(0.0f, -90.0f, 0.0f));
	
	AgroSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AgroSphere"));
	AgroSphere->SetupAttachment(RootComponent);
	AgroSphere->InitSphereRadius(800.0f);

	CombatSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CombatSphere"));
	CombatSphere->SetupAttachment(RootComponent);
	CombatSphere->InitSphereRadius(75.0f);
	
	CombatCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("CombatCollision"));

	bOverlappingCombatSphere = false;

	AttackMinTime = 0.5f;
	AttackMaxTime = 3.5f;

	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);	
	bUseControllerRotationYaw = false;

	IsDead = false;

	DeathDelay = 3.0f;
	
	HPBarWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HPBARWIDGET"));
	HPBarWidget->SetupAttachment(GetMesh());
	HPBarWidget->SetRelativeLocation(FVector(0.0f, 0.0f, 180.0f));
	HPBarWidget->SetWidgetSpace(EWidgetSpace::Screen);
	
	static ConstructorHelpers::FClassFinder<UUserWidget> UI_HUD(TEXT("/Game/HUD/UI_EnemyHP_2.UI_EnemyHP_2_C"));
	if (UI_HUD.Succeeded())
	{
		HPBarWidget->SetWidgetClass(UI_HUD.Class);
		HPBarWidget->SetDrawSize(FVector2D(150.0f, 100.0f));
		HPBarWidget->SetVisibility(false);
	}

	TargetPointWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("TARGETPOINT"));
	TargetPointWidget->SetupAttachment(GetMesh());
	TargetPointWidget->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	TargetPointWidget->SetWidgetSpace(EWidgetSpace::Screen);

	static ConstructorHelpers::FClassFinder<UUserWidget> TargetPoint_Widget(TEXT("/Game/HUD/TargetPoint/TargetPoint.TargetPoint_C"));
	if (TargetPoint_Widget.Succeeded())
	{
		TargetPointWidget->SetWidgetClass(TargetPoint_Widget.Class);
		TargetPointWidget->SetDrawSize(FVector2D(10.0f, 10.0f));
		TargetPointWidget->SetVisibility(false);
	}

	DamageText = CreateDefaultSubobject<UWidgetComponent>(TEXT("DAMAGETEXT"));
	DamageText->SetupAttachment(GetMesh());
	DamageText->SetRelativeLocation(FVector(0.0f, 0.0f, 100.0f));
	DamageText->SetWidgetSpace(EWidgetSpace::Screen);

	static ConstructorHelpers::FClassFinder<UUserWidget> DamageText_Widget(TEXT("/Game/HUD/FloatText_BP.FloatText_BP_C"));
	if (DamageText_Widget.Succeeded())
	{
		DamageText->SetWidgetClass(DamageText_Widget.Class);
		DamageText->SetDrawSize(FVector2D(10.0f, 10.0f));
		DamageText->SetVisibility(true);
	}
	
	//bBlocked = false;
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	AIController = Cast<AAIController>(GetController());

	AgroSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Ignore);
	AgroSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	CombatSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Ignore);

	
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CombatCollision->SetCollisionProfileName(TEXT("EnemyAttack"));

	GetCapsuleComponent()->SetGenerateOverlapEvents(false);
	GetMesh()->SetGenerateOverlapEvents(true);
	
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	auto CWidget = Cast<UEnemyWidget>(HPBarWidget->GetUserWidgetObject());
	if (CWidget != nullptr)
	{
		CWidget->BindCharacterStat(this);
	}
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEnemy::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	CombatCollision->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName("AttackSocket"));
	ABAnim = Cast<UEnemyAnimInstance>(GetMesh()->GetAnimInstance());
}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemy::MoveToTarget(APlayerCharacter* Target)
{
	SetEnemyMovementStatus(EEnemyMovementStatus::EMS_MoveToTarget);

	if (AIController)
	{
		FAIMoveRequest MoveRequest;
		MoveRequest.SetGoalActor(Target);
		MoveRequest.SetAcceptanceRadius(0.0f);  // collision distance

		FNavPathSharedPtr NavPath;

		AIController->MoveTo(MoveRequest, &NavPath);
	}
}

void AEnemy::ActivateCollision()
{
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AEnemy::DeActivateCollision()
{
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AEnemy::Attack()
{
	
}

void AEnemy::AttackEnd()
{
	
}


float AEnemy::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{

	return DamageAmount;
}

void AEnemy::Die()
{

}

bool AEnemy::Alive()
{
	return GetEnemyMovementStatus() != EEnemyMovementStatus::EMS_Dead;
}

void AEnemy::Disappear()
{
	Destroy();
}

void AEnemy::BlockedAttack()
{
	bBlocked = true;
	DeActivateCollision();
	AttackEnd();
	
}

void AEnemy::SuccessAttack(APlayerCharacter* DamagedPlayer)
{
	if (DamagedPlayer->HitParticles)
	{
		const USkeletalMeshSocket* TipSocket = GetMesh()->GetSocketByName("TipSocket");
		if (TipSocket)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), DamagedPlayer->HitParticles, TipSocket->GetSocketLocation(GetMesh()), FRotator(0.0f), false);
		}
	}
	if (DamagedPlayer->HitSound)
	{
		UGameplayStatics::PlaySound2D(this, DamagedPlayer->HitSound);
	}
}

void AEnemy::ShowTargetPoint(bool bShow) 
{ 
	TargetPointWidget->SetVisibility(bShow); 
}

void AEnemy::ShowDamageText(int32 Damage)
{
	auto DWidget = Cast<UDamageText>(DamageText->GetUserWidgetObject());
	if (DWidget != nullptr)
	{
		DWidget->CreateDamageText(Damage);
	}
	
}

void AEnemy::ShowMoneyText(int32 Money)
{
	auto DWidget = Cast<UDamageText>(DamageText->GetUserWidgetObject());
	if (DWidget != nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("33"));
		DWidget->CreateMoneyText(Money);
	}

}

bool AEnemy::GetIsStrafing()
{
	return bStrafing;
}