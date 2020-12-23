# Capstone

These sources were used to create an Action RPG game, and here I will cover the description of the character sources I wrote.


## Overview
* #### Character system overview
![CharacterSystemOverview](https://user-images.githubusercontent.com/74805177/99946111-2b161800-2db9-11eb-8671-40716b4d0974.png)
The picture above is a class diagram showing the simplified structure of this game.



## Character movement
### Movement related codes
* #### Bind axis
```C++
void APlayerCharacter::SetViewPoint()
{
	if (!bTargeting)
	{
		/** SpringArmComponent height setting */
		SpringArm->TargetArmLength = 325.0f;
	
		/** SpringArmComponent rotation based on the controller */
		SpringArm->bUsePawnControlRotation = true;	
		
		/** CameraLag setting
		SpringArm->bEnableCameraLag = true;
		SpringArm->CameraLagSpeed = 6.0f;
		
		/** If the spring arm collides, TargetArmLength decreases */
		SpringArm->bDoCollisionTest = true;
		
		SpringArm->bInheritPitch = true;
		SpringArm->bInheritRoll = true;
		SpringArm->bInheritYaw = true;
	
		/** Set the character to not rotate when the controller rotates */
		bUseControllerRotationYaw = false;
		bUseControllerRotationRoll = false;
		bUseControllerRotationPitch = false;
	
		/** RotationRate is used to rotate the character in the acceleration direction by changing the rotation speed */
		GetCharacterMovement()->bOrientRotationToMovement = true;
	
		/** If this value is true, the RotationRate is used as the rate of change of the rotational speed to smoothly rotate the character towards the desired rotation of the controller (usually Controller->ControlRotation) */
		GetCharacterMovement()->bUseControllerDesiredRotation = false;
		 
		/** Rate of change of rotation speed */
		GetCharacterMovement()->RotationRate = FRotator(0.0f, 700.0f, 0.0f);
	}
	...
}
```
The above method is called from the PlayerCharacter's constructor and sets the player's view of the camera.


* #### Up & Down
```c++ 	
void APlayerCharacter::UpDown(float NewAxisValue)
{
	bMoveingForward = false;
	if (CanMove(NewAxisValue))
	{
		AddMovementInput(FRotationMatrix(FRotator(0.0f, GetControlRotation().Yaw, 0.0f)).GetUnitAxis(EAxis::X), NewAxisValue);
		bMoveingForward = true;
	}
}
```
Find out which side is forward, and move the character forward or backward depending on the value of NewAxisValue

* #### Left & Right
```c++ 	
void APlayerCharacter::LeftRight(float NewAxisValue)
{
	bMoveingRight = false;
	if (CanMove(NewAxisValue))
	{
		AddMovementInput(FRotationMatrix(FRotator(0.0f, GetControlRotation().Yaw, 0.0f)).GetUnitAxis(EAxis::Y), NewAxisValue);
		bMoveingRight = true;
	}
}
```
Find out which side is right, and move the character forward or backward depending on the value of NewAxisValue

* #### Look up
```c++
void APlayerCharacter::LookUp(float NewAxisValue)
{
	if (!bTargeting)
	{
		AddControllerPitchInput(NewAxisValue);
	}
}
```
Changes the camera pitch value according to the mouse movement(=NewAxisValue)

* #### Turn
```c++
void APlayerCharacter::Turn(float NewAxisValue)
{
	if (!bTargeting)
	{
		AddControllerYawInput(NewAxisValue);
	}
}
```
Changes the camera yaw value according to the mouse movement(=NewAxisValue)

* #### Bind axis
```C++
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &APlayerCharacter::UpDown);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &APlayerCharacter::LeftRight);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &APlayerCharacter::LookUp);
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &APlayerCharacter::Turn);
	...
}
```
Binding methods called based on player input


## Character Walk/Run
<img src="https://user-images.githubusercontent.com/74805177/101165275-b6c15b80-3679-11eb-93a7-201546aeea28.gif" width="50%" height="30%"> 
As shown in the picture above, the movement animation that is executed is changed whenever the movement speed of the player character changes.

<img src="https://user-images.githubusercontent.com/74805177/101165468-0ef85d80-367a-11eb-8ec0-e93d7ac18657.png" width="50%" height="30%"> 
The variable value that will determine the animation to be run in this blend space is CurrentPawnSpeed to be.

```c++
void UABAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	
	auto Pawn = TryGetPawnOwner();
	if (::IsValid(Pawn) && !IsDead)
	{
		CurrentPawnSpeed = Pawn->GetVelocity().Size();
		...
	}
}
```
The value of currentpawnspeed changes by getting the character's speed value every frame.

### Walk/Run system flow
* #### 1. Player presses ctrl or shift to walk or run

* #### 2. Catch that input in SetupPlayerInputComponent method and call the PressedLCtrl() or PressedLShift()
```c++
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	...
	PlayerInputComponent->BindAction(TEXT("Walk"), EInputEvent::IE_Pressed, this, &APlayerCharacter::PressedLCtrl);
	...
	PlayerInputComponent->BindAction(TEXT("LShift"), EInputEvent::IE_Pressed, this, &APlayerCharacter::PressedLShift);
	...
}
```

* #### 3. Execute PressedCtrl() or PressedLShift() to change the player's movement speed
```C++
APlayerCharacter::APlayerCharacter()
{
	...
	MAXWalkSpeed = 150.0f;								  
	MAXNormalSpeed = 500.0f;							  
	MAXSprintSpeed = 700.0f;	
	...
}

void APlayerCharacter::PressedLCtrl()
{
	bPressedLCtrl = true;
	if (!bDrinking)
	{
		SetCharacterSpeed(MAXWalkSpeed);
	}
}

void APlayerCharacter::PressedLShift() 
{
	bPressedLShift = true;
	if (!bTargeting && !bDrinking)
	{
		SetCharacterSpeed(MAXSprintSpeed);
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

```

* #### 4. If the held ctrl or shift key is released, the movement speed is restored again
```c++
void APlayerCharacter::ReleasedLCtrl()
{
	bPressedLCtrl = false;
	if (!bDrinking)
	{
		SetCharacterSpeed(MAXNormalSpeed);
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
```

## Character attack
### Attack system flow
* #### 1. Player left mouse click to attack

* #### 2. Catch that input in SetupPlayerInputComponent method and call the Attack()
```C++
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	...
	PlayerInputComponent->BindAction(TEXT("Attack"), EInputEvent::IE_Pressed, this, &APlayerCharacter::Attack);
	...
}
```

* #### 3. Execute Attack()
#### 3.1 if first attack
<img src="https://user-images.githubusercontent.com/74805177/100538936-20a2c500-3276-11eb-90b4-97cb8502b24b.gif" width="50%" height="30%"> 

```C++
void APlayerCharacter::Attack()
{
	...
			if (bAttacking)
			{
				...
			}
			else
			{
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
	...
}
```
If it is the first attack, execute the code that executes attackmontage.

#### 3.2 if combo attack
<img src="https://user-images.githubusercontent.com/74805177/100539261-b2abcd00-3278-11eb-8494-d4a23d516d0a.gif" width="50%" height="30%"> <img src="https://user-images.githubusercontent.com/74805177/100539638-2fd84180-327b-11eb-9e39-84b54215766d.gif" width="50%" height="30%">

```C++
void APlayerCharacter::Attack()
{
	...
			if (bAttacking)
			{
				if (bNextAttack && CharacterStat->GetCStamina() - CurrentWeapon->GetAttackStamina() >= 0)
				{
					bComboInputOn = true;
				}
			}
			else
			{
				...
			}
	...
}
```
```C++
ABAnim->OnNextAttackCheck.AddLambda([this]()->void {
	bNextAttack = false;

	if (bComboInputOn && CharacterStat->GetCStamina() - CurrentWeapon->GetAttackStamina() >= 0)
	{
		AttackStartComboState();

		...
		
		if(bPressedLShift)
			ABAnim->JumpToStrongAttackMontageSection(CurrentCombo);
		else
			ABAnim->JumpToAttackMontageSection(CurrentCombo);
	}
});
```
```C++
void APlayerCharacter::AttackStartComboState()
{
	bNextAttack = true;
	bComboInputOn = false;
	CurrentCombo = CurrentCombo % CurrentWeapon->ComboNum + 1;
}
```
If an attack is attempted again before the attack is over (before NextAttackCheck Notify), the next combo attack is triggered by passing the section of Attack Montage to the next.

* #### 4. End Attack
```C++
void APlayerCharacter::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	bAttacking = false;
	...
	AttackEndComboState();
}
```
```C++
void APlayerCharacter::AttackEndComboState()
{
	bNextAttack = false;
	bComboInputOn = false;
	CurrentCombo = 0;
}
```
If AttackMontage reaches the EndAttack point without executing the combo attack, OnAttackMontageEnded() is executed to return to the initial state.


## Character blocking
### Blocking system flow
* #### 1. Player right mouse click to blocking

* #### 2. Catch that input in SetupPlayerInputComponent method and call the PressedRMouse()
```C++
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	...
	PlayerInputComponent->BindAction(TEXT("RMouse"), EInputEvent::IE_Pressed, this, &APlayerCharacter::PressedRMouse);
	...
}
```

* #### 3. Execute PressedRMouse()
```C++
void APlayerCharacter::PressedRMouse()
{
	bPressedRMouse = true;
	if(CharacterStat->GetCStamina() >= 30.f)
		BlockingStart();
}
```
```C++
void APlayerCharacter::BlockingStart()
{
	if (!bAttacking && !bEquiping && !bDamaged && !bDead && !bRolling && CurrentCombatMode == ECombatMode::E_Equip_Mode && !bDrinking)
	{
		ABAnim->PlayBlockingMontage();
		ABAnim->JumpToBlockingMontageStartSection();
	}
}
```

* #### 4. Play blocking montage
<img src="https://user-images.githubusercontent.com/74805177/100548981-8c575300-32b3-11eb-9a65-93920780413c.gif" width="50%" height="30%">

* #### 5. When the player takes damage, Execute TakeDamage() and checks whether the attacking monster is in the Damage Zone
<img src="https://user-images.githubusercontent.com/74805177/100596306-5a8bce00-333f-11eb-9394-cf68711acef8.png" width="50%" height="30%"><img src="https://user-images.githubusercontent.com/74805177/100596317-5d86be80-333f-11eb-9dfb-39edcacfe55c.png" width="50%" height="30%">

```C++
float APlayerCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{	
	bAttacking = false;
	bRolling = false;
	AEnemy* Enemy = Cast<AEnemy>(DamageCauser);
	if (Enemy)
	{
		...
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
	...
	return DamageAmount;
}
```
If the attacking monster is in the Block Zone, the player blocks the attack, and if it is in the Damage Zone, the player takes damage.


## Targeting the character to look at the monster
### Targeting system flow
* #### 1. Player presses 'q' key to evade enemy attack

* #### 2. Catch that input in SetupPlayerInputComponent method and call the TargetingLockOn()
```c++
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	...
	PlayerInputComponent->BindAction(TEXT("Targeting"), EInputEvent::IE_Pressed, this, &APlayerCharacter::TargetingLockOn);
	...
}
```

* #### 3. Execute TargetingLockOn()
<img src="https://user-images.githubusercontent.com/74805177/100698681-62e61680-33dc-11eb-8b8e-f9e8ff8082fb.gif" width="50%" height="30%">

```C++
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
```
The targeting system works in the same way as the blocking system, but a limitation has been added that it only works on monsters within a range of 2000.

## Attack avoidance
### evasion system flow
* #### 1. Player presses spacebar to evade enemy attack

* #### 2. Catch that input in SetupPlayerInputComponent method and call the JumpAndRoll()
```C++
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	...
	PlayerInputComponent->BindAction(TEXT("Jump/Roll"), EInputEvent::IE_Pressed, this, &APlayerCharacter::JumpAndRoll);
	...
}
```

* #### 3. Execute JumpAndRoll()
#### 3.1 if not combat mode
<img src="https://user-images.githubusercontent.com/74805177/100629483-af920900-336c-11eb-9907-544b778e5dd5.gif" width="50%" height="30%">

```C++
void APlayerCharacter::JumpAndRoll()
{
	if (!bDamaged && !bDead && !bBlockingHit && CharacterStat->GetCStamina() >= JumpAndRollingStamina && !bAttacking && !bSavePose && !bDrinking)
	{
		switch (CurrentCombatMode)
		{
		case ECombatMode::E_Normal_Mode:
			if (!GetCharacterMovement()->IsFalling() && !bEquiping)
			{
				CharacterStat->DecreaseCurrentStamina(JumpAndRollingStamina);
				bPressedJump = true;
				JumpKeyHoldTime = 0.0f;
			}
			break;
		case ECombatMode::E_Equip_Mode:
			...
			break;
		}
	}
}
```
Then jump

#### 3.2 if combat mode
##### 3.2.1 Set character's rotation to input axis
<img src="https://user-images.githubusercontent.com/74805177/100635962-84131c80-3374-11eb-9387-bf1be5086ab8.png" width="50%" height="30%">

```C++
bool APlayerCharacter::SetCharacterRotationToAxis()
{
	/** Save the value (1 or -1) according to the movement key input in a 2D vector */
	FVector2D mov;
	mov.X = GetInputAxisValue("MoveForward");
	mov.Y = GetInputAxisValue("MoveRight");

	/** Get the controller's Rotation value */
	const FRotator Rotation = GetController()->GetControlRotation();

	/** Setting the Rotation value of the new character */
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
```

##### 3.2.2 Play evasion
<img src="https://user-images.githubusercontent.com/74805177/100629930-3a730380-336d-11eb-81dd-d37129c9a7c7.gif" width="50%" height="30%">

```C++
void APlayerCharacter::JumpAndRoll()
{
	if (!bDamaged && !bDead && !bBlockingHit && CharacterStat->GetCStamina() >= JumpAndRollingStamina && !bAttacking && !bSavePose && !bDrinking)
	{
		switch (CurrentCombatMode)
		{
		case ECombatMode::E_Normal_Mode:
			...
			break;
		case ECombatMode::E_Equip_Mode:
			if (!GetCharacterMovement()->IsFalling() && !bRolling && !bEquiping && !bJumpping)
			{
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
```


## Character equipping
### Equipping system flow
* #### 1. The player presses the Tab or attack key to switch to combat mode

* #### 2. Catch that input in SetupPlayerInputComponent method and call the Equip()
```C++
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	...
	PlayerInputComponent->BindAction(TEXT("Equip"), EInputEvent::IE_Pressed, this, &APlayerCharacter::Equip);
	...
}
```

* #### 3. Execute Equip()
##### 3.1 if player is normal mode
<img src="https://user-images.githubusercontent.com/74805177/100643354-45ce2b00-337d-11eb-85a0-649a6b38832b.gif" width="50%" height="30%">

```C++void APlayerCharacter::Equip()
{
	if (!GetCharacterMovement()->IsFalling() && !bRolling && !bEquiping && !bPressedRMouse && !bAttacking && !bDamaged && !bDead && CurrentWeapon && !bBlocking && !bDrinking)
	{
		bEquiping = true;

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
			...
		}
			
	}
}
```

##### 3.2 if player is combat mode
<img src="https://user-images.githubusercontent.com/74805177/100643449-64342680-337d-11eb-89cd-9cdc90772b08.gif" width="50%" height="30%">

```C++
void APlayerCharacter::Equip()
{
	if (!GetCharacterMovement()->IsFalling() && !bRolling && !bEquiping && !bPressedRMouse && !bAttacking && !bDamaged && !bDead && CurrentWeapon && !bBlocking && !bDrinking)
	{
		bEquiping = true;

		if (CurrentWeapon->EquipSound)
		{
			UGameplayStatics::PlaySound2D(this, CurrentWeapon->EquipSound);
		}
		
		if (CurrentCombatMode == ECombatMode::E_Normal_Mode)
		{
			...
		}
		else
		{
			ABAnim->PlayEquippingMontage();
			ABAnim->JumpToUnEquippingMontageSection();
		}
			
	}
}
```

* #### 4. Change weapon position
<img src="https://user-images.githubusercontent.com/74805177/100644116-3e5b5180-337e-11eb-9517-8fd2280fc298.png" width="50%" height="30%">
If the equipping montage reaches the changeweaponposition point, the position of the equipment is changed according to the situation.


## Drinking potion for hp healing
### Drinking potion system flow
* #### 1. Player "r" key click to Drinking the potion

* #### 2. Catch that input in SetupPlayerInputComponent method and call the DrinkingPotion()
```c++
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	...
	PlayerInputComponent->BindAction(TEXT("UsePotion"), EInputEvent::IE_Pressed, this, &APlayerCharacter::DrinkingPotion);
}
```

* #### 3. Ready of drinking
<img src="https://user-images.githubusercontent.com/74805177/100647008-36051580-3382-11eb-892e-481f7211eab3.gif" width="50%" height="30%">

##### 3.1 Execute DrinkingPotion()
```c++
void APlayerCharacter::DrinkingPotion()
{
	if (!bDrinking && CanMove(1.f) && !bEquiping)
	{
		PotionRecovery = MainPlayerController->RequestPotion();
		if (PotionRecovery > 0)
		{
			...
		}
	}
}
```
##### 3.2 Execute RequestPotion() for check the potion's remain and recovery amount
```c++
float AABPlayerController::RequestPotion()
{
	return HUDOverlay->CheckPotion();
}

float UHUDOverlay::CheckPotion()
{
	return QuickSlot->UsePotion();
}

float UQuickSlot::UsePotion()
{
	if (CurrentAmount > 0)
	{
		CurrentAmount--;
		AmountText->SetText(FText::FromString(FString::FromInt(CurrentAmount)));
		return BaseHealingAmount + (OnePorceHealing * CurrentForce);
	}
	else
	{
		return 0;
	}
}
```
##### 3.3 Return to DrinkingPotion() and run after
```c++
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
```

* #### 4. Play drinking montage
<img src="https://user-images.githubusercontent.com/74805177/100648616-867d7280-3384-11eb-9850-827e2496b919.png" width="50%" height="30%">

##### 4.1 When the montage reaches the healing notify point
```C++
void APlayerCharacter::PostInitializeComponents()
{
	...
	ABAnim->OnHealing.AddLambda([this]()->void {
		CharacterStat->IncreaseCurrentHealth(PotionRecovery);
		...
	});
	...
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
```
The player's actual hp is restored by executing the IncreaseCurrentHealth() method of the Character Start Component that the player character has.


## Inventory system
### Structure
<img src="https://user-images.githubusercontent.com/74805177/101167358-1cfbad80-367d-11eb-9a19-b99d16ff486c.png" width="50%" height="30%">

```c++
UCLASS()
class RPG_PJT_API UInventoryWindow : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

private:
	/**
	* 인벤토리창 슬롯 (Inventory)
	*/
	UPROPERTY()
	class UUniformGridPanel* Inventory;

	UPROPERTY()
	class UTextBlock* MoneyTxt;

	int32 CurrentMoney;

public:
	void InitInventory();

	bool StoredInEmptySlot(FName ItemNam, int32 Amount=1);

	void SetMoney(int32 NewMoney);
	void IncrementMoney(int32 IncMoney);
	void DecrementMoney(int32 DecMoney);

	bool CheckDecrementMoney(int32 DecMoney);

private:
	/**
	* 장비창 슬롯 (Equipment)
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InventorySlot", Meta = (AllowPrivateAccess = true))
	class UEquipmentSlot* HeadSlot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InventorySlot", Meta = (AllowPrivateAccess = true))
	UEquipmentSlot* TopSlot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InventorySlot", Meta = (AllowPrivateAccess = true))
	UEquipmentSlot* BotSlot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InventorySlot", Meta = (AllowPrivateAccess = true))
	UEquipmentSlot* WeaponSlot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InventorySlot", Meta = (AllowPrivateAccess = true))
	UEquipmentSlot* ShieldSlot;

private:
	/**
	* 캐릭터 스텟 창 (CharacterStat)
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InventorySlot", Meta = (AllowPrivateAccess = true))
	class ULevelSlot* LvSlot;

public:
	bool ChangeEquipItem(class UInventorySlot* NewItem);

	bool EquipItem(FName ItemName);
	bool UnEquipItem(EEquipmentSlot NewItemSlot);

	void IncreaseStatOfEquip(FItemData* ItemData);
	void DecreaseStatOfEquip(FItemData* ItemData);

	void UnEquipALLItemForSave(class UABSaveGame* SaveSlot);
	void DecreaseStatOfSaveSlot(FItemData* ItemData, UABSaveGame* SaveSlot);

	ULevelSlot* GetLvSlot() { return LvSlot; }

	bool PurchaceItem(FName ItemName);

	void SaveItems(class UABSaveGame* SaveSlot);
	void LoadItems(class UABSaveGame* LoadSlot);

	FORCEINLINE UEquipmentSlot* GetShieldSlot() { return ShieldSlot; }
	FORCEINLINE UEquipmentSlot* GetWeaponSlot() { return WeaponSlot; }
};

```
### 1. Inventory
<img src="https://user-images.githubusercontent.com/74805177/101235635-554acc80-370d-11eb-97bd-50dcedc6548c.png" width="50%" height="30%">

<img src="https://user-images.githubusercontent.com/74805177/101235510-8c6cae00-370c-11eb-97e5-31b2a75a1473.png" width="50%" height="30%">

#### 1.1 If a new item is added to the inventory
```c++
bool UInventoryWindow::StoredInEmptySlot(FName ItemName, int32 Amount)
{
	UABGameInstance* ABGameInstance = Cast<UABGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	FItemData* ItemData = ABGameInstance->GetItemData(ItemName);
	int32 MaxStack = ItemData->GetMaxStackSize();
	
	for (int i = 0; i < 32; i++)
	{
		if (Cast<UInventorySlot>(Inventory->GetChildAt(i))->GetItemName() == ItemName && ItemData->GetIsStackable())
		{
			if (Cast<UInventorySlot>(Inventory->GetChildAt(i))->GetItemAmount() + Amount <= MaxStack)
			{
				Cast<UInventorySlot>(Inventory->GetChildAt(i))->AddAmount(Amount);
				return true;
			}
			else
			{
				Amount -= (MaxStack - Cast<UInventorySlot>(Inventory->GetChildAt(i))->GetItemAmount());
				Cast<UInventorySlot>(Inventory->GetChildAt(i))->SetItemAmount(MaxStack);
			}
		}w
		else if (Cast<UInventorySlot>(Inventory->GetChildAt(i))->GetItemName() == FName("Default"))
		{
			Cast<UInventorySlot>(Inventory->GetChildAt(i))->SetItem(ItemName, Amount);
			return true;
		}
	}
	return false;
}
```

#### 1.2 If the player drops the item
<img src="https://user-images.githubusercontent.com/74805177/101281314-97f5cd00-3811-11eb-9541-707d92316be8.gif" width="50%" height="30%">

#### 1.2.1 Detecting item drag
```c++
void UInventorySlot::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);

	FVector2D DragWindowOffset = InGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());

	UDragWidget* oper = NewObject<UDragWidget>();
	oper->DefaultDragVisual = this;
	oper->WidgetToDrag = this;
	oper->DropWindowOffset = DragWindowOffset;
	oper->Pivot = EDragPivot::CenterCenter;
	OutOperation = oper;
}
```

#### 1.2.2 Item drop rate detection and create an item pot with items droped on the floor
```c++
bool UHUDOverlay::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);

	...
	
	else if (Cast<UInventorySlot>(Cast<UDragWidget>(InOperation)->WidgetToDrag) && !Cast<AABPlayerController>(GetOwningPlayer())->IsOpenTradeWindow() && !Cast<APlayerCharacter>(GetOwningPlayerPawn())->GetCharacterMovement()->IsFalling())
	{
		if (Cast<APlayerCharacter>(GetOwningPlayerPawn())->GetTargetItemPot())
		{
			...
		}
		else
		{
			FVector CharacterLocation = Cast<APlayerCharacter>(GetOwningPlayerPawn())->GetCharacterMovement()->GetActorLocation();
			AItemPot* NewItemPot = GetWorld()->SpawnActor<AItemPot>(FVector(CharacterLocation.X, CharacterLocation.Y, CharacterLocation.Z-90), FRotator::ZeroRotator);
			NewItemPot->AddItem(Cast<UInventorySlot>(Cast<UDragWidget>(InOperation)->WidgetToDrag)->GetItemName(), Cast<UInventorySlot>(Cast<UDragWidget>(InOperation)->WidgetToDrag)->GetItemAmount());
			Cast<UInventorySlot>(Cast<UDragWidget>(InOperation)->WidgetToDrag)->SetItem(FName("Default"));
		}
	}

	return true;
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
```

#### 1.2.3 Player interacts with item pot to get item
```c++
void APlayerCharacter::PressedInteractionKey()
{
	if (!GetCharacterMovement()->IsFalling() && !bDead && !bDrinking)
	{
		bPressedInteractionKey = true;
		if (TargetItemPot && !(MainPlayerController->IsOpenLootWindow()))
		{
			MainPlayerController->OpenLootWindow(TargetItemPot);
		}
		...
	}
}

void AABPlayerController::OpenLootWindow(AItemPot* TargetItemPot)
{
	HUDOverlay->OpenLootWindow(TargetItemPot);
	bLootWindow = true;
}

void UHUDOverlay::OpenLootWindow(class AItemPot* TargetItemPot) 
{
	LootWindow->InitInventory(TargetItemPot);
	LootWindow->SetVisibility(ESlateVisibility::Visible);
}

void ULootWindow::InitInventory(AItemPot* TargetItemPot)
{
	if (CurrentItemPot != TargetItemPot)
	{
		TopItemIndex = 0;
		CurrentItemPot = TargetItemPot;
		
		for (auto& ItemInfo : TargetItemPot->GetItemPotInventory())
		{
			ULootSlot* ItemSlot = CreateWidget<ULootSlot>(GetOwningPlayer(), LootSlotAsset);
			Inventory_Box->AddChildToGrid(ItemSlot, TopItemIndex, 0);
			TopItemIndex++;
			ItemSlot->SetItem(ItemInfo.Key, ItemInfo.Value);
		}
	}
}
```

#### 1.2.4 To get the item of the item pot, Right click on the item
```c++
FReply ULootSlot::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FEventReply reply;
	reply.NativeReply = Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);

	if (InMouseEvent.IsMouseButtonDown(EKeys::RightMouseButton) == true)
	{
		if (Cast<AABPlayerController>(GetOwningPlayer())->GetHUDOverlay()->GetInventoryWindow()->StoredInEmptySlot(IName, IAmount))
		{
			int32 Row = Cast<UGridPanel>(GetParent())->GetChildIndex(this);
			Cast<APlayerCharacter>(GetOwningPlayerPawn())->GetTargetItemPot()->RemoveItem(Row);
			RemoveFromParent();
		}
	}

	return reply.NativeReply;
}
```

### 2. Equipment
<img src="https://user-images.githubusercontent.com/74805177/101261650-40595200-377c-11eb-8f0e-5549b0bffeda.png" width="30%" height="30%">

<img src="https://user-images.githubusercontent.com/74805177/101261835-4a2f8500-377d-11eb-8c90-5c8f86759b27.png" width="50%" height="30%">

#### 2.1 If the player equips or unequips an item
<img src="https://user-images.githubusercontent.com/74805177/101282886-e9568a00-381a-11eb-9546-19377e7c01c9.gif" width="50%" height="30%">

#### 2.1.1 Equip by right-clicking an equipment item in the inventory
```c++
FReply UInventorySlot::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	...	
	if (InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton) == true && ItemType != EItemType::None)
	{
		...
	}
	else if (InMouseEvent.IsMouseButtonDown(EKeys::RightMouseButton) == true && !(Cast<APlayerCharacter>(GetOwningPlayerPawn())->IsEquipping()) && SlotType == ESlotType::Inventory)
	{
		switch (ItemData->GetItemType())
		{
		case EItemType::Consumable:
			...
			break;
		case EItemType::Equipment:
			UseEquipTypeItem();
			break;
		default:
			break;
		}
	}

	return reply.NativeReply;
}

void UInventorySlot::UseEquipTypeItem()
{
	if (Cast<APlayerCharacter>(GetOwningPlayerPawn())->GetCombatMode() == ECombatMode::E_Normal_Mode)
	{
		Cast<AABPlayerController>(GetOwningPlayer())->EquipItem(this);
	}
}

bool AABPlayerController::EquipItem(UInventorySlot* EquippedItem)
{
	UABGameInstance* ABGameInstance = Cast<UABGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	FItemData* NewItemData = ABGameInstance->GetItemData(EquippedItem->GetItemName());
	if (HUDOverlay->GetInventoryWindow()->ChangeEquipItem(EquippedItem))
	{
		switch (NewItemData->GetEquipmentSlot())
		{
		case EEquipmentSlot::Head:
			Cast<APlayerCharacter>(GetPawn())->EquipHead(NewItemData->GetEquipMesh());
			break;
		case EEquipmentSlot::Chest:
			Cast<APlayerCharacter>(GetPawn())->EquipTop(NewItemData->GetEquipMesh());
			break;
		case EEquipmentSlot::Legs:
			Cast<APlayerCharacter>(GetPawn())->EquipBot(NewItemData->GetEquipMesh());
			break;
		case EEquipmentSlot::RightHand:
			if (NewItemData->GetWeaponType() == EWeaponType::OneHanded)
			{
				Cast<APlayerCharacter>(GetPawn())->EquipOneHandedWeapon(NewItemData->GetWeaponBP());
			}
			else if (NewItemData->GetWeaponType() == EWeaponType::TwoHanded)
			{
				Cast<APlayerCharacter>(GetPawn())->EquipTwoHandedWeapon(NewItemData->GetWeaponBP());
			}
			break;
		case EEquipmentSlot::LeftHand:
			Cast<APlayerCharacter>(GetPawn())->EquipShield(NewItemData->GetShieldBP());
			break;
		}
		return true;
	}
	else
	{
		return false;
	}
}

bool UInventoryWindow::ChangeEquipItem(class UInventorySlot* NewItem)
{
	FName CurrentItemName;

	switch (NewItem->GetEquipSlot())
	{
	case EEquipmentSlot::Head:
		Cast<AABPlayerController>(GetOwningPlayer())->UnEquipItem(HeadSlot->GetItemName());
		EquipItem(NewItem->GetItemName());
		NewItem->SetItem(FName("Default"));
		break;
	case EEquipmentSlot::Chest:
		Cast<AABPlayerController>(GetOwningPlayer())->UnEquipItem(TopSlot->GetItemName());
		EquipItem(NewItem->GetItemName());
		NewItem->SetItem(FName("Default"));
		break;
	case EEquipmentSlot::Legs:
		Cast<AABPlayerController>(GetOwningPlayer())->UnEquipItem(BotSlot->GetItemName());
		EquipItem(NewItem->GetItemName());
		NewItem->SetItem(FName("Default"));
		break;
	case EEquipmentSlot::RightHand:
		Cast<AABPlayerController>(GetOwningPlayer())->UnEquipItem(WeaponSlot->GetItemName());
		if(!EquipItem(NewItem->GetItemName())) return false;
		NewItem->SetItem(FName("Default"));
		break;
	case EEquipmentSlot::LeftHand:
		Cast<AABPlayerController>(GetOwningPlayer())->UnEquipItem(ShieldSlot->GetItemName());
		if(!EquipItem(NewItem->GetItemName())) return false;
		NewItem->SetItem(FName("Default"));
		break;
	}

	return true;
}

bool UInventoryWindow::UnEquipItem(EEquipmentSlot NewItemSlot)
{
	UABGameInstance* ABGameInstance = Cast<UABGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	FItemData* ItemData = nullptr;

	switch (NewItemSlot)
	{
	case EEquipmentSlot::Head:
		if (HeadSlot->GetItemName() == FName("Default")) return true;
		ItemData = ABGameInstance->GetItemData(HeadSlot->GetItemName());
		if (StoredInEmptySlot(HeadSlot->GetItemName()))
		{
			HeadSlot->SetItem(FName("Default"));
		}
		else
		{
			return false;
		}
		break;
	case EEquipmentSlot::Chest:
		if (TopSlot->GetItemName() == FName("Default")) return true;
		ItemData = ABGameInstance->GetItemData(TopSlot->GetItemName());
		if (StoredInEmptySlot(TopSlot->GetItemName()))
		{
			TopSlot->SetItem(FName("Default"));
		}
		else
		{
			return false;
		}
		break;
	case EEquipmentSlot::Legs:
		if (BotSlot->GetItemName() == FName("Default"))	return true;
		ItemData = ABGameInstance->GetItemData(BotSlot->GetItemName());
		if (StoredInEmptySlot(BotSlot->GetItemName()))
		{
			BotSlot->SetItem(FName("Default"));
		}
		else
		{
			return false;
		}
		break;
	case EEquipmentSlot::RightHand:
		if (WeaponSlot->GetItemName() == FName("Default"))	return true;
		ItemData = ABGameInstance->GetItemData(WeaponSlot->GetItemName());
		if (StoredInEmptySlot(WeaponSlot->GetItemName()))
		{
			WeaponSlot->SetItem(FName("Default"));
		}
		else
		{
			return false;
		}
		break;
	case EEquipmentSlot::LeftHand:
		if (ShieldSlot->GetItemName() == FName("Default"))	return true;
		ItemData = ABGameInstance->GetItemData(ShieldSlot->GetItemName());
		if (StoredInEmptySlot(ShieldSlot->GetItemName()))
		{
			ShieldSlot->SetItem(FName("Default"));
		}
		else
		{
			return false;
		}
		break;
	}

	DecreaseStatOfEquip(ItemData);
	return true;
}

bool UInventoryWindow::EquipItem(FName ItemName)
{
	UABGameInstance* ABGameInstance = Cast<UABGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	FItemData* ItemData = ABGameInstance->GetItemData(ItemName);
	
	switch (ItemData->GetEquipmentSlot())
	{
	case EEquipmentSlot::Head:
		HeadSlot->SetItem(ItemName);
		break;
	case EEquipmentSlot::Chest:
		TopSlot->SetItem(ItemName);
		break;
	case EEquipmentSlot::Legs:
		BotSlot->SetItem(ItemName);
		break;
	case EEquipmentSlot::RightHand:
		if (ItemData->GetWeaponType() == EWeaponType::TwoHanded)
		{
			if (ShieldSlot->GetItemName() != FName("Default"))
			{
				if (!Cast<AABPlayerController>(GetOwningPlayer())->UnEquipItem(ShieldSlot->GetItemName())) return false;
			}
		}
		WeaponSlot->SetItem(ItemName);
		break;
	case EEquipmentSlot::LeftHand:
		if (WeaponSlot->GetWeaponType() == EWeaponType::TwoHanded)
		{
			if (WeaponSlot->GetItemName() != FName("Default"))
			{
				if (!Cast<AABPlayerController>(GetOwningPlayer())->UnEquipItem(WeaponSlot->GetItemName())) return false;
			}
		}
		ShieldSlot->SetItem(ItemName);
		break;
	}

	IncreaseStatOfEquip(ItemData);
	return true;
}
```

#### 2.1.1 Right-click the item in the equipment slot you want to equip
```c++
FReply UEquipmentSlot::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FEventReply reply;
	reply.NativeReply = Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
	
	if (ItemData->GetItemType() != EItemType::None)
	{
		if (InMouseEvent.IsMouseButtonDown(EKeys::RightMouseButton) == true)
		{
			if (Cast<APlayerCharacter>(GetOwningPlayerPawn())->GetCombatMode() == ECombatMode::E_Normal_Mode && !(Cast<APlayerCharacter>(GetOwningPlayerPawn())->IsEquipping()))
			{
				Cast<AABPlayerController>(GetOwningPlayer())->UnEquipItem(ItemData->GetName());
			}
		}
	}
	return reply.NativeReply;
}
```

### 3. CharacterStat
<img src="https://user-images.githubusercontent.com/74805177/101283558-78b16c80-381e-11eb-8191-66c4a589913c.gif" width="50%" height="30%">

#### 3.1 Increase the character's stats using stat points
Players can earn stat points each time they level up, and use these stat points to increase their stats.

#### 3.1.1 When clicking a button to increase each stat, the appropriate method is called.
```c++
void ULevelSlot::ClickedHPPoint()
{
	CurrentCharacterStat->UseStatPoint(FString("HP"));
}

void ULevelSlot::ClickedSPPoint()
{
	CurrentCharacterStat->UseStatPoint(FString("SP"));
}

void ULevelSlot::ClickedATKPoint()
{
	CurrentCharacterStat->UseStatPoint(FString("ATK"));
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

void UCharacterStatComponent::IncreaseHP(int32 IncHP)
{
	Hp += IncHP;
	OnHPStatChanged.Broadcast();
	IncreaseMaxHealth(OnePointHp * IncHP);
}

void UCharacterStatComponent::IncreaseMaxHealth(float IncHealth)
{ 
	MaxHealth += IncHealth;
	OnMaxHPChanged.Broadcast();
}

void UCharacterStatComponent::IncreaseSP(int32 IncSP)
{
	Sp += IncSP;
	OnSPStatChanged.Broadcast();
	IncreaseMaxStamina(OnePointSp * IncSP);
}

void UCharacterStatComponent::IncreaseMaxStamina(float IncStamina)
{
	MaxStamina += IncStamina;
	OnMaxSPChanged.Broadcast();
}

void UCharacterStatComponent::IncreaseATK(int32 IncATK)
{
	Atk += IncATK;
	OnATKStatChanged.Broadcast();
	IncreaseAttack(OnePointAtk * IncATK);
}

void UCharacterStatComponent::IncreaseAttack(int32 IncAttack)
{
	Attack += IncAttack;
	OnAttackChanged.Broadcast();
}
```

#### 3.1.2 The updated stat information broadcasted by characterstatcomponent is notified by the stat widget and the changed information is displayed to the player.
```c++
	CurrentCharacterStat->OnStatPointChanged.AddLambda([this]()->void {
		if (CurrentCharacterStat->GetStatPoint() == 0)
		{
			HPPointBt->SetVisibility(ESlateVisibility::Hidden);
			SPPointBt->SetVisibility(ESlateVisibility::Hidden);
			ATKPointBt->SetVisibility(ESlateVisibility::Hidden);
		}
		else if (HPPointBt->GetVisibility() == ESlateVisibility::Hidden)
		{
			HPPointBt->SetVisibility(ESlateVisibility::Visible);
			SPPointBt->SetVisibility(ESlateVisibility::Visible);
			ATKPointBt->SetVisibility(ESlateVisibility::Visible);
		}
		StatPoint->SetText(FText::FromString(FString::FromInt(CurrentCharacterStat->GetStatPoint())));
	});
	
	CurrentCharacterStat->OnMaxHPChanged.AddLambda([this]()->void {
		TotalHP->SetText(FText::FromString(FString::FromInt(CurrentCharacterStat->GetMHealth())));
	});

	CurrentCharacterStat->OnMaxSPChanged.AddLambda([this]()->void {
		TotalSP->SetText(FText::FromString(FString::FromInt(CurrentCharacterStat->GetMStamina())));
	});

	CurrentCharacterStat->OnAttackChanged.AddLambda([this]()->void {
		TotalATK->SetText(FText::FromString(FString::FromInt(CurrentCharacterStat->GetAttack())));
	});

	CurrentCharacterStat->OnDefenseChanged.AddLambda([this]()->void {
		TotalDEF->SetText(FText::FromString(FString::FromInt(CurrentCharacterStat->GetDefense())));
	});

	CurrentCharacterStat->OnHPStatChanged.AddLambda([this]()->void {
		HPPoint->SetText(FText::FromString(FString::FromInt(CurrentCharacterStat->GetHpStat())));
	});

	CurrentCharacterStat->OnSPStatChanged.AddLambda([this]()->void {
		SPPoint->SetText(FText::FromString(FString::FromInt(CurrentCharacterStat->GetSpStat())));
	});

	CurrentCharacterStat->OnATKStatChanged.AddLambda([this]()->void {
		ATKPoint->SetText(FText::FromString(FString::FromInt(CurrentCharacterStat->GetAtkStat())));
	});
```
