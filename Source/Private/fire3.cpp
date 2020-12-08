// Fill out your copyright notice in the Description page of Project Settings.


#include "fire3.h"
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
Afire3::Afire3()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	CombatCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	CombatCollision->InitSphereRadius(35.0f);
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
void Afire3::BeginPlay()
{
	Super::BeginPlay();
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CombatCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	CombatCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CombatCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	CombatCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);

	CombatCollision->OnComponentBeginOverlap.AddDynamic(this, &Afire3::CombatCollisionOnOverlapBegin);
	CombatCollision->OnComponentEndOverlap.AddDynamic(this, &Afire3::CombatCollisionOnOverlapEnd);
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

// Called every frame
void Afire3::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void Afire3::FireInDirection(const FVector& ShootDirection)
{
	ProjectileMovementComponent->Velocity = ShootDirection * ProjectileMovementComponent->InitialSpeed;
}

void Afire3::FireInDirection2()
{
	InitialLifeSpan = 10.0f;
	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), target->GetActorLocation());
	FRotator LookAtRotationYaw(0.f, LookAtRotation.Yaw, LookAtRotation.Yaw);
	FVector ALocation = GetActorLocation();
	FVector MuzzleLocation(ALocation.X, ALocation.Y, ALocation.Z);
	FRotator MuzzleRotation = LookAtRotation;
	ProjectileMovementComponent->InitialSpeed = 800.0f;
	ProjectileMovementComponent->MaxSpeed = 800.0f;
	FVector LaunchDirection = MuzzleRotation.Vector();
	FireInDirection(LaunchDirection);

}
void Afire3::SetTarget(APlayerCharacter* tg)
{
	target = tg;
}

void Afire3::CombatCollisionOnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		Player = Cast<APlayerCharacter>(OtherActor);
		if (Player)
		{
			if (DamageTypeClass)
			{
				UGameplayStatics::ApplyDamage(Player, 20, a, this, DamageTypeClass);
				GetWorldTimerManager().SetTimer(DamageTimer, this, &Afire3::Damage, 0.8, true);
			}
		}
	}
}
void Afire3::Damage() 
{
	UGameplayStatics::ApplyDamage(Player, 20, a, this, DamageTypeClass);
}

void Afire3::Fire_GetAIController(class AAIController* aa) 
{
	AIController = aa;
}

void Afire3::CombatCollisionOnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	GetWorldTimerManager().ClearTimer(DamageTimer);
}