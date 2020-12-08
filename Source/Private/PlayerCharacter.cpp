// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "ABAnimInstance.h"
#include "Kismet/KismetMathLibrary.h"
#include "Weapon.h"
#include "Shield.h"
#include "Sound/SoundCue.h"
#include "Enemy.h"
#include "ABPlayerController.h"
#include "Engine.h"
#include "ABGameInstance.h"
#include "ItemPot.h"
#include "Set.h"
#include "SaveActor.h"
#include "CharacterStatComponent.h"
#include "DamageText.h"
#include "Npc.h"
#include "WidgetComponent.h"
#include "Potion.h"
//#include "ABGameMode.h"

// Sets default values
APlayerCharacter::APlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	/** SpringArm */
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SPRINGARM"));
	SpringArm->SetupAttachment(GetRootComponent());

	/** Camera */
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("CAMERA"));
	Camera->SetupAttachment(SpringArm);

	/** 캐릭터 스켈레탈 메시 설정 */
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -88.0f), FRotator(0.0f, -90.0f, 0.0f));
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_Character(TEXT("/Game/Character/CompleteCharacters/Base/sk_CharM_Base.sk_CharM_Base"));
	if (SK_Character.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(SK_Character.Object);
	}

	/** 캐릭터 AnimationInstance 설정 */
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	static ConstructorHelpers::FClassFinder<UAnimInstance> ANIM_Character(TEXT("/Game/Character/CompleteCharacters/PlayerCharacterAnim_BP.PlayerCharacterAnim_BP_C"));
	if (ANIM_Character.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(ANIM_Character.Class);
	}

	HairPart = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("HAIRPARTS"));
	HeadPart = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("HEAD1"));
	TopPart = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("TOP1"));
	BottomPart = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BOTTOM1"));
	TopClothesPart = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("TOPCLOTHES1"));

	HairPart->SetupAttachment(GetMesh());
	HeadPart->SetupAttachment(GetMesh());
	TopPart->SetupAttachment(GetMesh());
	BottomPart->SetupAttachment(GetMesh());
	TopClothesPart->SetupAttachment(GetMesh());

	HairPart->SetMasterPoseComponent(GetMesh());
	HeadPart->SetMasterPoseComponent(GetMesh());
	TopPart->SetMasterPoseComponent(GetMesh());
	BottomPart->SetMasterPoseComponent(GetMesh());
	TopClothesPart->SetMasterPoseComponent(GetMesh());

	SetViewPoint();

	MaxCombo = 2;		
	
	AttackEndComboState();								  

	MAXWalkSpeed = 150.0f;								  
	MAXNormalSpeed = 500.0f;							  
	MAXSprintSpeed = 700.0f;							  
	JumpAndRollingStamina = 50.f;
					
	bSkillAttacking = false;
	bRolling = false;									  
	bEquiping = false;									  
	bBlocking = false;
	bJumpping = false;
	bDamaged = false;
	bBlockingHit = false;
	bPressedLShift = false;								  
	bESCDown = false;
	bPressedRMouse = false;
	bPressedLCtrl = false;
	bMoveingForward = false;
	bMoveingRight = false;
	bDead = false;
	bTargeting = false;
	bDrinking = false;

	/** 점프 높이 */
	GetCharacterMovement()->JumpZVelocity = 500.0f;

	GetCharacterMovement()->MaxWalkSpeed = 500.0f;

	/** 공중에서 control power 설정 */
	GetCharacterMovement()->AirControl = 0.1f;
	
	CurrentCombatMode = ECombatMode::E_Normal_Mode;
	
	
	InterpSpeed = 15.0f;
	bInterpToEnemy = false;

	TargetSphere = CreateDefaultSubobject<USphereComponent>(TEXT("TargetSphere"));
	TargetSphere->SetupAttachment(RootComponent);
	TargetSphere->InitSphereRadius(800.0f);

	CharacterStat = CreateDefaultSubobject<UCharacterStatComponent>(TEXT("CHARACTERSTAT"));

	CurrentWeapon = nullptr;
	CurrentShield = nullptr;

	DamageText = CreateDefaultSubobject<UWidgetComponent>(TEXT("DAMAGETEXT"));
	DamageText->SetupAttachment(GetMesh());
	DamageText->SetRelativeLocation(FVector(0.0f, 0.0f, 100.0f));
	DamageText->SetWidgetSpace(EWidgetSpace::Screen);

	static ConstructorHelpers::FClassFinder<UUserWidget> DamageText_Widget(TEXT("/Game/HUD/FloatText_BP.FloatText_BP_C"));
	if (DamageText_Widget.Succeeded())
	{
		DamageText->SetWidgetClass(DamageText_Widget.Class);
		DamageText->SetDrawSize(FVector2D(10.0f, 10.0f));
		DamageText->SetVisibility(true);
	}
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	TargetSphere->OnComponentBeginOverlap.AddDynamic(this, &APlayerCharacter::TargetSphereOnOverlapBegin);
	TargetSphere->OnComponentEndOverlap.AddDynamic(this, &APlayerCharacter::TargetSphereOnOverlapEnd);
	TargetSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Ignore);
	TargetSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	GetCapsuleComponent()->SetGenerateOverlapEvents(true);
	GetMesh()->SetCollisionProfileName(TEXT("PlayerMesh"));
	GetMesh()->SetGenerateOverlapEvents(true);

	ABAnim->SetBaseAnimMode();
	
	MainPlayerController = Cast<AABPlayerController>(GetController());

	Cast<AABGameMode>(UGameplayStatics::GetGameMode(GetWorld()))->SetGameState();
	Cast<AABGameMode>(UGameplayStatics::GetGameMode(GetWorld()))->SetPlayer(this);
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bInterpToEnemy && LockOnTarget)
	{
		FRotator LookAtYaw = GetLookAtRotationYaw(LockOnTarget->GetActorLocation());

		FRotator InterpRotation = FMath::RInterpTo(GetActorRotation(), LookAtYaw, DeltaTime, InterpSpeed);
		SetActorRotation(InterpRotation);	

		MainPlayerController->SetControlRotation(FRotator(-45.f, InterpRotation.Yaw, 0.f));

	}
}

void APlayerCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	/** 메시가 가지고 있는 AnimationInstance로 ABAnim 초기화 */
	ABAnim = Cast<UABAnimInstance>(GetMesh()->GetAnimInstance());
	
	ABAnim->OnEndAttack.AddLambda([this]()->void {
		bAttacking = false;
		bSkillAttacking = false;

		AttackEndComboState();

		if (bPressedRMouse)
		{
			BlockingStart();
		}
	});

	ABAnim->OnNextAttackCheck.AddLambda([this]()->void {
		bNextAttack = false;

		if (bComboInputOn && CharacterStat->GetCStamina() - CurrentWeapon->GetAttackStamina() >= 0)
		{
			AttackStartComboState();

			if(!bTargeting)
				SetCharacterRotationToAxis();

			if(bPressedLShift)
				ABAnim->JumpToStrongAttackMontageSection(CurrentCombo);
			else
				ABAnim->JumpToAttackMontageSection(CurrentCombo);
		}
	});

	ABAnim->OnEndRolling.AddLambda([this]()->void {
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		bRolling = false;
		SetInterpToEnemy(true);

		if (bPressedRMouse)
		{
			BlockingStart();
		}
	});

	ABAnim->OnEndEquiping.AddLambda([this]()->void {
		CurrentCombatMode = ECombatMode::E_Equip_Mode;
		if (bPressedLShift)
		{
			SetCharacterSpeed(MAXSprintSpeed);
		}
		else if (bPressedLCtrl)
		{
			SetCharacterSpeed(MAXWalkSpeed);
		}
		else
		{
			SetCharacterSpeed(MAXNormalSpeed);
		}
		bEquiping = false;
	});

	ABAnim->OnEndUnEquipping.AddLambda([this]()->void {
		CurrentCombatMode = ECombatMode::E_Normal_Mode;
		if (bPressedLShift)
		{
			SetCharacterSpeed(MAXSprintSpeed);
		}
		else if (bPressedLCtrl)
		{
			SetCharacterSpeed(MAXWalkSpeed);
		}
		else
		{
			SetCharacterSpeed(MAXNormalSpeed);
		}
		bEquiping = false;
	});

	ABAnim->OnChangeWeaponPosition.AddLambda([this]()->void {
		ChangeWeaponPosition();
	});

	ABAnim->OnActivateCollision.AddLambda([this]()->void {
		CurrentWeapon->ActivateCollision();
		if (bSkillAttacking)
		{
			CharacterStat->DecreaseCurrentStamina(CurrentWeapon->GetSkillStamina());
		}
		else
		{
			CharacterStat->DecreaseCurrentStamina(CurrentWeapon->GetAttackStamina());
		}
	});

	ABAnim->OnDeActivateCollision.AddLambda([this]()->void {
		CurrentWeapon->DeActivateCollision();
	});

	ABAnim->OnPlaySwingSound.AddLambda([this]()->void {
		if (CurrentWeapon->SwingSound)
		{
			UGameplayStatics::PlaySound2D(this, CurrentWeapon->SwingSound);
		}
	});

	ABAnim->OnDeadEnd.AddLambda([this]()->void {
		Cast<AABPlayerController>(GetController())->OpenDeadMenu();
	});

	ABAnim->OnBlockingStart.AddLambda([this]()->void {
		bBlocking = true;
	});

	ABAnim->OnBlockingEnd.AddLambda([this]()->void {
		bBlocking = false;
	});

	ABAnim->OnBlockDamageEnd.AddLambda([this]()->void {
		bBlockingHit = false;
	});

	ABAnim->OnShieldHitEnd.AddLambda([this]()->void {
		bBlockingHit = false;
		if (bPressedRMouse)
		{
			ABAnim->PlayBlockingMontage();
			ABAnim->JumpToBlockingMontageLoopSection();
		}
		else
		{
			bBlocking = false;
		}
	});

	ABAnim->OnLandingStart.AddLambda([this]()->void {
		bJumpping = true;
	});

	ABAnim->OnJumpEnd.AddLambda([this]()->void {
		bJumpping = false;
	});

	ABAnim->OnDamageEnd.AddLambda([this]()->void {
		bDamaged = false;
		
		if (bPressedRMouse)
		{
			BlockingStart();
		}		
	});
	
	ABAnim->OnEndSavePose.AddLambda([this]()->void {
		Cast<AABPlayerController>(GetController())->OpenSaveMenu();
		bSavePose = false;
	});

	ABAnim->OnHealing.AddLambda([this]()->void {
		CharacterStat->IncreaseCurrentHealth(PotionRecovery);
		auto DWidget = Cast<UDamageText>(DamageText->GetUserWidgetObject());
		if (DWidget != nullptr)
		{
			DWidget->CreateHealingText(PotionRecovery);
		}
	});

	ABAnim->OnDrinkingEnd.AddLambda([this]()->void {
		bDrinking = false;
		Potion->Destroy();
		SetCharacterSpeed(MAXNormalSpeed);
	});

	ABAnim->OnFire.AddLambda([this]()->void {
		CharacterStat->DecreaseCurrentStamina(CurrentWeapon->GetSkillStamina());
		CurrentWeapon->FireSkill();
	});

	CharacterStat->OnZeroStamina.AddLambda([this]()->void {
		SetCharacterSpeed(MAXNormalSpeed);
		bPressedLShift = false;
	});

	CharacterStat->OnZeroHealth.AddLambda([this]()->void {
		Die();
	});

}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &APlayerCharacter::UpDown);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &APlayerCharacter::LeftRight);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &APlayerCharacter::LookUp);
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &APlayerCharacter::Turn);

	PlayerInputComponent->BindAction(TEXT("Walk"), EInputEvent::IE_Pressed, this, &APlayerCharacter::PressedLCtrl);
	PlayerInputComponent->BindAction(TEXT("Walk"), EInputEvent::IE_Released, this, &APlayerCharacter::ReleasedLCtrl);
	PlayerInputComponent->BindAction(TEXT("LShift"), EInputEvent::IE_Pressed, this, &APlayerCharacter::PressedLShift);
	PlayerInputComponent->BindAction(TEXT("LShift"), EInputEvent::IE_Released, this, &APlayerCharacter::ReleasedLShift);
	PlayerInputComponent->BindAction(TEXT("RMouse"), EInputEvent::IE_Pressed, this, &APlayerCharacter::PressedRMouse);
	PlayerInputComponent->BindAction(TEXT("RMouse"), EInputEvent::IE_Released, this, &APlayerCharacter::ReleasedRMouse);
	PlayerInputComponent->BindAction(TEXT("Interaction"), EInputEvent::IE_Pressed, this, &APlayerCharacter::PressedInteractionKey);
	PlayerInputComponent->BindAction(TEXT("Interaction"), EInputEvent::IE_Released, this, &APlayerCharacter::ReleasedInteractionKey);
	PlayerInputComponent->BindAction(TEXT("Targeting"), EInputEvent::IE_Pressed, this, &APlayerCharacter::TargetingLockOn);

	PlayerInputComponent->BindAction(TEXT("Jump/Roll"), EInputEvent::IE_Pressed, this, &APlayerCharacter::JumpAndRoll);
	PlayerInputComponent->BindAction(TEXT("Attack"), EInputEvent::IE_Pressed, this, &APlayerCharacter::Attack);
	PlayerInputComponent->BindAction(TEXT("Equip"), EInputEvent::IE_Pressed, this, &APlayerCharacter::Equip);

	PlayerInputComponent->BindAction(TEXT("ShowInventory"), EInputEvent::IE_Pressed, this, &APlayerCharacter::OpenInventory);
	PlayerInputComponent->BindAction(TEXT("ESC"), EInputEvent::IE_Pressed, this, &APlayerCharacter::OpenMenu);
	PlayerInputComponent->BindAction(TEXT("ShowCursor"), EInputEvent::IE_Pressed, this, &APlayerCharacter::ShowMouseCursor);

	PlayerInputComponent->BindAction(TEXT("UsePotion"), EInputEvent::IE_Pressed, this, &APlayerCharacter::DrinkingPotion);
}

