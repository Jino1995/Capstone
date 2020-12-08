// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterStatComponent.h"
#include "ABGameInstance.h"
#include "PlayerCharacter.h"
#include "ABSaveGame.h"

// Sets default values for this component's properties
UCharacterStatComponent::UCharacterStatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	bWantsInitializeComponent = true;
	
	Level = 1;
	PureStat.MaxHealth = MaxHealth = 100.f;
	SetMaxHp(100.0f);
	SetCurrentHp(MaxHealth);
	PureStat.MaxStamina = MaxStamina = 100.f;
	SetMaxSP(100.0f);
	SetCurrentSP(MaxStamina);
	Hp = 0;
	Sp = 0;
	Atk = 0;
	Attack = 0;
	Defense = 0;
	StaminaRate = 20.0f;
	ConsumeStaminaRate = 50.0f;
	DefPercent = 0;
}


// Called when the game starts
void UCharacterStatComponent::BeginPlay()
{
	Super::BeginPlay();
	RenewalStat();
}

// Called every frame
void UCharacterStatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	/** 스테미나 자연 회복률 / 손실률 */
	float DeltaStamina = StaminaRate * DeltaTime;
	float DeltaStamina2 = ConsumeStaminaRate * DeltaTime;

	if (Cast<APlayerCharacter>(GetOwner())->CheckStopRecoveryStamina())
	{
		
	}

	/** 스테미나 사용중인 경우(캐릭터가 달리고 있는 경우) */
	else if (Cast<APlayerCharacter>(GetOwner())->CheckPlayConsumeStamina())
	{
		if (CurrentStamina - DeltaStamina2 <= 0.0f)
		{
			SetCurrentSPToZero();
		}
		else
		{
			DecreaseCurrentStamina(DeltaStamina2);
		}
	}
	/** 스테미나를 사용중이지 않을 경우 */
	else
	{
		/** 현재 스테미나를 MAXStamina가 될 때 까지 회복 */
		if (CurrentStamina + DeltaStamina >= MaxStamina)
		{
			SetCurrentSP(MaxStamina);
		}
		else
		{
			IncreaseCurrentStamina(DeltaStamina * 2);
		}
	}
}



void UCharacterStatComponent::InitializeComponent()
{
	Super::InitializeComponent();
}

void UCharacterStatComponent::SetNewLevel(int32 NewLevel)
{
	auto ABGameInstance = Cast<UABGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	CurrentStatData = ABGameInstance->GetCharacterData(NewLevel);
	if (CurrentStatData != nullptr)
	{
		SetPureStat(CurrentStatData);
		Level = NewLevel;
		SetMaxHp(CurrentStatData->GetMaxHP());
		SetCurrentHp(MaxHealth);
		SetMaxSP(CurrentStatData->GetMaxSP());
		SetCurrentSP(MaxStamina);
		OnLevelChanged.Broadcast();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Stat Load Error"));
	}
}

void UCharacterStatComponent::SetMaxHp(float NewHp)
{
	MaxHealth = NewHp;
	OnMaxHPChanged.Broadcast();
}

void UCharacterStatComponent::SetMaxSP(float NewSp)
{
	MaxStamina = NewSp;
	OnMaxSPChanged.Broadcast();
}

void UCharacterStatComponent::SetCurrentHp(float NewHp)
{
	CurrentHealth = NewHp;
	OnCurrentHPChanged.Broadcast();
}

void UCharacterStatComponent::SetCurrentSP(float NewSp)
{
	CurrentStamina = NewSp;
	OnCurrentSPChanged.Broadcast();
}

void UCharacterStatComponent::SetCurrentSPToZero()
{
	CurrentStamina = 0;
	OnZeroStamina.Broadcast();
	OnCurrentSPChanged.Broadcast();
}

void UCharacterStatComponent::SetAttack(int32 NewAttack)
{
	Attack = NewAttack;
	OnAttackChanged.Broadcast();
}

void UCharacterStatComponent::SetDefense(int32 NewDefense)
{
	Defense = NewDefense;
	OnDefenseChanged.Broadcast();
	SetDefPercent();
}

