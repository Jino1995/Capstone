// Fill out your copyright notice in the Description page of Project Settings.


#include "Skill.h"
#include "Enemy.h"
#include "PlayerCharacter.h"
#include "TimerManager.h"

// Sets default values
ASkill::ASkill()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MESH"));
	CollisionRange = CreateDefaultSubobject<USphereComponent>(TEXT("COLLISIONRANGE"));
	
	SetRootComponent(CollisionRange);
	Mesh->SetupAttachment(CollisionRange);
	CollisionRange->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->SetUpdatedComponent(CollisionRange);
	ProjectileMovementComponent->InitialSpeed = 3000.0f;
	ProjectileMovementComponent->MaxSpeed = 3000.0f;
	ProjectileMovementComponent->bRotationFollowsVelocity = false;
}

// Called when the game starts or when spawned
void ASkill::BeginPlay()
{
	Super::BeginPlay();
	
	CollisionRange->OnComponentBeginOverlap.AddDynamic(this, &ASkill::CombaOnOverlapBegin);
	CollisionRange->OnComponentEndOverlap.AddDynamic(this, &ASkill::CombaOnOverlapEnd);

	CollisionRange->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionRange->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	CollisionRange->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CollisionRange->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	GetWorldTimerManager().SetTimer(DestroyTimer, this, &ASkill::Dead, 0.5f);
}

void ASkill::CombaOnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Error, TEXT("OverlapCHICKEN"));
	if (OtherActor)
	{
		UE_LOG(LogTemp, Error, TEXT("OverlapSuccessCHICKEN"));
		AEnemy* Enemy = Cast<AEnemy>(OtherActor);
		if (Enemy)
		{
			if (DamageTypeClass)
			{
				UE_LOG(LogTemp, Error, TEXT("DMGCHICKEN"));
				UGameplayStatics::PlaySound2D(GetWorld(), SkillHitSound);
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SkillParticle, GetActorLocation());
				UGameplayStatics::ApplyDamage(Enemy, Cast<APlayerCharacter>(GetOwner())->GetCharacterStat()->GetAttack()*SkillDamagePercent, Cast<AABPlayerController>(GetInstigator()), Cast<APlayerCharacter>(GetOwner()), DamageTypeClass);
				Destroy();
			}
		}
	}
}

void ASkill::CombaOnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	
}

void ASkill::FireInDirection(const FVector& ShootDirection)
{
	ProjectileMovementComponent->Velocity = ShootDirection * ProjectileMovementComponent->InitialSpeed;
}

void ASkill::Dead()
{
	Destroy();
}