void APlayerCharacter::SetViewPoint()
{
	if (!bTargeting)
	{
		/** 카메라 지지대 높이 설정 */
		SpringArm->TargetArmLength = 325.0f;
	
		/** 컨트롤러를 기반으로 카메라 지지대 회전 */
		SpringArm->bUsePawnControlRotation = true;	
		
		SpringArm->bEnableCameraLag = true;
		SpringArm->CameraLagSpeed = 6.0f;
		SpringArm->bDoCollisionTest = true;
		
		SpringArm->bInheritPitch = true;
		SpringArm->bInheritRoll = true;
		SpringArm->bInheritYaw = true;
	
		/** 
		* 컨트롤러가 회전할 때 캐릭터가 회전하지 않도록 설정
		* 컨트롤러의 회전이 카메라 에만(정확히는 카메라 지지대) 영향이 미치도록 설정
		*/
		bUseControllerRotationYaw = false;
		bUseControllerRotationRoll = false;
		bUseControllerRotationPitch = false;
	
		/** 
		* 참이면 회전 속도 변화로 RotationRate를 사용하여 캐릭터를 가속 방향으로 회전시킨다.
		* 이 값이 참일 경우 bUseControllerRotationYaw를 비활성화 시켜야 한다.
		* bUseControllerDesiredRotation 에 의해 재정의 될 수 있다.
		*/
		GetCharacterMovement()->bOrientRotationToMovement = true;
	
		/**
		* 참이면 회전 속도 변화율로 RotationRate를 사용하여 캐릭터를 컨트롤러의 원하는 회전 (일반적으로 컨트롤러-> ControlRotation)쪽으로 부드럽게 회전시킨다.
		* 이 값이 참일 경우 bUseControllerRotationYaw를 비활성화 시켜야 한다.
		* bOrientRotationToMovement 에 의해 재정의 될 수 있다.
		*/
		GetCharacterMovement()->bUseControllerDesiredRotation = false;
		 
		/** 
		* 회전속도 변화율
		* 위의 bOrientRotationToMovement, bUseControllerDesiredRotation 에 영향을 준다.
		*/
		GetCharacterMovement()->RotationRate = FRotator(0.0f, 700.0f, 0.0f);
	}
	else
	{
		SpringArm->SetRelativeRotation(FRotator(-30.f, 0.f, 0.f));
		SpringArm->bInheritPitch = false;
		SpringArm->bInheritRoll = false;

		GetCharacterMovement()->bOrientRotationToMovement = false;
		GetCharacterMovement()->bUseControllerDesiredRotation = true;
	}
}