void UCharacterStatComponent::SetHp(int NewHp) 
{
	Hp = NewHp;
	IncreaseMaxHealth(Hp*OnePointHp);
	OnHPStatChanged.Broadcast();
}

void UCharacterStatComponent::SetSp(int NewSp) 
{ 
	Sp = NewSp; 
	IncreaseMaxStamina(Sp*OnePointSp);
	OnSPStatChanged.Broadcast();
}

void UCharacterStatComponent::SetAtk(int NewATK)
{
	Atk = NewATK; 
	IncreaseAttack(Attack+ (Atk*OnePointAtk));
	OnATKStatChanged.Broadcast();
}

void UCharacterStatComponent::SetDefPercent()
{
	DefPercent = Defense / 10;
	OnDEFPercentChanged.Broadcast();
}

void UCharacterStatComponent::SetStatPoint(int NewStatPoint)
{
	StatPoint = NewStatPoint; 
	OnStatPointChanged.Broadcast();
}

void UCharacterStatComponent::IncreaseMaxHealth(float IncHealth)
{ 
	MaxHealth += IncHealth;
	OnMaxHPChanged.Broadcast();
}

void UCharacterStatComponent::IncreaseMaxStamina(float IncStamina)
{
	MaxStamina += IncStamina;
	OnMaxSPChanged.Broadcast();
}

void UCharacterStatComponent::DecreaseMaxStamina(float DecHealth)
{
	MaxStamina -= DecHealth;
	OnMaxSPChanged.Broadcast();
}

void UCharacterStatComponent::DecreaseMaxHealth(float DecStamina)
{
	MaxHealth -= DecStamina;
	OnMaxHPChanged.Broadcast();
}

void UCharacterStatComponent::IncreaseAttack(int32 IncAttack)
{
	Attack += IncAttack;
	OnAttackChanged.Broadcast();
}

void UCharacterStatComponent::IncreaseDefense(int32 IncDefense)
{
	Defense += IncDefense;
	OnDefenseChanged.Broadcast();
	SetDefPercent();
}

void UCharacterStatComponent::IncreaseHP(int32 IncHP)
{
	Hp += IncHP;
	OnHPStatChanged.Broadcast();
	IncreaseMaxHealth(OnePointHp * IncHP);
}

void UCharacterStatComponent::IncreaseSP(int32 IncSP)
{
	Sp += IncSP;
	OnSPStatChanged.Broadcast();
	IncreaseMaxStamina(OnePointSp * IncSP);
}

void UCharacterStatComponent::IncreaseATK(int32 IncATK)
{
	Atk += IncATK;
	OnATKStatChanged.Broadcast();
	IncreaseAttack(OnePointAtk * IncATK);
}

void UCharacterStatComponent::IncreaseStatPoint(int32 IncStatPoint)
{
	StatPoint += IncStatPoint;
	OnStatPointChanged.Broadcast();
}

void UCharacterStatComponent::IncreaseCurrentHealth(float IncHealth)
{
	CurrentHealth += IncHealth;
	if (CurrentHealth > MaxHealth)
	{
		CurrentHealth = MaxHealth;
	}
	OnCurrentHPChanged.Broadcast();
}

void UCharacterStatComponent::IncreaseCurrentStamina(float IncStamina)
{
	CurrentStamina += IncStamina;
	if (CurrentStamina > MaxStamina)
	{
		CurrentStamina = MaxStamina;
	}
	OnCurrentSPChanged.Broadcast();
}

void UCharacterStatComponent::DecreaseCurrentStamina(float DecStamina)
{
	CurrentStamina -= DecStamina;
	if (CurrentStamina < 0)
	{
		CurrentStamina = 0;
	}
	OnCurrentSPChanged.Broadcast();
}

float UCharacterStatComponent::DecreaseCurrentHealth(float DecHealth)
{
	DecHealth = DecHealth*((100 - DefPercent) / 100);
	DecHealth = FMath::CeilToFloat(DecHealth);

	CurrentHealth -= DecHealth;
	if (CurrentHealth <= 0)
	{
		CurrentHealth = 0;
		OnZeroHealth.Broadcast();
	}
	OnCurrentHPChanged.Broadcast();

	return DecHealth;
}

