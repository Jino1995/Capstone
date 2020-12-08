// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"
#include "GameFramework/Character.h"
#include "Npc.generated.h"

UENUM(BlueprintType)
enum class ENPCCategory :uint8
{
	Trader,
	Trainer
};

UCLASS()
class RPG_PJT_API ANpc : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ANpc();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction", Meta=(AllowPrivateAccess=true))
	class USphereComponent* InteractionSphere;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Items", Meta = (AllowPrivateAccess = true))
	TArray<FName> SellingItems;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
	class UWidgetComponent* NameTextWidget;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = UI, Meta = (AllowPrivateAccess = true))
	class UWidgetComponent* InteractionTextWidget;

	class APlayerCharacter* TargetPlayer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Category, Meta=(AllowPrivateAccess=true))
	ENPCCategory NpcCategory;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
	FRotator InitRotate;
public:
	UFUNCTION()
	virtual void ActZoneOnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void ActZoneOnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	TArray<FName> GetSellingItems() { return SellingItems; }

	ENPCCategory GetCategory() { return NpcCategory; }
};
