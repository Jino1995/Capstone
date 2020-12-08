// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"
#include "GameFramework/Actor.h"
#include "Skill.h"
#include "Weapon.generated.h"

UENUM(BlueprintType)
enum class EWeaponTypes : uint8
{
	OneHandSW_Shield,
	TwoHandWeapon,
	Spear,
	Katana
};

UCLASS()
class RPG_PJT_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeapon();

	UPROPERTY(EditDefaultsOnly, Category = "SavedData")
	FString Name;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(VisibleAnywhere, Category = "Mesh")
	USkeletalMeshComponent* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Types", Meta = (AllowPrivateAccess = true))
	TMap<TSubclassOf<class ASkill>, int32> Projectiles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Types", Meta = (AllowPrivateAccess = true))
	EWeaponTypes WeaponType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack", Meta = (AllowPrivateAccess = true))
	UAnimMontage* AttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack", Meta = (AllowPrivateAccess = true))
	UAnimMontage* StrongAttackMontage;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack", Meta = (AllowPrivateAccess = true))
	UAnimMontage* EquippingMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack", Meta = (AllowPrivateAccess = true))
	UAnimMontage* BlockingMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Position", Meta = (AllowPrivateAccess = true))
	FName EquipPosition;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Position", Meta = (AllowPrivateAccess = true))
	FName UnEquipPosition;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack", Meta = (AllowPrivateAccess = true))
	bool bBlocking;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack", Meta = (AllowPrivateAccess = true))
	float AttackStamina;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack", Meta = (AllowPrivateAccess = true))
	float SkillStamina;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack", Meta = (AllowPrivateAccess = true))
	float SkillDamagePercent;

	TSet<class AEnemy*> DamagedEnemy;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	bool bHasSpecialAttack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	int32 ComboNum;

	UPROPERTY()
	class APlayerCharacter* WeaponOwner;

	UFUNCTION()
	virtual void CombaOnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	virtual void CombaOnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void ActivateCollision();
	void DeActivateCollision();

	
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item | Combat")
	class UBoxComponent* CombatCollision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Combat")
	float Damage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Sound")
	class USoundCue* OnEquipSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Sound")
	USoundCue* SwingSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Sound")
	USoundCue* EquipSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	TSubclassOf<UDamageType> DamageTypeClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	AController* WeaponInstigator;

public:
	void FireSkill();
	
	FORCEINLINE void SetWeaponOwner(APlayerCharacter* OwnerCharacter) { WeaponOwner = OwnerCharacter; }
	FORCEINLINE void SetInstigator(AController* Inst) { WeaponInstigator = Inst; }
	FORCEINLINE UAnimMontage* GetAttackMontage() { return AttackMontage; }
	FORCEINLINE UAnimMontage* GetStrongAttackMontage() { return StrongAttackMontage; }
	FORCEINLINE UAnimMontage* GetEquippingMontage() { return EquippingMontage; }
	FORCEINLINE UAnimMontage* GetBlockingMontage() { return BlockingMontage; }
	FORCEINLINE FName GetEquipPosion() { return EquipPosition; }
	FORCEINLINE FName GetUnEquipPosion() { return UnEquipPosition; }
	FORCEINLINE EWeaponTypes GetWeaponType() { return WeaponType; }
	FORCEINLINE bool HasBlocking() { return bBlocking; }
	FORCEINLINE float GetAttackStamina() { return AttackStamina; }
	FORCEINLINE float GetSkillStamina() { return SkillStamina; }
};