void UCharacterStatComponent::DecreaseHP(int32 DecHP)
{
	Hp -= DecHP;
	OnHPStatChanged.Broadcast();
	DecreaseMaxHealth(OnePointHp * DecHP);
}
void UCharacterStatComponent::DecreaseSP(int32 DecSP)
{
	Sp -= DecSP;
	OnSPStatChanged.Broadcast();
	DecreaseMaxStamina(OnePointSp * DecSP);
}

void UCharacterStatComponent::DecreaseATK(int32 DecATK)
{
	Atk -= DecATK;
	OnATKStatChanged.Broadcast();
	DecreaseDamage(OnePointAtk * DecATK);
}

void UCharacterStatComponent::DecreaseDEF(int32 DecDEF)
{
	Defense -= DecDEF;
	OnDefenseChanged.Broadcast();
	SetDefPercent();
}

void UCharacterStatComponent::DecreaseDamage(int32 DecDmg)
{
	Attack -= DecDmg;
	OnAttackChanged.Broadcast();
}

void UCharacterStatComponent::UseStatPoint(FString IncStat)
{
	if (StatPoint > 0)
	{
		if (IncStat == FString("HP"))
		{
			StatPoint -= 1;
			OnStatPointChanged.Broadcast();
			IncreaseHP(1);
		}
		else if (IncStat == FString("SP"))
		{
			StatPoint -= 1;
			OnStatPointChanged.Broadcast();
			IncreaseSP(1);
		}
		else if (IncStat == FString("ATK"))
		{
			StatPoint -= 1;
			OnStatPointChanged.Broadcast();
			IncreaseATK(1);
		}
	}
}

void UCharacterStatComponent::LevelUp(int32 LvUpNum)
{
	auto ABGameInstance = Cast<UABGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	Level += LvUpNum;
	float PrePureMaxHP = CurrentStatData->GetMaxHP();
	float PrePureMaxSP = CurrentStatData->GetMaxSP();
	CurrentStatData = ABGameInstance->GetCharacterData(Level);
	RenewalLevelUpCost();
	IncreaseStatPoint(5);
	IncreaseMaxHealth(CurrentStatData->GetMaxHP() - PrePureMaxHP);
	IncreaseMaxStamina(CurrentStatData->GetMaxSP() - PrePureMaxSP);

	OnLevelChanged.Broadcast();
}

void UCharacterStatComponent::RenewalLevelUpCost()
{
	LevelUpCost = CurrentStatData->GetNextLvCost();
	OnLevelUpCostChanged.Broadcast();
}

void UCharacterStatComponent::SavePlayerStats(UABSaveGame* SaveSlot)
{
	SaveSlot->Level = Level;
	SaveSlot->Hp = Hp;
	SaveSlot->Sp = Sp;
	SaveSlot->Atk = Atk;
	SaveSlot->CurrentHealth = CurrentHealth;
	SaveSlot->CurrentStamina = CurrentStamina;
	SaveSlot->StatPoint = StatPoint;
}

void UCharacterStatComponent::LoadPlayerStats(class UABSaveGame* LoadSlot)
{
	SetNewLevel(LoadSlot->Level);
	SetHp(LoadSlot->Hp);
	SetSp(LoadSlot->Sp);
	SetAtk(LoadSlot->Atk);
	SetCurrentHp(LoadSlot->CurrentHealth);
	SetCurrentSP(LoadSlot->CurrentStamina);
	SetStatPoint(LoadSlot->StatPoint);
}

void UCharacterStatComponent::SetPureStat(FCharacterData* Data)
{
	PureStat.MaxHealth = Data->GetMaxHP();
	PureStat.MaxStamina = Data->GetMaxSP();
}

void UCharacterStatComponent::RenewalStat()
{
	auto ABGameInstance = Cast<UABGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	CurrentStatData = ABGameInstance->GetCharacterData(Level);
	OnLevelChanged.Broadcast();
	OnMaxHPChanged.Broadcast();
	OnCurrentHPChanged.Broadcast();
	OnMaxSPChanged.Broadcast();
	OnCurrentSPChanged.Broadcast();
	OnLevelUpCostChanged.Broadcast();
	OnAttackChanged.Broadcast();
	OnDefenseChanged.Broadcast();
}