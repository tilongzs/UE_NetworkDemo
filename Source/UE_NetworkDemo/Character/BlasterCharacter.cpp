#include "BlasterCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/Controller.h"
#include "Camera/CameraComponent.h"
#include "../Plugins/EnhancedInput/Source/EnhancedInput/Public/EnhancedInputSubsystems.h"
#include "../Plugins/EnhancedInput/Source/EnhancedInput/Public/EnhancedInputComponent.h"
#include "../Common/Utils.h"
#include "Components/CapsuleComponent.h"
#include "../Weapon/Weapon.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Net/UnrealNetwork.h"

#include <thread>
#include <chrono>
#include "Kismet/GameplayStatics.h"
#include "../LobbyGameMode.h"
using namespace std;
using namespace chrono;

#define FIRE_TRACE_LENGTH 8000

ABlasterCharacter::ABlasterCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	_cameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	_cameraBoom->SetupAttachment(GetMesh());
	_cameraBoom->TargetArmLength = 600.f;
	_cameraBoom->bUsePawnControlRotation = true; // 使用Pawn身上的PlayerController控制弹簧臂
	_followCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	_followCamera->SetupAttachment(_cameraBoom, USpringArmComponent::SocketName);
	_followCamera->bUsePawnControlRotation = false;
	bUseControllerRotationYaw = false; // 身体不跟随控制器（镜头）转向
	GetCharacterMovement()->bOrientRotationToMovement = true; // 身体随运动方向自动转向
	GetCharacterMovement()->NavAgentProps.bCanCrouch = true; // 允许蹲下
	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &ABlasterCharacter::OnCapsuleBeginOverlap);
	GetCapsuleComponent()->OnComponentEndOverlap.AddDynamic(this, &ABlasterCharacter::OnCapsuleEndOverlap);
	NetUpdateFrequency = 66.f;
	MinNetUpdateFrequency = 33.f;
	OnTakePointDamage.AddDynamic(this, &ABlasterCharacter::OnDlgTakePointDamage);
}

void ABlasterCharacter::BeginPlay()
{
	Super::BeginPlay();

	_defaultFOV = _followCamera->FieldOfView;
	_currentFOV = _defaultFOV;

	/*
		在网络游戏中，PlayerState的初始化和复制需要一些时间。因此，在BeginPlay函数中调用GetPlayerState可能会在PlayerState初始化之前。
		可以通过重写OnRep_PlayerState函数来得知PlayerState的初始化和复制完成。
	*/
	APlayerState* playerState = GetPlayerState();
	if (playerState)
	{
		//Log(FString::Printf(TEXT("玩家名 %s"), *playerState->GetPlayerName()));
	}
}

void ABlasterCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABlasterCharacter, _equippedWeapon);
	DOREPLIFETIME(ABlasterCharacter, _isAiming);
	DOREPLIFETIME(ABlasterCharacter, _currentHealth);
}

void ABlasterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateZoom(DeltaTime);
}

void ABlasterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	// 参考教程 https://dev.epicgames.com/community/learning/tutorials/aqrD/unreal-engine-enhanced-input-binding-with-gameplay-tags-c

	if (APlayerController* pc = CastChecked<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(pc->GetLocalPlayer()))
		{
			subsystem->AddMappingContext(InputMappingContext, 100);
		}
	}

	if (UEnhancedInputComponent* inputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (IA_MoveForward)
		{
			inputComponent->BindAction(IA_MoveForward, ETriggerEvent::Triggered, this, &ABlasterCharacter::OnActionMoveForward);
		}

		if (IA_MoveRight)
		{
			inputComponent->BindAction(IA_MoveRight, ETriggerEvent::Triggered, this, &ABlasterCharacter::OnActionMoveRight);
		}

		if (IA_Turn)
		{
			inputComponent->BindAction(IA_Turn, ETriggerEvent::Triggered, this, &ABlasterCharacter::OnActionTurn);
		}

		if (IA_LookUp)
		{
			inputComponent->BindAction(IA_LookUp, ETriggerEvent::Triggered, this, &ABlasterCharacter::OnActionLookUp);
		}

		if (IA_Jump)
		{
			inputComponent->BindAction(IA_Jump, ETriggerEvent::Started, this, &ABlasterCharacter::OnActionJump);
		}

		if (IA_PickUp)
		{
			inputComponent->BindAction(IA_PickUp, ETriggerEvent::Started, this, &ABlasterCharacter::OnActionPickUp);
		}

		if (IA_Drop)
		{
			inputComponent->BindAction(IA_Drop, ETriggerEvent::Started, this, &ABlasterCharacter::OnActionDrop);
		}

		if (IA_Crouch)
		{
			inputComponent->BindAction(IA_Crouch, ETriggerEvent::Started, this, &ABlasterCharacter::OnActionCrouch);
		}

		if (IA_Aiming)
		{
			inputComponent->BindAction(IA_Aiming, ETriggerEvent::Started, this, &ABlasterCharacter::OnActionAimingStart);
			inputComponent->BindAction(IA_Aiming, ETriggerEvent::Completed, this, &ABlasterCharacter::OnActionAimingComplete);
		}

		if (IA_Fire)
		{
			inputComponent->BindAction(IA_Fire, ETriggerEvent::Started, this, &ABlasterCharacter::OnActionFireStart);
			inputComponent->BindAction(IA_Fire, ETriggerEvent::Completed, this, &ABlasterCharacter::OnActionFireComplete);
		}
	}
}

void ABlasterCharacter::OnRep_PlayerState()
{
	//Log("ABlasterCharacter::OnRep_PlayerState");
	// 用于客户端；会晚于BeginPlay
	EventPlayerStateUpdate(GetPlayerState());
}

void ABlasterCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	//Log("ABlasterCharacter::PossessedBy");

	// 用于服务端；会早于BeginPlay
	EventPlayerStateUpdate(NewController->PlayerState);
}

class USkeletalMeshComponent* ABlasterCharacter::GetWeaponMesh()
{
	if (_equippedWeapon)
	{
		return _equippedWeapon->GetMesh();
	}

	return nullptr;
}

void ABlasterCharacter::OnActionMoveForward(const FInputActionValue& inputActionValue)
{
	float inputValue = inputActionValue.Get<float>();
	//Log(FString::Printf(TEXT("ActionMoveForward value:%f"), inputValue));
	if (Controller && inputValue != 0)
	{
		const FRotator controllerYawRotator(0, Controller->GetControlRotation().Yaw, 0);		
		const FVector direction = controllerYawRotator.RotateVector(FVector::ForwardVector); //const FVector direction(FRotationMatrix(controllerYawRotator).GetUnitAxis(EAxis::X));
		AddMovementInput(direction, inputValue);
	}
}

void ABlasterCharacter:: OnActionMoveRight(const FInputActionValue& inputActionValue)
{
	float inputValue = inputActionValue.Get<float>();
	//Log(FString::Printf(TEXT("ActionMoveForward value:%f"), inputValue));
	if (Controller && inputValue != 0)
	{
		const FRotator controllerYawRotator(0, Controller->GetControlRotation().Yaw, 0);
		const FVector direction = controllerYawRotator.RotateVector(FVector::RightVector);
		AddMovementInput(direction, inputValue);
	}
}

void ABlasterCharacter::OnActionJump(const FInputActionValue& inputActionValue)
{
	if (!GetCharacterMovement()->IsFalling())
	{
		if (bIsCrouched)
		{
			UnCrouch();
		}
		else
		{
			Jump();
		}
	}
}

void ABlasterCharacter::OnActionLookUp(const FInputActionValue& inputActionValue)
{
	float inputValue = inputActionValue.Get<float>();
	AddControllerPitchInput(inputValue);
}

void ABlasterCharacter::OnActionTurn(const FInputActionValue& inputActionValue)
{
	float inputValue = inputActionValue.Get<float>();
	AddControllerYawInput(inputValue);
}

void ABlasterCharacter::OnActionPickUp(const FInputActionValue& inputActionValue)
{
	PickUp();
}

void ABlasterCharacter::OnActionDrop(const FInputActionValue& inputActionValue)
{
	Drop();
}

void ABlasterCharacter::OnActionCrouch(const FInputActionValue& inputActionValue)
{
	if (!GetCharacterMovement()->IsFalling())
	{
		if (bIsCrouched)
		{
			UnCrouch();
		}
		else
		{
			Crouch();
		}
	}
}