void APlayerCharacter::UpDown(float NewAxisValue)
{
	/**
	* 컨트롤러의 정방향(FRotator(0.0f, GetControlRotation().Yaw, 0.0f))을 찾고,
	* 정방향을 기준으로 X축의 값을(.GetUnitAxis(EAxis::X)) 얻어오고,
	* 새로 들어온 입력 값(NewAxisValue(1, -1))을 근거로 해서 전방향 행렬을 생성 후,
	* 캐릭터의 위치를 이동.
	*/
	bMoveingForward = false;
	if (CanMove(NewAxisValue))
	{
		AddMovementInput(FRotationMatrix(FRotator(0.0f, GetControlRotation().Yaw, 0.0f)).GetUnitAxis(EAxis::X), NewAxisValue);
		bMoveingForward = true;
	}
}

void APlayerCharacter::LeftRight(float NewAxisValue)
{
	/**
	* 컨트롤러의 정방향(FRotator(0.0f, GetControlRotation().Yaw, 0.0f))을 찾고,
	* 정방향을 기준으로 Y축의 값을(.GetUnitAxis(EAxis::Y)) 얻어오고,
	* 새로 들어온 입력 값(NewAxisValue(1, -1))을 근거로 해서 전방향 행렬을 생성 후,
	* 캐릭터의 위치를 이동.
	*/
	bMoveingRight = false;
	if (CanMove(NewAxisValue))
	{
		AddMovementInput(FRotationMatrix(FRotator(0.0f, GetControlRotation().Yaw, 0.0f)).GetUnitAxis(EAxis::Y), NewAxisValue);
		bMoveingRight = true;
	}
}

void APlayerCharacter::LookUp(float NewAxisValue)
{
	if (!bTargeting)
	{
		/** NewAxisValue 의 값에 의하여 컨트롤러 회전 (위 아래) */
		AddControllerPitchInput(NewAxisValue);
	}
}

void APlayerCharacter::Turn(float NewAxisValue)
{
	if (!bTargeting)
	{
		/** NewAxisValue 의 값에 의하여 컨트롤러 회전 (오른쪽 왼쪽) */
		AddControllerYawInput(NewAxisValue);
	}
}

void APlayerCharacter::Attack()
{
	if (CurrentCombatMode == ECombatMode::E_Equip_Mode && !bRolling && !bDrinking)
	{
		if (!GetCharacterMovement()->IsFalling() && !bEquiping && !bRolling && !bDamaged && !bBlockingHit)
		{
			if (bBlocking)
			{
				bBlocking = false;
			}
	
			/** 
			* 이미 공격중인 경우
			* 다음 콤보 어택이 가능하다면 bComboInputOn을 true로 바꾼다.
			* 이로인해 NextAttackCheck 노티파이의 실행시 람다식의 실행으로 다음 콤보 어택이 발동한다.
			*
			** 첫 콤보 어택일 경우
			* AttackMontage를 실행한다.
			*/
			if (bAttacking)
			{
				if (bNextAttack && CharacterStat->GetCStamina() - CurrentWeapon->GetAttackStamina() >= 0)
				{
					bComboInputOn = true;
				}
			}
			else
			{
				/** Shift 입력 여부는 강/약 공격 실행의 분기 */
				if (bPressedLShift && CurrentWeapon->bHasSpecialAttack && CurrentWeapon->GetSkillStamina() <= CharacterStat->GetCStamina())
				{
					bAttacking = true;
					AttackStartComboState();
					if (!bTargeting)
					{
						SetCharacterRotationToAxis();
					}
					bSkillAttacking = true;
					ABAnim->PlayStrongAttackMontage();
					ABAnim->JumpToStrongAttackMontageSection(CurrentCombo);
				}
				else if (!bPressedLShift && CurrentWeapon->GetAttackStamina() <= CharacterStat->GetCStamina())
				{
					bAttacking = true;
					AttackStartComboState();
					if (!bTargeting)
					{
						SetCharacterRotationToAxis();
					}
					ABAnim->PlayAttackMontage();
					ABAnim->JumpToAttackMontageSection(CurrentCombo);
				}
			}
		}
	}
	else
	{
		if(!bEquiping)
			Equip();
	}
}

void APlayerCharacter::BlockingStart()
{
	if (!bAttacking && !bEquiping && !bDamaged && !bDead && !bRolling && CurrentCombatMode == ECombatMode::E_Equip_Mode && !bDrinking)
	{
		ABAnim->PlayBlockingMontage();
		ABAnim->JumpToBlockingMontageStartSection();
	}
}

void APlayerCharacter::BlockingEnd()
{
	if (CurrentCombatMode == ECombatMode::E_Equip_Mode && !bDrinking && (CurrentShield || CurrentWeapon->HasBlocking()))
	{
		if (bBlockingHit)
		{
			//bBlockingHit = false;
		}
		else
		{
			bBlocking = false;
			ABAnim->Montage_Stop(0.1f, ABAnim->GetBlockingMontage());
		}
	}
}

