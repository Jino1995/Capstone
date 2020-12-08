// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"
#include "GameFramework/Character.h"
#include "ABPlayerController.h"
#include "ItemPot.h"
#include "CharacterStatComponent.h"
#include "PlayerCharacter.generated.h"

/**
* UPROPERTY()는 객체가 아닌 경우 블루프린트에서 조작 할 변수 또는 에디터에 나타낼 경우에만 삽입(객체는 필수 삽입 객체의 경우 선언을 해줘야 GC에 의해 해제되지 않음)
* UFUNCTION()의 경우에도 블루프린트로 조작할 함수에다만 삽입, 또는 Delegate 관련 함수에다만 선언을 한다. 
*/

UENUM(BlueprintType)
enum class ECombatMode : uint8
{
	E_Normal_Mode,
	E_Equip_Mode
};

UCLASS()
class RPG_PJT_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerCharacter();
	
	void SwitchLevel(FName LevelName);

	UPROPERTY(VisibleAnywhere, Category = UI)
	class UWidgetComponent* DamageText;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** 캐릭터가 사용하는 AnimationInstance */
	UPROPERTY()
	class UABAnimInstance* ABAnim;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/** 컴포넌트 초기화 이후 - 액터의 컴포넌트 초기화 완료 후 호출 */
	virtual void PostInitializeComponents() override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private: 
	//////////////////////////////////////////////////////////////////////////
	// 클래스, 컴포넌트 객체.
	//////////////////////////////////////////////////////////////////////////

	/** 카메라 지지대 */
	UPROPERTY(VisibleAnywhere, Category = "Camera")
	USpringArmComponent* SpringArm;

	/** 카메라 */
	UPROPERTY(VisibleAnywhere, Category = "Camera")
	UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Parts", Meta = (AllowPrivateAccess = true))
	USkeletalMeshComponent* HairPart;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Parts", Meta = (AllowPrivateAccess = true))
	USkeletalMeshComponent* HeadPart;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Parts", Meta = (AllowPrivateAccess = true))
	USkeletalMeshComponent* TopPart;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Parts", Meta = (AllowPrivateAccess = true))
	USkeletalMeshComponent* BottomPart;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Parts", Meta = (AllowPrivateAccess = true))
	USkeletalMeshComponent* TopClothesPart;

	/** CurrentWeapon을 블루프린트에서 편집하기 위한 TSubclass */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", Meta = (AllowPrivateAccess = true))
	TSubclassOf<class AWeapon> Weapon_BP;

	/** 캐릭터가 현재 사용하고있는 무기 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", Meta = (AllowPrivateAccess = true))
	class AWeapon* CurrentWeapon;

	/** CurrentWeapon을 블루프린트에서 편집하기 위한 TSubclass */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", Meta = (AllowPrivateAccess = true))
	TSubclassOf<class AShield> Shield_BP;

	/** 캐릭터가 현재 사용하고있는 무기 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", Meta = (AllowPrivateAccess = true))
	class AShield* CurrentShield;

	/** 코드에서 캐릭터의 컨트롤러에 접근하기 위한 Controller 객체 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Controller", Meta=(AllowPrivateAccess=true))
	class AABPlayerController* MainPlayerController;

	/** Targeting 1차 감지 범위 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Targeting", Meta=(AllowPrivateAccess=true))
	class USphereComponent* TargetSphere;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Potion", Meta = (AllowPrivateAccess = true))
	TSubclassOf<class APotion> Potion_BP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Potion", Meta = (AllowPrivateAccess = true))
	APotion* Potion;


private:
	/** 캐릭터의 / 걷기 / 평상시 / 뛸때 / 속도 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats", Meta=(AllowPrivateAccess=true))
	float MAXWalkSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats", Meta = (AllowPrivateAccess = true))
	float MAXNormalSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats", Meta = (AllowPrivateAccess = true))
	float MAXSprintSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player Stats", Meta = (AllowPrivateAccess = true))
	class UCharacterStatComponent* CharacterStat;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina", Meta = (AllowPrivateAccess = true))
	float JumpAndRollingStamina;

private:
	//////////////////////////////////////////////////////////////////////////
	// 캐릭터 상태 변수.
	//////////////////////////////////////////////////////////////////////////
	/** 
	* 캐릭터의 전투 모드 여부 
	* ECombatMode::E_Normal_Mode (비전투 모드)
	* ECombatMode::E_Equip_Mode (전투 모드)
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pawn", Meta = (AllowPrivateAccess = true))
	ECombatMode CurrentCombatMode = ECombatMode::E_Normal_Mode;

	/** 캐릭터가 현재 공격중 인지 */
	bool bAttacking;
	bool bSkillAttacking;

	/** 캐릭터가 다음 공격을 할 수 있는지 */
	bool bNextAttack;

	bool bBlocking;

	bool bJumpping;

	bool bDamaged;

	bool bBlockingHit;

	/** 
	* 다음 콤보 공격을 입력을 했는지 
	* 공격이 끝나기 전에 공격 클릭 시 true로 전환 
	*/
	bool bComboInputOn;

	/** 캐릭터의 현재 콤보 공격 단계 */
	int32 CurrentCombo;

	/** 캐릭터의 최대 콤보 공격 단계 */
	int32 MaxCombo;

	/** 캐릭터가 Roll Animation을 실행 중인지 */
	bool bRolling;

	/** 캐릭터가 Equip Animation을 실행 중인지 */
	bool bEquiping;

	/** 캐릭터가 죽었는지 */
	bool bDead;

	/** 캐릭터가 움직이고 있는지 */
	bool bMoveingForward;
	bool bMoveingRight;

	bool bTargeting;

	bool bInterpToEnemy;

	bool bSavePose;

	bool bDrinking;

