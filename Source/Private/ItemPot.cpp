// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemPot.h"
#include "PlayerCharacter.h"
#include "ABGameInstance.h"
#include "Components/WidgetComponent.h"

// Sets default values
AItemPot::AItemPot()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MESH"));
	LootSphere = CreateDefaultSubobject<USphereComponent>(TEXT("LOOTSPHERE"));

	SetRootComponent(Mesh);
	LootSphere->SetupAttachment(Mesh);
	LootSphere->InitSphereRadius(150.0f);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_Mesh(TEXT("/Game/InventorySystem/Environment/Meshes/SM_pot.SM_Pot"));
	if (SM_Mesh.Succeeded())
	{
		Mesh->SetStaticMesh(SM_Mesh.Object);
	}

	Mesh->SetRelativeScale3D(FVector(0.5f, .5f, .5f));
	Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
		
	//
	LootSphere->SetHiddenInGame(false);
	//

	TargetPlayer = nullptr;

	InteractionTextWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("INTERACTIONTEXT"));
	InteractionTextWidget->SetupAttachment(Mesh);
	InteractionTextWidget->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	InteractionTextWidget->SetWidgetSpace(EWidgetSpace::Screen);

	static ConstructorHelpers::FClassFinder<UUserWidget> IntAct_Widget(TEXT("/Game/HUD/PickupText_BP.PickupText_BP_C"));
	if (IntAct_Widget.Succeeded())
	{
		InteractionTextWidget->SetWidgetClass(IntAct_Widget.Class);
		InteractionTextWidget->SetDrawSize(FVector2D(145.0f, 40.0f));
		InteractionTextWidget->SetVisibility(false);
		InteractionTextWidget->SetRelativeLocation(FVector(0.f, -10.f, 50.f));
	}
}

// Called when the game starts or when spawned
void AItemPot::BeginPlay()
{
	Super::BeginPlay();

	LootSphere->OnComponentBeginOverlap.AddDynamic(this, &AItemPot::LootSphereOnOverlapBegin);
	LootSphere->OnComponentEndOverlap.AddDynamic(this, &AItemPot::LootSphereOnOverlapEnd);
	LootSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	LootSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
}

// Called every frame
void AItemPot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AItemPot::LootSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		APlayerCharacter* Character = Cast<APlayerCharacter>(OtherActor);
		if (Character)
		{
			TargetPlayer = Character;
			Character->AddItemPot(this);
			InteractionTextWidget->SetVisibility(true);
		}
	}
}

void AItemPot::LootSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		APlayerCharacter* Character = Cast<APlayerCharacter>(OtherActor);
		if (Character)
		{
			Character->RemoveItemPot(this);
			TargetPlayer = nullptr;
			InteractionTextWidget->SetVisibility(false);
		}
	}
}

void AItemPot::AddItem(FName ItemName, int32 ItemAmount)
{
	UABGameInstance* ABGameInstance = Cast<UABGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	int32 MaxStack = ABGameInstance->GetItemData(ItemName)->GetMaxStackSize();
	int32 RemainStack = ItemAmount;

	for (auto& Item : ItemPotInventory)
	{
		if ((Item.Key == ItemName) && (Item.Value <= MaxStack))
		{
			if ((Item.Value + ItemAmount) <= MaxStack)
			{
				Item.Value = Item.Value + ItemAmount;
				return;
			}
			else
			{
				Item.Value = MaxStack;
				ItemAmount = ItemAmount - MaxStack;
			}
		}
	}
	ItemPotInventory.Add(TPair<FName, int32>(ItemName, ItemAmount));
}

void AItemPot::RemoveItem(int32 Index)
{
	ItemPotInventory.RemoveAt(Index);
	if (ItemPotInventory.Num() == 0)
	{	
		Destroy();
	}
}