void APlayerCharacter::JumpAndRoll()
{
	/** 
	* 캐릭터가 비전투모드일 경우 점프를 실행
	* 전투 모드일 경우 구르기, 뒤로회피를 실행한다.
	*/
	if (!bDamaged && !bDead && !bBlockingHit && CharacterStat->GetCStamina() >= JumpAndRollingStamina && !bAttacking && !bSavePose && !bDrinking)
	{
		switch (CurrentCombatMode)
		{
		case ECombatMode::E_Normal_Mode:
			if (!GetCharacterMovement()->IsFalling() && !bEquiping)
			{
				//(*Stamina) -= JumpAndRollingStamina;
				CharacterStat->DecreaseCurrentStamina(JumpAndRollingStamina);
				bPressedJump = true;
				JumpKeyHoldTime = 0.0f;
			}
			break;
		case ECombatMode::E_Equip_Mode:
			if (!GetCharacterMovement()->IsFalling() && !bRolling && !bEquiping && !bJumpping)
			{
				/** 
				* 회피키 입력 시 방향키를 누르고 있는 상태라면 해당 방향으로 구르기 시전
				* 아닐 경우 뒤로 회피
				*/
				CharacterStat->DecreaseCurrentStamina(JumpAndRollingStamina);
				SetInterpToEnemy(false);
				if (bPressedRMouse)
				{
					BlockingEnd();
				}

				if (!SetCharacterRotationToAxis())
				{
					ABAnim->PlayBackDodgeMontage();
				}
				else
				{
					GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
					ABAnim->PlayRollMontage();
					bRolling = true;
				}
			}
			break;
		}
	}
}

void APlayerCharacter::Equip()
{
	if (!GetCharacterMovement()->IsFalling() && !bRolling && !bEquiping && !bPressedRMouse && !bAttacking && !bDamaged && !bDead && CurrentWeapon && !bBlocking && !bDrinking)
	{
		bEquiping = true;

		/** 무기에 Equip 사운드가 있다면 소리 실행 이 부부은 변경 예정 */
		if (CurrentWeapon->EquipSound)
		{
			UGameplayStatics::PlaySound2D(this, CurrentWeapon->EquipSound);
		}
		
		if (CurrentCombatMode == ECombatMode::E_Normal_Mode)
		{
			ABAnim->PlayEquippingMontage();
			ABAnim->JumpToEquippingMontageSection();
		}
		else
		{
			ABAnim->PlayEquippingMontage();
			ABAnim->JumpToUnEquippingMontageSection();
		}
			
	}
}

void APlayerCharacter::SetCharacterSpeed(float NewSpeed)
{
	if (!bTargeting)
	{
		switch (CurrentCombatMode)
		{
		case ECombatMode::E_Normal_Mode:
			GetCharacterMovement()->MaxWalkSpeed = NewSpeed;
			break;
		case ECombatMode::E_Equip_Mode:
			if(!bPressedLCtrl)
				GetCharacterMovement()->MaxWalkSpeed = NewSpeed - 100;
			else
				GetCharacterMovement()->MaxWalkSpeed = NewSpeed;
			break;
		}
	}
	else
	{
		if (bPressedLCtrl)
		{
			GetCharacterMovement()->MaxWalkSpeed = 150.0f;
		}
		else
		{
			GetCharacterMovement()->MaxWalkSpeed = 400.0f;
		}
	}
}

void APlayerCharacter::PressedLCtrl()
{
	bPressedLCtrl = true;
	if (!bDrinking)
	{
		SetCharacterSpeed(MAXWalkSpeed);
	}
}

void APlayerCharacter::ReleasedLCtrl()
{
	bPressedLCtrl = false;
	if (!bDrinking)
	{
		SetCharacterSpeed(MAXNormalSpeed);
	}
}

void APlayerCharacter::PressedInteractionKey()
{
	if (!GetCharacterMovement()->IsFalling() && !bDead && !bDrinking)
	{
		bPressedInteractionKey = true;
		if (TargetItemPot && !(MainPlayerController->IsOpenLootWindow()))
		{
			MainPlayerController->OpenLootWindow(TargetItemPot);
		}
		else if (Cast<ASaveActor>(InteractionTarget) && CurrentCombatMode == ECombatMode::E_Normal_Mode && !bPressedLShift)
		{
			bSavePose = true;
			ABAnim->PlaySavePoseMontage();
		}
		else if (TargetNpc != nullptr && CurrentCombatMode == ECombatMode::E_Normal_Mode && !IsMoveing())
		{
			if (TargetNpc->GetCategory() == ENPCCategory::Trader)
			{
				MainPlayerController->OpenTradeWindow(TargetNpc);
			}
			else if (TargetNpc->GetCategory() == ENPCCategory::Trainer)
			{
				CharacterStat->RenewalLevelUpCost();
				MainPlayerController->OpenTrainerWindow();
			}
		}
	}
}

void APlayerCharacter::ReleasedInteractionKey()
{
	//bPressedInteractionKey = false;
}

void APlayerCharacter::PressedLShift() 
{
	bPressedLShift = true;
	if (!bTargeting && !bDrinking)
	{
		SetCharacterSpeed(MAXSprintSpeed);
	}
}

void APlayerCharacter::ReleasedLShift()
{
	bPressedLShift = false;
	if (!bTargeting && !bDrinking)
	{
		SetCharacterSpeed(MAXNormalSpeed);
	}
}

void APlayerCharacter::PressedRMouse()
{
	bPressedRMouse = true;
	if(CharacterStat->GetCStamina() >= 30.f)
	BlockingStart();
}

void APlayerCharacter::ReleasedRMouse()
{
	bPressedRMouse = false;
	if(!bBlockingHit)
		BlockingEnd();
}

void APlayerCharacter::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	bAttacking = false;
	SetInterpToEnemy(false);
	AttackEndComboState();
}

void APlayerCharacter::AttackStartComboState()
{
	bNextAttack = true;
	bComboInputOn = false;
	CurrentCombo = CurrentCombo % CurrentWeapon->ComboNum + 1;
}

void APlayerCharacter::AttackEndComboState()
{
	bNextAttack = false;
	bComboInputOn = false;
	CurrentCombo = 0;
}