void ABlasterCharacter::OnActionAimingStart(const FInputActionValue& inputActionValue)
{
	if (_equippedWeapon)
	{
		Aim(true);
	}
}

void ABlasterCharacter::OnActionAimingComplete(const FInputActionValue& inputActionValue)
{
	Aim(false);
}

void ABlasterCharacter::OnActionFireStart(const FInputActionValue& inputActionValue)
{
	if (_equippedWeapon && !_fireTimer.IsValid())
	{
		GetWorldTimerManager().SetTimer(_fireTimer, this, &ABlasterCharacter::OnTimerFire, _equippedWeapon->GetFireDelay(), true, 0);
	}
}

void ABlasterCharacter::OnActionFireComplete(const FInputActionValue& inputActionValue)
{
	GetWorldTimerManager().ClearTimer(_fireTimer);

	FVector_NetQuantize fireImpactPoint;
	Server_Fire(true, fireImpactPoint);
}

void ABlasterCharacter::OnCapsuleBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	_overlapActors.Add(OtherActor);
}

void ABlasterCharacter::OnCapsuleEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	_overlapActors.Remove(OtherActor);
}

void ABlasterCharacter::PickUp()
{
	AWeapon* weapon(nullptr);
	for (auto& actor : _overlapActors)
	{
		weapon = Cast<AWeapon>(actor);
		if (weapon)
		{
			// 丢弃当前武器	
			Drop();

			// 装备该武器
			const USkeletalMeshSocket* rightHandSocket = GetMesh()->GetSocketByName(FName("RightHandSocket"));
			if (rightHandSocket)
			{
				_equippedWeapon = weapon;
				_equippedWeapon->SetState(EWeaponState::Equipped);
				_equippedWeapon->SetOwner(this);
				rightHandSocket->AttachActor(_equippedWeapon, GetMesh());

// 				bUseControllerRotationYaw = true; // 身体跟随控制器（镜头）转向
// 				GetCharacterMovement()->bOrientRotationToMovement = false; // 身体不跟随运动方向自动转向

				_dlgPickup.Broadcast(true);
			}
			else
			{
				LogError(TEXT("装备该武器 RightHandSocket不存在"));
			}

			break;
		}
	}

	if (!HasAuthority())
	{
		Server_PickUp();
	}
}

void ABlasterCharacter::Server_PickUp_Implementation()
{
	PickUp();
}

void ABlasterCharacter::Drop()
{
	if (!_equippedWeapon)
	{
		return;		
	}

	// 丢弃当前武器		
	FDetachmentTransformRules rules(EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, true);
	_equippedWeapon->DetachFromActor(rules);
	_equippedWeapon->SetOwner(nullptr);
	_equippedWeapon->SetState(EWeaponState::Dropped);
	_equippedWeapon = nullptr;

	bUseControllerRotationYaw = false; // 身体不跟随控制器（镜头）转向
	GetCharacterMovement()->bOrientRotationToMovement = true; // 身体跟随运动方向自动转向

	_dlgPickup.Broadcast(false);

	if (!HasAuthority())
	{
		Server_Drop();
	}
}

void ABlasterCharacter::Server_Drop_Implementation()
{
	Drop();
}

void ABlasterCharacter::Aim(bool isAiming)
{
	_isAiming = isAiming;

	if (!HasAuthority())
	{
		Server_Aim(isAiming);
	}
}

void ABlasterCharacter::Server_Aim_Implementation(bool isAiming)
{
	Aim(isAiming);
}

void ABlasterCharacter::UpdateZoom(float DeltaTime)
{
	if (!IsLocallyControlled())
	{
		return;
	}

	if (_isAiming)
	{
		_currentFOV = FMath::FInterpTo(_currentFOV, _equippedWeapon->GetZoomedFOV(), DeltaTime, _equippedWeapon->GetZoomInterpSpeed());
	}
	else
	{
		_currentFOV = FMath::FInterpTo(_currentFOV, _defaultFOV, DeltaTime, _zoomInterpSpeed);
	}
	_followCamera->SetFieldOfView(_currentFOV);
}

