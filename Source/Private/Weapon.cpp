// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "PlayerCharacter.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Engine/StaticMeshSocket.h"
#include "Enemy.h"
#include "Sound/SoundCue.h"
#include "Engine.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Skill.h"

// Sets default values
AWeapon::AWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MESH"));

	SetRootComponent(Mesh);

	Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	Mesh->SetSimulatePhysics(false);

	CombatCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("CombatCollision"));
	CombatCollision->SetupAttachment(RootComponent);
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Damage = 25.0f;
}

// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	CombatCollision->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::CombaOnOverlapBegin);
	CombatCollision->OnComponentEndOverlap.AddDynamic(this, &AWeapon::CombaOnOverlapEnd);

	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CombatCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	CombatCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CombatCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
}

// Called every frame
void AWeapon::Tick(float DeltaTime)
{

}

void AWeapon::CombaOnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		AEnemy* Enemy = Cast<AEnemy>(OtherActor);
		if (Enemy)
		{
			if (DamageTypeClass)
			{
				if (!DamagedEnemy.Contains(Enemy))
				{
					DamagedEnemy.Emplace(Enemy);
					if (Enemy->HitParticles)
					{
						const USkeletalMeshSocket* WeaponSocket = Mesh->GetSocketByName("WeaponSocket");
						if (WeaponSocket)
						{
							WeaponSocket->GetSocketLocation(Mesh);
							UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Enemy->HitParticles, WeaponSocket->GetSocketLocation(Mesh), FRotator(0.0f), false);
						}

					}
					if (Enemy->HitSound)
					{
						UGameplayStatics::PlaySound2D(this, Enemy->HitSound);
					}

					if (WeaponOwner->IsSkillAttacking())
					{
						UGameplayStatics::ApplyDamage(Enemy, WeaponOwner->GetCharacterStat()->GetAttack()*SkillDamagePercent, WeaponInstigator, this, DamageTypeClass);
					}
					else
					{
						UGameplayStatics::ApplyDamage(Enemy, WeaponOwner->GetCharacterStat()->GetAttack(), WeaponInstigator, this, DamageTypeClass);
					}
				}
			}
		}
	}
}

void AWeapon::CombaOnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	
}

void AWeapon::ActivateCollision()
{
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AWeapon::DeActivateCollision()
{
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	DamagedEnemy.Empty();
}

void AWeapon::FireSkill()
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = WeaponOwner;
	SpawnParams.Instigator = GetInstigator();
	
	FVector SpawnLocation = WeaponOwner->GetMesh()->GetSocketLocation(FName("Projectile_Socket"));
	
	for (auto SpawnTarget: Projectiles)
	{
		int RandNum = rand()%100;
		if (RandNum <= SpawnTarget.Value)
		{
			ASkill* Projectile = GetWorld()->SpawnActor<ASkill>(SpawnTarget.Key, SpawnLocation, WeaponOwner->GetActorRotation(), SpawnParams);
			if (Projectile)
			{
				FVector LaunchDirection = WeaponOwner->GetActorRotation().Vector();
				Projectile->FireInDirection(LaunchDirection);
			}
			break;
		}
	}
}