bool APlayerCharacter::SetCharacterRotationToAxis()
{
	/** 이동키 입력에 따른 값(1 or -1)을 2D벡터에 저장 */
	FVector2D mov;
	mov.X = GetInputAxisValue("MoveForward");
	mov.Y = GetInputAxisValue("MoveRight");

	/** 컨트롤러의 Rotation 값을 가져온다 */
	const FRotator Rotation = GetController()->GetControlRotation();

	/** 새로 바뀔 캐릭터의 Rotation값 */
	FRotator YawRotation;

	if (mov.X == 0 && mov.Y == 0)
	{
		return false;
	}
	else if (mov.X == 0)
	{
		YawRotation = FRotator(0.f, Rotation.Yaw + (90.0f*mov.Y), 0.0f);
	}
	else if (mov.Y == 0)
	{
		YawRotation = FRotator(0.f, Rotation.Yaw - (90.0f*(mov.X - 1)), 0.0f);
	}
	else
	{
		YawRotation = FRotator(0.f, Rotation.Yaw + (((45-(mov.X*45))+45)*mov.Y), 0.0f);
	}

	SetActorRotation(YawRotation);
	
	return true;
}

/** 추후에 변경될 메서드 */
void APlayerCharacter::SetInterpToEnemy(bool Interp)
{
	bInterpToEnemy = Interp;
}

FRotator APlayerCharacter::GetLookAtRotationYaw(FVector Target)
{
	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Target);
	FRotator LookAtRotationYaw(0.f, LookAtRotation.Yaw, 0.f);
	return LookAtRotationYaw;
}
//

/** 캐릭터가 데미지를 받을 경우 (ApplyDamage 메서드의 대상이 되었을 경우 호출) */
float APlayerCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{	
	bAttacking = false;
	bRolling = false;
	AEnemy* Enemy = Cast<AEnemy>(DamageCauser);
	if (Enemy)
	{
		if (bDrinking)
		{
			bDrinking = false;
			Potion->Destroy();
			SetCharacterSpeed(MAXNormalSpeed);
		}

		if (bBlocking)
		{
			FRotator BetweenAngle = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), DamageCauser->GetActorLocation());
			FRotator EnemyLocation = UKismetMathLibrary::NormalizedDeltaRotator(GetActorRotation(), BetweenAngle);
			bool bDamageZone = (UKismetMathLibrary::InRange_FloatFloat(EnemyLocation.Yaw, -180.0f, -80.0f) || UKismetMathLibrary::InRange_FloatFloat(EnemyLocation.Yaw, 80.0f, 180.0f));
			
	
			if (bDamageZone)
			{
				Cast<AEnemy>(DamageCauser)->SuccessAttack(this);
				Damaged(DamageAmount);
			}
			else
			{
				if (CurrentShield)
				{
					UGameplayStatics::PlaySound2D(this, GetCurrentShield()->BlockingSound);
		
					bBlockingHit = true;
					CharacterStat->DecreaseCurrentStamina(DamageAmount*0.3);
					if (CharacterStat->GetCStamina() == 0)
					{
						bBlocking = false;
						ABAnim->PlayBlockingMontage();
						ABAnim->JumpToBlockingMontageDamageSection();
					}
					else
					{
						ABAnim->PlayBlockingMontage();
						ABAnim->JumpToBlockingMontageBlockSection();
					}
	
				}
				else if (CurrentWeapon->HasBlocking())
				{
					ABAnim->PlayBlockingMontage();
					ABAnim->JumpToBlockingMontageBlockSection();
				}
				if (!(Enemy->MonsterType == EMonsterType::Boss))
				{
					Cast<AEnemy>(DamageCauser)->BlockedAttack();
				}
			}
		}
		else
		{
			Cast<AEnemy>(DamageCauser)->SuccessAttack(this);
			Damaged(DamageAmount);
		}
	}
	else {
		if (bDrinking)
		{
			bDrinking = false;
			Potion->Destroy();
			SetCharacterSpeed(MAXNormalSpeed);
		}
		Damaged(DamageAmount);
		return DamageAmount;
	}
	return DamageAmount;
}

void APlayerCharacter::Damaged(float Amount)
{
	Amount = CharacterStat->DecreaseCurrentHealth(Amount);
	auto DWidget = Cast<UDamageText>(DamageText->GetUserWidgetObject());
	if (DWidget != nullptr)
	{
		DWidget->CreateDamageText(Amount);
	}

	if (bAttacking)
	{
		bAttacking = false;
		AttackEndComboState();
		CurrentWeapon->DeActivateCollision();
	}
	else if (bEquiping)
	{
		ChangeWeaponPosition(true);
		bEquiping = false;
	}

	if (bBlocking)
	{
		bBlockingHit = false;
		BlockingEnd();
	}

	bDamaged = true;
	if (CharacterStat->GetCHealth() > 0)
	{
		ABAnim->PlayDamageMontage();
	}
}

void APlayerCharacter::Die()
{
	bDead = true;
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	if (Cast<AABPlayerController>(GetController())->IsOpenInventoryWindow())
	{
		Cast<AABPlayerController>(GetController())->OpenInventoryWindow();
	}
	if (Cast<AABPlayerController>(GetController())->IsOpenLootWindow())
	{
		Cast<AABPlayerController>(GetController())->CloseLootWindow();
	}
	if (Cast<AABPlayerController>(GetController())->IsOpenSaveMenu())
	{
		Cast<AABPlayerController>(GetController())->OpenSaveMenu();
	}
	if (Cast<AABPlayerController>(GetController())->IsOpenMenuWindow())
	{
		Cast<AABPlayerController>(GetController())->OpenMenuWindow();
	}
}