void ABlasterCharacter::Server_Fire_Implementation(bool isStop, const FVector_NetQuantize& fireImpactPoint)
{
	if (isStop)
	{
		GetWorldTimerManager().ClearTimer(_fireTimer);
	}
	else
	{
		_fireImpactPoint = fireImpactPoint;
		if (!_fireTimer.IsValid())
		{
			GetWorldTimerManager().SetTimer(_fireTimer, this, &ABlasterCharacter::OnTimerFire, _equippedWeapon->GetFireDelay(), true, 0);
		}
	}
}

void ABlasterCharacter::OnTimerFire()
{
	if (IsLocallyControlled())
	{
		FVector	fireImpactPoint;
		TraceUnderCrosshairs(fireImpactPoint);

		Server_Fire(false, fireImpactPoint);
	}

	if (HasAuthority())
	{
		Multicast_Fire(_fireImpactPoint);
	}
}

void ABlasterCharacter::Multicast_Fire_Implementation(const FVector_NetQuantize& fireImpactPoint)
{
	if (_equippedWeapon)
	{
		_equippedWeapon->Fire(fireImpactPoint);
	}
}

void ABlasterCharacter::TraceUnderCrosshairs(FVector& fireImpactPoint)
{
	if (_equippedWeapon && IsLocallyControlled())
	{
		if (GEngine && GEngine->GameViewportForWorld(GetWorld()))
		{
			FVector2D viewPortSize;
			GEngine->GameViewportForWorld(GetWorld())->GetViewportSize(viewPortSize);

			FVector2D crosshairScreenLocation(viewPortSize.X / 2, viewPortSize.Y / 2);
			FVector crosshairWorldLocation;
			FVector crosshairWorldDirection;
			bool ret = UGameplayStatics::DeprojectScreenToWorld(UGameplayStatics::GetPlayerController(this, 0), crosshairScreenLocation, crosshairWorldLocation, crosshairWorldDirection);
			if (ret)
			{
				float distanceToCharacter = (crosshairWorldLocation - GetActorLocation()).Size() + 50;
				crosshairWorldLocation += (crosshairWorldDirection * distanceToCharacter); // 射线的起始位置设置在角色前，而不是摄像机位置
				//DrawDebugSphere(GetWorld(), crosshairWorldLocation, 15.f, 12, FColor::Red, true);
				FVector traceEnd = crosshairWorldLocation + crosshairWorldDirection * FIRE_TRACE_LENGTH;
				FHitResult hitResult;
				//DrawDebugLine(GetWorld(), crosshairWorldLocation, traceEnd, FColor::Yellow, false, 3);
				ret = GetWorld()->LineTraceSingleByChannel(hitResult, crosshairWorldLocation, traceEnd, ECollisionChannel::ECC_Visibility);
				if (ret)
				{
					//DrawDebugSphere(GetWorld(), crosshairWorldLocation, 0.3, 8, FColor::Green);
					if (hitResult.bBlockingHit)
					{
						fireImpactPoint = hitResult.ImpactPoint;
						//DrawDebugLine(GetWorld(), crosshairWorldLocation, hitResult.Location, FColor::Red, false, 3);
					}
					else
					{
						fireImpactPoint = traceEnd;
					}
				}
				else
				{
					fireImpactPoint = traceEnd;
					//LogWarning(TEXT("LineTraceSingleByChannel失败"));
				}
			}
			else
			{
				LogWarning(TEXT("DeprojectScreenToWorld失败"));
			}
		}
	}
}

void ABlasterCharacter::OnDlgTakePointDamage(AActor* DamagedActor, float Damage, class AController* InstigatedBy, FVector HitLocation, class UPrimitiveComponent* FHitComponent, FName BoneName, FVector ShotFromDirection, const class UDamageType* DamageType, AActor* DamageCauser)
{
	_currentHealth -= Damage;

	if (_currentHealth <= 0)
	{
		Drop();

		ALobbyGameMode* gamemode = GetWorld()->GetAuthGameMode<ALobbyGameMode>();
		if (gamemode)
		{
			gamemode->OnCharacterKilled(this, InstigatedBy->GetPawn());
		}
	}
}

