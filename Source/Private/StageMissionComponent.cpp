// Fill out your copyright notice in the Description page of Project Settings.


#include "StageMissionComponent.h"
#include "ABGameInstance.h"

// Sets default values for this component's properties
UStageMissionComponent::UStageMissionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UStageMissionComponent::BeginPlay()
{
	Super::BeginPlay();

	auto ABGameInstance = Cast<UABGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));

	
}


// Called every frame
void UStageMissionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