void APlayerCharacter::SwitchLevel(FName LevelName)
{
	UWorld* World = GetWorld();
	if (World)
	{
		FString CurrentLevel = World->GetMapName();

		FName CurrentLevelName(*CurrentLevel);
		if (CurrentLevelName != LevelName)
		{
			UGameplayStatics::OpenLevel(World, LevelName);
		}
	}
}

bool APlayerCharacter::CanMove(float Value)
{
	if (MainPlayerController)
	{
		return (!bAttacking && Value != 0.0f && !bJumpping && !bDamaged && !bDead && !bBlockingHit && !bSavePose && !bRolling);
	}
	return false;
}

void APlayerCharacter::ChangeWeaponPosition(bool reverse)
{
	if (!reverse)
	{
		if (CurrentCombatMode == ECombatMode::E_Equip_Mode)
		{
			CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, CurrentWeapon->GetUnEquipPosion());
			if(CurrentShield)
				CurrentShield->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, CurrentShield->GetUnEquipPosition());
		}
		else
		{
			CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, CurrentWeapon->GetEquipPosion());
			if(CurrentShield)
				CurrentShield->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, CurrentShield->GetEquipPosition());
		}
	}
	else
	{
		if (CurrentCombatMode == ECombatMode::E_Equip_Mode)
		{
			CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, CurrentWeapon->GetEquipPosion());
			if (CurrentShield)
				CurrentShield->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, CurrentShield->GetEquipPosition());
		}
		else
		{
			CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, CurrentWeapon->GetUnEquipPosion());
			if (CurrentShield)
				CurrentShield->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, CurrentShield->GetUnEquipPosition());
		}
	}
}

void APlayerCharacter::OpenInventory()
{
	if (!bDead)
	{
		MainPlayerController->OpenInventoryWindow();
	}
}

void APlayerCharacter::OpenMenu()
{
	if (!bDead)
	{
		MainPlayerController->OpenMenuWindow();
	}
}

void APlayerCharacter::EquipHead(USkeletalMesh* HeadItem)
{
	HairPart->SetSkeletalMesh(HeadItem);
}

void APlayerCharacter::UnEquipHead()
{
	HairPart->SetSkeletalMesh(nullptr);
}

void APlayerCharacter::EquipTop(USkeletalMesh* TopItem)
{
	TopClothesPart->SetSkeletalMesh(TopItem);
}

void APlayerCharacter::UnEquipTop()
{
	TopClothesPart->SetSkeletalMesh(nullptr);
}

void APlayerCharacter::EquipBot(USkeletalMesh* TopItem)
{
	BottomPart->SetSkeletalMesh(TopItem);
}

void APlayerCharacter::UnEquipBot()
{
	BottomPart->SetSkeletalMesh(Cast<UABGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()))->GetItemData(FName("BaseBottom"))->GetEquipMesh());
}

void APlayerCharacter::EquipOneHandedWeapon(TSubclassOf<AWeapon> NewWeapon)
{
	if (CurrentWeapon)
	{
		if (CurrentWeapon->GetWeaponType() == EWeaponTypes::TwoHandWeapon)
		{
			UnEquipTwoHandedWeapon();
		}
		else
		{
			UnEquipOneHandedWeapon();
		}
	}
	
	CurrentWeapon = GetWorld()->SpawnActor<AWeapon>(NewWeapon, FVector::ZeroVector, FRotator::ZeroRotator);
	if (CurrentWeapon != nullptr)
	{
		CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, CurrentWeapon->GetUnEquipPosion());
		CurrentWeapon->SetInstigator(GetController());
		CurrentWeapon->SetWeaponOwner(this);

		ABAnim->SetAttackMontage(CurrentWeapon->GetAttackMontage());
		if (CurrentWeapon->bHasSpecialAttack)
		{
			ABAnim->SetStrongAttackMontage(CurrentWeapon->GetStrongAttackMontage());
		}
		ABAnim->SetEquippingMontage(CurrentWeapon->GetEquippingMontage());
	}
}

void APlayerCharacter::UnEquipOneHandedWeapon()
{
	CurrentWeapon->Destroy();
	CurrentWeapon = nullptr;
}

void APlayerCharacter::EquipTwoHandedWeapon(TSubclassOf<AWeapon> NewWeapon)
{
	if (CurrentWeapon)
	{
		if (CurrentWeapon->GetWeaponType() == EWeaponTypes::TwoHandWeapon)
		{
			UnEquipTwoHandedWeapon();
		}
		else
		{
			UnEquipOneHandedWeapon();
		}
	}

	CurrentWeapon = GetWorld()->SpawnActor<AWeapon>(NewWeapon, FVector::ZeroVector, FRotator::ZeroRotator);
	if (CurrentWeapon != nullptr)
	{
		CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, CurrentWeapon->GetUnEquipPosion());
		CurrentWeapon->SetInstigator(GetController());
		CurrentWeapon->SetWeaponOwner(this);

		ABAnim->SetAttackMontage(CurrentWeapon->GetAttackMontage());
		if (CurrentWeapon->bHasSpecialAttack)
		{
			ABAnim->SetStrongAttackMontage(CurrentWeapon->GetStrongAttackMontage());
		}
		ABAnim->SetEquippingMontage(CurrentWeapon->GetEquippingMontage());
	}
}

void APlayerCharacter::UnEquipTwoHandedWeapon()
{
	ABAnim->SetBlockingMontage(nullptr);
	CurrentWeapon->Destroy();
	CurrentWeapon = nullptr;
}