private:
	//////////////////////////////////////////////////////////////////////////
	// 키보드 입력 여부 boolean 변수.
	//////////////////////////////////////////////////////////////////////////

	/** 좌측 Shift 키 입력 여부 */
	UPROPERTY(EditAnywhere, Category="Pressed", Meta=(AllowPrivateAccess=true))
	bool bPressedLShift;

	/** 좌측 Ctrl 키 입력 여부 */
	UPROPERTY(EditAnywhere, Category = "Pressed", Meta = (AllowPrivateAccess = true))
	bool bPressedLCtrl;

	/** ESC 키 입력 여부 */
	UPROPERTY(EditAnywhere, Category = "Pressed", Meta = (AllowPrivateAccess = true))
	bool bESCDown;

	UPROPERTY(EditAnywhere, Category = "Pressed", Meta = (AllowPrivateAccess = true))
	bool bPressedRMouse;

	UPROPERTY(EditAnywhere, Category = "Pressed", Meta = (AllowPrivateAccess = true))
	bool bPressedInteractionKey;

private:
	// 기타 인스턴스
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat", Meta = (AllowPrivateAccess = true))
	FVector CombatTargetLocation;

	float InterpSpeed;

	TArray<AItemPot*> ItPots;

	UPROPERTY(VisibleAnywhere, Meta = (AllowPrivateAccess = true))
	class AItemPot* TargetItemPot;

	TArray<class AEnemy*> TargetingEnemys;

	UPROPERTY()
	AEnemy* LockOnTarget;

	TSet<AEnemy*> DamagedEnemy;
	
	UPROPERTY()
	AActor* InteractionTarget;

	class ANpc* TargetNpc;

	float PotionRecovery;

public:
	/** 
	* 피격 이팩트 
	* 블루프린트로 지정
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", Meta = (AllowPrivateAccess = true))
	class UParticleSystem* HitParticles;

	/** 
	* 피격 소리 
	* 블루프린트로 지정
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	class USoundCue* HitSound;

	/**
	* 코인 흭득 소리
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Coin")
	USoundCue* CoinSound;

public:
	//////////////////////////////////////////////////////////////////////////
	// 키에 바인드 되어 호출되는 메서드들.
	// 주로 SetPlayerInputComponent 에서 호출되는 메서드들
	//////////////////////////////////////////////////////////////////////////

	/** 
	* 플레이어의 시점 세팅 
	* SpringArm, Camera 컴포넌트를 조정하여 변경
	* 시점 변경 시 메서드 내부를 자체적으로 조정 필요
	*/
	void SetViewPoint();

	/** 위 아래 키 입력시 호출 */
	void UpDown(float NewAxisValue);

	/** 왼쪽 오른쪽 키 입력시 호출 */
	void LeftRight(float NewAxisValue);

	/** 마우스를 위아래로 움직일 때 호출 */
	void LookUp(float NewAxisValue);

	/** 마우스를 좌우로 움직일 때 호출 */
	void Turn(float NewAxisValue);
	
	/** 
	* 공격키 입력 시 호출 
	* 캐릭터가 비전투 모드일 경우 공격이 나가지 않고 전투 모드로 전환을 시켜준다.
	*/
	void Attack();

	void BlockingStart();
	void BlockingEnd();

	/**
	* SpaceBar 입력시 호출
	* 비전투 모드에서 호출시 jump animation 호출
	* 전투 모드에서 호출시 roll animation 호출
	*/
	void JumpAndRoll();
	
	/** 
	* 전투모드 비전투 모드 전환 함수
	* 좌측 Tab 키 입력 시 호출
	*/
	void Equip();

	/**
	* 좌측 Ctrl 입력에 따른 호출
	* 캐릭터의 걷기 활성화/비활성화
	*/
	void PressedLCtrl();
	void ReleasedLCtrl();

	void PressedInteractionKey();
	void ReleasedInteractionKey();

	/**
	* 좌측 Shift 입력에 따른 호출
	* 캐릭터의 뛰기 활성화/비활성화
	* 강공격/약공격 변환
	*/
	void PressedLShift();
	void ReleasedLShift();

	void PressedRMouse();
	void ReleasedRMouse();

