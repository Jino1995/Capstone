// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"
#include "Components/ActorComponent.h"
#include "CharacterStatComponent.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnLevelChangedDelegate);
DECLARE_MULTICAST_DELEGATE(FOnLevelUpCostChangedDelegate);
DECLARE_MULTICAST_DELEGATE(FOnMaxHPChangedDelegate);
DECLARE_MULTICAST_DELEGATE(FOnMaxSPChangedDelegate);

DECLARE_MULTICAST_DELEGATE(FOnCurrentHPChangedDelegate);
DECLARE_MULTICAST_DELEGATE(FOnCurrentSPChangedDelegate);

DECLARE_MULTICAST_DELEGATE(FOnAttackChangedDelegate);
DECLARE_MULTICAST_DELEGATE(FOnDefenseChangedDelegate);
DECLARE_MULTICAST_DELEGATE(FOnHPStatChangedDelegate);
DECLARE_MULTICAST_DELEGATE(FOnSPStatChangedDelegate);
DECLARE_MULTICAST_DELEGATE(FOnATKStatChangedDelegate);
DECLARE_MULTICAST_DELEGATE(FOnDEFPercentChangedDelegate);
DECLARE_MULTICAST_DELEGATE(FOnStatPointChangedDelegate);
DECLARE_MULTICAST_DELEGATE(FOnMaxEXPChangedDelegate);

DECLARE_MULTICAST_DELEGATE(FOnCurrentEXPChangedDelegate);

DECLARE_MULTICAST_DELEGATE(FOnZeroHealthDelegate);
DECLARE_MULTICAST_DELEGATE(FOnZeroStaminaDelegate);

USTRUCT(BlueprintType)
struct FPureStat
{
	GENERATED_BODY()

	float MaxHealth;
	float MaxStamina;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RPG_PJT_API UCharacterStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UCharacterStatComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void InitializeComponent() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	void SetNewLevel(int32 NewLevel);

private:
	struct FCharacterData* CurrentStatData = nullptr;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Player Stats", Meta = (AllowPrivateAccess = true))
	int32 Level;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Player Stats", Meta = (AllowPrivateAccess = true))
	int32 LevelUpCost;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Stats", Meta = (AllowPrivateAccess = true))
	float MaxHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats", Meta = (AllowPrivateAccess = true))
	float CurrentHealth;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Stats", Meta = (AllowPrivateAccess = true))
	float MaxStamina;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Player Stats", Meta = (AllowPrivateAccess = true))
	float CurrentStamina;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Player Stats", Meta = (AllowPrivateAccess = true))
	int32 Attack;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Player Stats", Meta = (AllowPrivateAccess = true))
	int32 Defense;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Player Stats", Meta = (AllowPrivateAccess = true))
	int32 Hp;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Player Stats", Meta = (AllowPrivateAccess = true))
	int32 Sp;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Player Stats", Meta = (AllowPrivateAccess = true))
	int32 Atk;
	
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Player Stats", Meta = (AllowPrivateAccess = true))
	float DefPercent;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Player Stats", Meta = (AllowPrivateAccess = true))
	int32 StatPoint;

	FPureStat PureStat;

	/** 스태미나 자연 재생률 */
	float StaminaRate;
	float ConsumeStaminaRate;

	const int32 OnePointHp = 10;
	const int32 OnePointSp = 5;
	const int32 OnePointAtk = 2;

private:
	void SetPureStat(FCharacterData* Data);

public:
	FORCEINLINE int32 GetLevel() { return Level; }
	FORCEINLINE int32 GetLevelUpCost() { return LevelUpCost; }
	FORCEINLINE float GetMHealth() { return MaxHealth; }
	FORCEINLINE	float GetMStamina() { return MaxStamina; }
	FORCEINLINE float GetCHealth() { return CurrentHealth; }
	FORCEINLINE float GetCStamina() { return CurrentStamina; }
	FORCEINLINE int32 GetAttack() { return Attack; }
	FORCEINLINE int32 GetDefense() { return Defense; }
	FORCEINLINE int32 GetHpStat() { return Hp; }
	FORCEINLINE int32 GetSpStat() { return Sp; }
	FORCEINLINE int32 GetAtkStat() { return Atk; }
	FORCEINLINE int32 GetDefPercent() { return DefPercent; }
	FORCEINLINE int32 GetStatPoint() { return StatPoint; }

	void RenewalStat();

	void SetMaxHp(float NewHp);
	void SetMaxSP(float NewSp);
	void SetCurrentHp(float NewHp);
	void SetCurrentSP(float NewSp);
	void SetCurrentSPToZero();
	void SetAttack(int32 NewAttack);
	void SetDefense(int32 NewDefense);
	void SetHp(int NewHp);
	void SetSp(int NewSp);
	void SetAtk(int NewATK);
	void SetDefPercent();
	void SetStatPoint(int NewStatPoint);

	void IncreaseMaxHealth(float IncHealth);
	void DecreaseMaxHealth(float DecHealth);
	void IncreaseMaxStamina(float IncStamina);
	void DecreaseMaxStamina(float DecStamina);

	void IncreaseCurrentHealth(float IncHealth);
	float DecreaseCurrentHealth(float DecHealth);
	void IncreaseCurrentStamina(float IncStamina);
	void DecreaseCurrentStamina(float DecStamina);

	void IncreaseHP(int32 IncHP);
	void DecreaseHP(int32 DecHP);
	void IncreaseSP(int32 IncSP);
	void DecreaseSP(int32 DecSP);
	void IncreaseATK(int32 IncATK);
	void DecreaseATK(int32 DecATK);
	void IncreaseDefense(int32 IncDefense);
	void DecreaseDEF(int32 DecDEF);
	void IncreaseAttack(int32 IncAttack);
	void DecreaseDamage(int32 DecDmg);
	void IncreaseStatPoint(int32 IncStatPoint);

	void UseStatPoint(FString IncStat);
	void LevelUp(int32 LvUpNum);

	void RenewalLevelUpCost();

	void SavePlayerStats(class UABSaveGame* SaveSlot);
	void LoadPlayerStats(class UABSaveGame* LoadSlot);

public:
	FOnLevelChangedDelegate OnLevelChanged;
	FOnLevelUpCostChangedDelegate OnLevelUpCostChanged;
	FOnMaxHPChangedDelegate OnMaxHPChanged;
	FOnMaxHPChangedDelegate OnMaxSPChanged;
	FOnCurrentHPChangedDelegate OnCurrentSPChanged;
	FOnCurrentSPChangedDelegate OnCurrentHPChanged;
	FOnAttackChangedDelegate OnAttackChanged;
	FOnDefenseChangedDelegate OnDefenseChanged;
	FOnHPStatChangedDelegate OnHPStatChanged;
	FOnSPStatChangedDelegate OnSPStatChanged;
	FOnATKStatChangedDelegate OnATKStatChanged;
	FOnDEFPercentChangedDelegate OnDEFPercentChanged;
	FOnStatPointChangedDelegate OnStatPointChanged;
	FOnZeroHealthDelegate OnZeroHealth;
	FOnZeroStaminaDelegate OnZeroStamina;
};
