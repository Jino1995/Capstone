// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemPot.generated.h"

UCLASS()
class RPG_PJT_API AItemPot : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItemPot();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
	UStaticMeshComponent* Mesh;

	UPROPERTY()
	class APlayerCharacter* TargetPlayer;

	TArray<TTuple<FName, int32>> ItemPotInventory;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = UI, Meta = (AllowPrivateAccess = true))
	class UWidgetComponent* InteractionTextWidget;

	UPROPERTY()
	USphereComponent* LootSphere;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void LootSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void LootSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void AddItem(FName ItemName, int32 ItemAmount);
	void RemoveItem(int32 Index);

	FORCEINLINE TArray<TTuple<FName, int32>>& GetItemPotInventory() { return ItemPotInventory; }

	FTimerHandle DeathTimer;
};
