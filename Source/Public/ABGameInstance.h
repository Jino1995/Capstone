// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"
#include "Engine/GameInstance.h"
#include "Engine/DataTable.h"
#include "ABGameInstance.generated.h"

UENUM(BlueprintType)
enum class EItemQuality : uint8
{
	Trash,
	Common,
	Magic,
	Rare,
	Unique,
	legeond
};

UENUM(BlueprintType)
enum class EItemType : uint8
{
	None,
	Consumable,
	Equipment
};

UENUM(BlueprintType)
enum class EEquipmentType : uint8
{
	None,
	Armor,
	Weapon,
	Shield
};

UENUM(BlueprintType)
enum class EEquipmentSlot : uint8
{
	None,
	Head,
	Chest,
	Legs,
	RightHand,
	LeftHand,
	BothHands
};

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	None,
	OneHanded,
	TwoHanded
};

USTRUCT(BlueprintType)
struct FItemData : public FTableRowBase
{
	GENERATED_BODY()

public:
	FItemData() : Name(FName("Default")), Icon(nullptr), Quality(EItemQuality::Common), ItemType(EItemType::None), Amount(0), IsStackable(false), MaxStackSize(0), IsDroppable(false), EquipMesh(nullptr), Weapon_BP(nullptr), Shield_BP(nullptr), Heal(0), EquipmentType(EEquipmentType::None), EquipmentSlot(EEquipmentSlot::None), WeaponType(EWeaponType::None), WeaponDetail(FString("None")), Damage(0), Armor(0), Use(FString("None")), Description(FString("None")), HP(0), SP(0), Def(0), Atk(0), Purchase(0), Sale(0){}

	FName GetName() { return Name; }

	UTexture2D* GetIcon() { return Icon; }

	EItemQuality GetQuality() { return Quality; }

	EItemType GetItemType() { return ItemType; }

	int32 GetAmount() { return Amount; }

	bool GetIsStackable() { return IsStackable; }

	int32 GetMaxStackSize() { return MaxStackSize; }

	bool GetIsDroppable() { return IsDroppable; }

	USkeletalMesh* GetEquipMesh() { return EquipMesh; }

	TSubclassOf<class AWeapon> GetWeaponBP() { return Weapon_BP; }

	TSubclassOf<class AShield> GetShieldBP() { return Shield_BP; }

	int32 GetHeal() { return Heal; }

	EEquipmentType GetEquipmentType() { return EquipmentType; }

	EEquipmentSlot GetEquipmentSlot() { return EquipmentSlot; }

	EWeaponType GetWeaponType() { return WeaponType; }

	FString GetWeaponDetail() { return WeaponDetail; }

	int32 GetDamage() { return Damage; }

	int32 GetArmor() { return Armor; }

	FString GetUse() { return Use; }

	FString GetDescription() { return Description; }

	void SetAmount(int32 NewAmount) { Amount = NewAmount; }

	int32 GetHP() { return HP; }
	int32 GetSP() { return SP; }
	int32 GetDef() { return Def; }
	int32 GetAtk() { return Atk; }

	int32 GetPurchase() { return Purchase; }
	int32 GetSale() { return Sale; }

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Data, Meta = (AllowPrivateAccess = true))
	FName Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Data, Meta = (AllowPrivateAccess = true))
	UTexture2D* Icon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Data, Meta = (AllowPrivateAccess = true))
	EItemQuality Quality;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Data, Meta = (AllowPrivateAccess = true))
	EItemType ItemType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Data, Meta = (AllowPrivateAccess = true))
	int32 Amount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Data, Meta = (AllowPrivateAccess = true))
	bool IsStackable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Data, Meta = (AllowPrivateAccess = true))
	int32 MaxStackSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Data, Meta = (AllowPrivateAccess = true))
	bool IsDroppable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Data, Meta = (AllowPrivateAccess = true))
	USkeletalMesh* EquipMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Data, Meta = (AllowPrivateAccess = true))
	TSubclassOf<class AWeapon> Weapon_BP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Data, Meta = (AllowPrivateAccess = true))
	TSubclassOf<class AShield> Shield_BP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Data, Meta = (AllowPrivateAccess = true))
	int32 Heal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Data, Meta = (AllowPrivateAccess = true))
	EEquipmentType EquipmentType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Data, Meta = (AllowPrivateAccess = true))
	EEquipmentSlot EquipmentSlot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Data, Meta = (AllowPrivateAccess = true))
	EWeaponType WeaponType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Data, Meta = (AllowPrivateAccess = true))
	FString WeaponDetail;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Data, Meta = (AllowPrivateAccess = true))
	int32 Damage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Data, Meta = (AllowPrivateAccess = true))
	int32 Armor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Data, Meta = (AllowPrivateAccess = true))
	FString Use;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Data, Meta = (AllowPrivateAccess = true))
	FString Description;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Data, Meta = (AllowPrivateAccess = true))
	int32 HP;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Data, Meta = (AllowPrivateAccess = true))
	int32 SP;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Data, Meta = (AllowPrivateAccess = true))
	int32 Def;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Data, Meta = (AllowPrivateAccess = true))
	int32 Atk;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Data, Meta = (AllowPrivateAccess = true))
	int32 Purchase;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Data, Meta = (AllowPrivateAccess = true))
	int32 Sale;
};

USTRUCT(BlueprintType)
struct FCharacterData : public FTableRowBase
{
	GENERATED_BODY()

public:
	FCharacterData() : Level(1), MaxHP(100.f), MaxSP(100.f), NextLvCost(0) {}

	int32 GetLevel() { return Level; }
	float GetMaxHP() { return MaxHP; }
	float GetMaxSP() { return MaxSP; }
	int32 GetNextLvCost() { return NextLvCost; }

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data", Meta=(AllowPrivateAccess=true))
	int32 Level;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data", Meta = (AllowPrivateAccess = true))
	float MaxHP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data", Meta = (AllowPrivateAccess = true))
	float MaxSP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data", Meta = (AllowPrivateAccess = true))
	int32 NextLvCost;
};

USTRUCT(BlueprintType)
struct FStageInfo : public FTableRowBase
{
	GENERATED_BODY()

public:
	FStageInfo() : LevelName(FString("Stage1")), MonsterNum(0), BossNum(0) {}

	FString GetLevelName() { return LevelName; }
	int32 GetMonsterNum() { return MonsterNum; }
	int32 GetBossNum() { return BossNum; }

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data", Meta = (AllowPrivateAccess = true))
	FString LevelName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data", Meta = (AllowPrivateAccess = true))
	int32 MonsterNum;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data", Meta = (AllowPrivateAccess = true))
	int32 BossNum;

};

/**
 * 
 */
UCLASS()
class RPG_PJT_API UABGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	UABGameInstance();

	virtual void Init() override;

	FItemData* GetItemData(FName ItemName);

	FCharacterData* GetCharacterData(int32 Level);

	FStageInfo* GetStageInfo(FString StageName);

private:

	UPROPERTY()
	class UDataTable* ItemTable;

	UPROPERTY()
	class UDataTable* CharacterStatTable;

	UPROPERTY()
	class UDataTable* StageInfoTable;
};