public:
	//////////////////////////////////////////////////////////////////////////
	// 기타 메서드들
	//////////////////////////////////////////////////////////////////////////
	
	/** 
	* 공격이 끝날 경우 호출 
	* 공격 후 다음 콤보 공격을 하지 않거나, 이미 최대 콤보치에 도달하여 다음 공격을 할 수 없을 경우 호출
	*/
	UFUNCTION()
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	/** 
	* 공격 시작시 호출 
	* 꼭 콤보 공격의 시작 뿐만이 아니라 매 공격 시작마다 호출이 된다.
	*/
	void AttackStartComboState();

	/** 
	* 공격이 끝난 후 호출 
	* OnAttackMontageEnded 내부에서 호출이 된다.
	*/
	void AttackEndComboState();

	/******새로 구성 해야할 메서드********/
	/** 캐릭터가 데미지를 받을 경우(상대방의 메서드 호출에 의해) 호출이 된다 */
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	/** 체력 감소 */
	void Damaged(float Amount);

	/** 체력이 0으로 됨으로 인해 호출 */
	void Die();
	/***********************************/

	/** 
	* 캐릭터의 이동속도를 변경하기 위한 메서드
	* float NewSpeed = 새롭게 설정할 속도 값.
	*/
	void SetCharacterSpeed(float NewSpeed);

	/** 
	* 캐릭터의 방향을 지정하여 해당 방향으로 회전 시킨다. 
	* 주로 Attack/Roll animation 시 같이 입력된 방향 값으로 바로 animation이 실행이 되도록 하기위해 호출이 된다.
	*/
	bool SetCharacterRotationToAxis();

	/** 
	* 캐릭터의 이동 가능 여부 
	* 주로 UpDown/LeftRight 메서드에 의해 호출이 된다.
	* float Value = 위 메서드들의 NewAxisValue 값
	*/
	bool CanMove(float Value);

	void ChangeWeaponPosition(bool reverse = false);

	void SetInterpToEnemy(bool Interp);

	FRotator GetLookAtRotationYaw(FVector Target);

public:
	//////////////////////////////////////////////////////////////////////////
	// Getter, Setter Inline 메서드들
	//////////////////////////////////////////////////////////////////////////

	FORCEINLINE bool IsEquipping() { return bEquiping; }
	FORCEINLINE bool IsDead() { return bDead; }
	FORCEINLINE bool IsMoveing() { return bMoveingForward || bMoveingRight; }
	FORCEINLINE bool IsBlocking() { return bBlocking; }
	FORCEINLINE ECombatMode GetCombatMode() { return CurrentCombatMode; }
	FORCEINLINE AShield* GetCurrentShield() { return CurrentShield; }
	FORCEINLINE UABAnimInstance* GetAnimInstance() { return ABAnim; }
	FORCEINLINE bool IsPressedInteractionKey() { return bPressedInteractionKey; }
	FORCEINLINE AItemPot* GetTargetItemPot() { return TargetItemPot; }
	FORCEINLINE void SetTargetItemPot(AItemPot* NewTarget) {
		if(NewTarget == nullptr)
		{
			MainPlayerController->CloseLootWindow();
		}
		TargetItemPot = NewTarget; 
	}
	FORCEINLINE AEnemy* GetLockOnTarget() { return LockOnTarget; }
	FORCEINLINE bool IsTargeting() { return bTargeting; }
	FORCEINLINE void SetInteractionTarget(AActor* ITarget) { InteractionTarget = ITarget; }
	FORCEINLINE UCharacterStatComponent* GetCharacterStat() { return CharacterStat; }
	FORCEINLINE bool IsSkillAttacking() { return bSkillAttacking; }

public:
	void OpenInventory();
	void OpenMenu();

	void EquipHead(USkeletalMesh* HeadItem);
	void UnEquipHead();

	void EquipTop(USkeletalMesh* TopItem);
	void UnEquipTop();

	void EquipBot(USkeletalMesh* TopItem);
	void UnEquipBot();

	void EquipOneHandedWeapon(TSubclassOf<AWeapon> NewWeapon);
	void UnEquipOneHandedWeapon();

	void EquipTwoHandedWeapon(TSubclassOf<AWeapon> NewWeapon);
	void UnEquipTwoHandedWeapon();

	void EquipShield(TSubclassOf<AShield> NewShield);
	void UnEquipShield();

	void ShowMouseCursor();

	void AddItemPot(AItemPot* NweItemPot);
	void RemoveItemPot(AItemPot* RemovedItemPot);

	void AddTarget();

	UFUNCTION()
	virtual void TargetSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	virtual void TargetSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	void TargetingLockOn();

	FORCEINLINE void RemoveEnemyToTarget(AEnemy* Target) { TargetingEnemys.Remove(Target); }
	
	bool CheckStopRecoveryStamina();
	bool CheckPlayConsumeStamina();

	void SetTargetNpc(ANpc* Target);

	void GainMoney(int32 GMoney);

	void DrinkingPotion();

};
