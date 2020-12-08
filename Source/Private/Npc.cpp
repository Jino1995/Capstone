// Fill out your copyright notice in the Description page of Project Settings.


#include "Npc.h"
#include "PlayerCharacter.h"
#include "Components/WidgetComponent.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ANpc::ANpc()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	InteractionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("INTERACTION"));
	InteractionSphere->SetupAttachment(RootComponent);
	InteractionSphere->InitSphereRadius(800.0f);

	NameTextWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("NAMETEXT"));
	NameTextWidget->SetupAttachment(GetMesh());
	NameTextWidget->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	NameTextWidget->SetWidgetSpace(EWidgetSpace::Screen);
	static ConstructorHelpers::FClassFinder<UUserWidget> NameTxt_Widget(TEXT("/Game/Npc/EquipmentTrader/NPCNameTEXT_BP.NPCNameTEXT_BP_C"));
	if (NameTxt_Widget.Succeeded())
	{
		NameTextWidget->SetWidgetClass(NameTxt_Widget.Class);
		NameTextWidget->SetDrawSize(FVector2D(145.0f, 40.0f));
		NameTextWidget->SetVisibility(true);
		NameTextWidget->SetRelativeLocation(FVector(0.f, -10.f, 50.f));
	}

	InteractionTextWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("INTERACTIONTEXT"));
	InteractionTextWidget->SetupAttachment(GetMesh());
	InteractionTextWidget->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	InteractionTextWidget->SetWidgetSpace(EWidgetSpace::Screen);
	static ConstructorHelpers::FClassFinder<UUserWidget> IntAct_Widget(TEXT("/Game/HUD/InteractionText_BP.InteractionText_BP_C"));
	if (IntAct_Widget.Succeeded())
	{
		InteractionTextWidget->SetWidgetClass(IntAct_Widget.Class);
		InteractionTextWidget->SetDrawSize(FVector2D(250.0f, 50.0f));
		InteractionTextWidget->SetVisibility(false);
	}

	TargetPlayer = nullptr;
	InitRotate = GetActorRotation();
}

// Called when the game starts or when spawned
void ANpc::BeginPlay()
{
	Super::BeginPlay();
	
	InteractionSphere->OnComponentBeginOverlap.AddDynamic(this, &ANpc::ActZoneOnOverlapBegin);
	InteractionSphere->OnComponentEndOverlap.AddDynamic(this, &ANpc::ActZoneOnOverlapEnd);
	InteractionSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Ignore);
	InteractionSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
}

// Called every frame
void ANpc::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (TargetPlayer != nullptr)
	{
		FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), TargetPlayer->GetActorLocation());
		FRotator LookAtRotationYaw(0.f, LookAtRotation.Yaw, 0.f);

		FRotator InterpRotation = FMath::RInterpTo(GetActorRotation(), LookAtRotationYaw, DeltaTime, 5.0f);

		SetActorRotation(InterpRotation);
	}
	else
	{
		FRotator InterpRotation = FMath::RInterpTo(GetActorRotation(), InitRotate, DeltaTime, 5.0f);

		SetActorRotation(InterpRotation);
	}
}

// Called to bind functionality to input
void ANpc::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ANpc::ActZoneOnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor);
		if (Player)
		{
			Player->SetTargetNpc(this);
			TargetPlayer = Player;
			InteractionTextWidget->SetVisibility(true);
		}
	}
}

void ANpc::ActZoneOnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor);
		if (Player)
		{
			Player->SetTargetNpc(nullptr);
			TargetPlayer = nullptr;
			InteractionTextWidget->SetVisibility(false);
		}
	}
}

