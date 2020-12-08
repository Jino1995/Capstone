// Fill out your copyright notice in the Description page of Project Settings.


#include "SaveActor.h"
#include "PlayerCharacter.h"
#include "ABPlayerController.h"
#include "Components/WidgetComponent.h"
#include "Engine.h"

// Sets default values
ASaveActor::ASaveActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MESH"));
	actZone = CreateDefaultSubobject<USphereComponent>(TEXT("INTERACTION"));

	SetRootComponent(Mesh);
	actZone->SetupAttachment(Mesh);
	actZone->InitSphereRadius(800.0f);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> Mesh_SM(TEXT("/Game/InfinityBladeGrassLands/Environments/Plains/Env_Plains_Statues/StaticMesh/SM_Plains_Angel_Statue_01.SM_Plains_Angel_Statue_01"));
	if (Mesh_SM.Succeeded())
	{
		Mesh->SetStaticMesh(Mesh_SM.Object);
	}

	InteractionTextWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("INTERACTIONTEXT"));
	InteractionTextWidget->SetupAttachment(Mesh);
	InteractionTextWidget->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	InteractionTextWidget->SetWidgetSpace(EWidgetSpace::Screen);

	static ConstructorHelpers::FClassFinder<UUserWidget> IntAct_Widget(TEXT("/Game/HUD/InteractionText_BP.InteractionText_BP_C"));
	if (IntAct_Widget.Succeeded())
	{
		InteractionTextWidget->SetWidgetClass(IntAct_Widget.Class);
		InteractionTextWidget->SetDrawSize(FVector2D(250.0f, 50.0f));
		InteractionTextWidget->SetVisibility(false);
	}
}

// Called when the game starts or when spawned
void ASaveActor::BeginPlay()
{
	Super::BeginPlay();
	
	actZone->OnComponentBeginOverlap.AddDynamic(this, &ASaveActor::ActZoneOnOverlapBegin);
	actZone->OnComponentEndOverlap.AddDynamic(this, &ASaveActor::ActZoneOnOverlapEnd);
	actZone->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Ignore);
	actZone->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

}

// Called every frame
void ASaveActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASaveActor::ActZoneOnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor);
		if (Player)
		{
			InteractionTextWidget->SetVisibility(true);
			Player->SetInteractionTarget(this);
		}
	}
}

void ASaveActor::ActZoneOnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor);
		if (Player)
		{
			InteractionTextWidget->SetVisibility(false);
			Player->SetInteractionTarget(nullptr);
		}
	}
}

