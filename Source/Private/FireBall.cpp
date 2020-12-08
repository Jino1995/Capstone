// Fill out your copyright notice in the Description page of Project Settings.


#include "FireBall.h"
#include "Enemy.h"
#include "Engine/Classes/Components/SphereComponent.h"
#include "Engine/Classes/GameFramework/ProjectileMovementComponent.h"

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
AFireBall::AFireBall()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	CombatCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	CombatCollision->InitSphereRadius(15.0f);
	RootComponent = CombatCollision;

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->SetUpdatedComponent(CombatCollision);
	ProjectileMovementComponent->InitialSpeed = 100.0f;
	ProjectileMovementComponent->MaxSpeed = 100.0f;
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->bShouldBounce = true;
	ProjectileMovementComponent->Bounciness = 0.3f;
}

// Called when the game starts or when spawned
void AFireBall::BeginPlay()
{
	Super::BeginPlay();
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CombatCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	CombatCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CombatCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	CombatCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);

	CombatCollision->OnComponentBeginOverlap.AddDynamic(this, &AFireBall::CombatCollisionOnOverlapBegin);
	CombatCollision->OnComponentEndOverlap.AddDynamic(this, &AFireBall::CombatCollisionOnOverlapEnd);
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

// Called every frame
void AFireBall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AFireBall::FireInDirection(const FVector& ShootDirection)
{
	ProjectileMovementComponent->Velocity = ShootDirection * ProjectileMovementComponent->InitialSpeed;
}


void AFireBall::CombatCollisionOnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		Player = Cast<APlayerCharacter>(OtherActor);
		if (Player)
		{
			if (DamageTypeClass)
			{
				UGameplayStatics::ApplyDamage(Player, 50, a, this, DamageTypeClass);
			}
		}
	}

}
void AFireBall::Damage() 
{
	UGameplayStatics::ApplyDamage(Player, 20, a, this, DamageTypeClass);
}

void AFireBall::Fire_GetAIController(class AAIController* aa)
{
	AIController = aa;
}

void AFireBall::CombatCollisionOnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	GetWorldTimerManager().ClearTimer(DamageTimer);
}