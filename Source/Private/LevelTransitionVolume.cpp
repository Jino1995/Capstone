// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelTransitionVolume.h"
#include "Components/BillboardComponent.h"
#include "PlayerCharacter.h"
#include "ABPlayerController.h"
#include "Kismet/KismetStringLibrary.h"

// Sets default values
ALevelTransitionVolume::ALevelTransitionVolume()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	BodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BODY"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> BODY_SM(TEXT("/Game/Portal/portal_subs_Box001.portal_subs_Box001"));
	if (BODY_SM.Succeeded())
	{
		BodyMesh->SetStaticMesh(BODY_SM.Object);
	}
	RootComponent = BodyMesh;
	
	PortalMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PORTAL"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> PORTAL_SM(TEXT("/Game/Portal/portal_subs_Plane001.portal_subs_Plane001"));
	if (PORTAL_SM.Succeeded())
	{
		PortalMesh->SetStaticMesh(PORTAL_SM.Object);
	}
	PortalMesh->SetupAttachment(RootComponent);

	TransitionVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("TransitionVolume"));
	TransitionVolume->SetupAttachment(RootComponent);

	Billboard = CreateDefaultSubobject<UBillboardComponent>(TEXT("BillBoard"));
	Billboard->SetupAttachment(RootComponent);

	TransitionLevelName = "Stage1";
}

// Called when the game starts or when spawned
void ALevelTransitionVolume::BeginPlay()
{
	Super::BeginPlay();
	
	TransitionVolume->OnComponentBeginOverlap.AddDynamic(this, &ALevelTransitionVolume::OnOverlapBegin);
}

// Called every frame
void ALevelTransitionVolume::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void ALevelTransitionVolume::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor);
		if (Player)
		{
			Cast<AABPlayerController>(Player->GetController())->NextStageSaveGame();
			
			FString CurrentStage = UGameplayStatics::GetCurrentLevelName(GetWorld());
			if (CurrentStage.Contains(FString("Stage")))
			{
				if (CurrentStage == FString("Stage6"))
				{
					TransitionLevelName = FName("FinalStage");
				}
				else
				{
					CurrentStage.RemoveAt(0, 5);

					int32 NextStageNum = UKismetStringLibrary::Conv_StringToInt(CurrentStage);
					NextStageNum++;

					FString NextStageName = FString("Stage") + FString::FromInt(NextStageNum);
					TransitionLevelName = FName(*NextStageName);
				}
			}

			Player->SwitchLevel(TransitionLevelName);
		}
	}
}

