// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"
#include "GameFramework/Actor.h"
#include "Shield.generated.h"

UCLASS()
class RPG_PJT_API AShield : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AShield();

	UPROPERTY(EditDefaultsOnly, Category = "SavedData")
	FString Name;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, Category = "Mesh")
	USkeletalMeshComponent* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montage", Meta = (AllowPrivateAccess = true))
	UAnimMontage* BlockingMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Position", Meta = (AllowPrivateAccess = true))
	FName EquipPosition;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Position", Meta = (AllowPrivateAccess = true))
	FName UnEquipPosition;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Sound")
	class USoundCue* BlockingSound;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	AController* ShieldInstigator;

	FORCEINLINE void SetInstigator(AController* Inst) { ShieldInstigator = Inst; }
	FORCEINLINE UAnimMontage* GetBlockingMontage() { return BlockingMontage; }
	FORCEINLINE FName GetEquipPosition() { return EquipPosition; }
	FORCEINLINE FName GetUnEquipPosition() { return UnEquipPosition; }
};