void APlayerCharacter::EquipShield(TSubclassOf<AShield> NewShield)
{
	if (CurrentShield)
	{
		UnEquipShield();
	}
	CurrentShield = GetWorld()->SpawnActor<AShield>(NewShield, FVector::ZeroVector, FRotator::ZeroRotator);
	if (CurrentShield != nullptr)
	{
		CurrentShield->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, CurrentShield->GetUnEquipPosition());
		CurrentShield->SetInstigator(GetController());

		ABAnim->SetBlockingMontage(CurrentShield->GetBlockingMontage());
	}
}

void APlayerCharacter::UnEquipShield()
{
	ABAnim->SetBlockingMontage(nullptr);
	CurrentShield->Destroy();
	CurrentShield = nullptr;
}

void APlayerCharacter::ShowMouseCursor()
{
	if (MainPlayerController->bShowMouseCursor)
	{
		MainPlayerController->bShowMouseCursor = false;
	}
	else
	{
		MainPlayerController->bShowMouseCursor = true;
	}
}

void APlayerCharacter::AddItemPot(AItemPot* NewItemPot)
{
	ItPots.Emplace(NewItemPot);
	TargetItemPot = NewItemPot;
}

void APlayerCharacter::RemoveItemPot(AItemPot* RemovedItemPot)
{
	ItPots.Remove(RemovedItemPot);

	if (TargetItemPot == RemovedItemPot)
	{
		MainPlayerController->CloseLootWindow();
		if (ItPots.Num() > 0)
		{
			TargetItemPot = ItPots[ItPots.Num()-1];
		}
		else
		{
			TargetItemPot = nullptr;
		}
	}
}

void APlayerCharacter::AddTarget()
{
	SetInterpToEnemy(true);
}

void APlayerCharacter::TargetSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		AEnemy* DetectedEnemy = Cast<AEnemy>(OtherActor);
		if(DetectedEnemy)
		{
			TargetingEnemys.Emplace(DetectedEnemy);
		}
	}
}

void APlayerCharacter::TargetSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		AEnemy* DetectedEnemy = Cast<AEnemy>(OtherActor);
		if (DetectedEnemy)
		{
			RemoveEnemyToTarget(DetectedEnemy);
			if (LockOnTarget == DetectedEnemy)
			{
				TargetingLockOn();
			}
		}
	}
}

void APlayerCharacter::TargetingLockOn()
{
	float CloseDistance = 2000;
	if (!bDrinking)
	{
		if (TargetingEnemys.Num() > 0 && !bTargeting)
		{
			for (auto& Enemy : TargetingEnemys)
			{
				FRotator BetweenAngle = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Enemy->GetActorLocation());
				FRotator EnemyLocation = UKismetMathLibrary::NormalizedDeltaRotator(GetActorRotation(), BetweenAngle);
				bool bDamageZone = (UKismetMathLibrary::InRange_FloatFloat(EnemyLocation.Yaw, -180.0f, -90.0f) || UKismetMathLibrary::InRange_FloatFloat(EnemyLocation.Yaw, 90.0f, 180.0f));
				
				if (!bDamageZone)
				{
					if (CloseDistance > GetDistanceTo(Enemy))
					{
						CloseDistance = GetDistanceTo(Enemy);
						LockOnTarget = Enemy;
					}
				}
			}
			if (LockOnTarget)
			{
				LockOnTarget->ShowTargetPoint(true);
				bTargeting = true;
				SetInterpToEnemy(true);
				SetViewPoint();
				SetCharacterSpeed(0.f);
				UE_LOG(LogTemp, Error, TEXT("??"));
			}
		}
		else
		{
			if (LockOnTarget)
			{
				LockOnTarget->ShowTargetPoint(false);
				LockOnTarget = nullptr;
			}
			bTargeting = false;
			SetViewPoint();
			SetInterpToEnemy(false);
			if (bPressedLShift)
			{
				SetCharacterSpeed(MAXSprintSpeed);
			}
			else if(bPressedLCtrl)
			{
				SetCharacterSpeed(MAXWalkSpeed);
			}
			else
			{
				SetCharacterSpeed(MAXNormalSpeed);
			}
		}
	}
}

bool APlayerCharacter::CheckStopRecoveryStamina()
{
	return bAttacking || bJumpping || bRolling || GetCharacterMovement()->IsFalling() || bBlocking || bBlockingHit || bDamaged;
}

bool APlayerCharacter::CheckPlayConsumeStamina()
{
	return bPressedLShift && IsMoveing() && !bTargeting && !bDrinking;
}

void APlayerCharacter::SetTargetNpc(ANpc* Target)
{
	TargetNpc = Target;
}

void APlayerCharacter::GainMoney(int32 GMoney)
{
	auto DWidget = Cast<UDamageText>(DamageText->GetUserWidgetObject());
	if (DWidget != nullptr)
	{
		DWidget->CreateMoneyText(GMoney);
	}
	MainPlayerController->RequestIncMoney(GMoney);
	UGameplayStatics::PlaySound2D(this, CoinSound);
}

void APlayerCharacter::DrinkingPotion()
{
	if (!bDrinking && CanMove(1.f) && !bEquiping)
	{
		PotionRecovery = MainPlayerController->RequestPotion();
		if (PotionRecovery > 0)
		{
			if (bBlocking)
			{
				BlockingEnd();
			}
			bDrinking = true;
			GetCharacterMovement()->MaxWalkSpeed = 150.0f;
			Potion = GetWorld()->SpawnActor<APotion>(Potion_BP, FVector::ZeroVector, FRotator::ZeroRotator);
			Potion->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, Potion->GetSpawnPosition());
			ABAnim->PlayDrinkingMontage();
		}
	}
}