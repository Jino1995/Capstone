// Fill out your copyright notice in the Description page of Project Settings.


#include "Fire2.h"
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
AFire2::AFire2()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
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



	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CombatCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	CombatCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CombatCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	CombatCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);

	CombatCollision->OnComponentBeginOverlap.AddDynamic(this, &AFire2::CombatCollisionOnOverlapBegin);
	CombatCollision->OnComponentEndOverlap.AddDynamic(this, &AFire2::CombatCollisionOnOverlapEnd);

}

// Called when the game starts or when spawned
void AFire2::BeginPlay()
{
	Super::BeginPlay();
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

// Called every frame
void AFire2::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AFire2::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AFire2::Fire2InDirection(const FVector& ShootDirection)
{
	ProjectileMovementComponent->Velocity = ShootDirection * ProjectileMovementComponent->InitialSpeed;
}


void AFire2::CombatCollisionOnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

	float EnemyDamage = 100.0f;
	if (OtherActor)
	{
		APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor);
		if (Player)
		{
			if (DamageTypeClass)
			{
			}
		}
	}
}
void AFire2::Fire2_GetAIController(class AAIController* aa) {
	AIController = aa;
}

void AFire2::CombatCollisionOnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}
void AFire2::SuccessAttack(APlayerCharacter* DamagedPlayer)
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
