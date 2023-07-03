﻿#include "BlacterCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "../Plugins/EnhancedInput/Source/EnhancedInput/Public/EnhancedInputSubsystems.h"
#include "../Plugins/EnhancedInput/Source/EnhancedInput/Public/EnhancedInputComponent.h"
#include "../Common/Utils.h"

ABlacterCharacter::ABlacterCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	_cameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	_cameraBoom->SetupAttachment(GetMesh());
	_cameraBoom->TargetArmLength = 600.f;
	_cameraBoom->bUsePawnControlRotation = true; // 使用Pawn身上的PlayerController控制弹簧臂
	_followCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	_followCamera->SetupAttachment(_cameraBoom, USpringArmComponent::SocketName);
	_followCamera->bUsePawnControlRotation = false;
}

void ABlacterCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABlacterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABlacterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
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
			inputComponent->BindAction(IA_MoveForward, ETriggerEvent::Triggered, this, &ABlacterCharacter::OnActionMoveForward);
		}

		if (IA_MoveRight)
		{
			inputComponent->BindAction(IA_MoveRight, ETriggerEvent::Triggered, this, &ABlacterCharacter::OnActionMoveRight);
		}

		if (IA_Turn)
		{
			inputComponent->BindAction(IA_Turn, ETriggerEvent::Triggered, this, &ABlacterCharacter::OnActionTurn);
		}

		if (IA_LookUp)
		{
			inputComponent->BindAction(IA_LookUp, ETriggerEvent::Triggered, this, &ABlacterCharacter::OnActionLookUp);
		}

		if (IA_Jump)
		{
			inputComponent->BindAction(IA_Jump, ETriggerEvent::Started, this, &ABlacterCharacter::OnActionJump);
		}
	}
}

void ABlacterCharacter::OnActionMoveForward(const FInputActionValue& inputActionValue)
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

void ABlacterCharacter:: OnActionMoveRight(const FInputActionValue& inputActionValue)
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

void ABlacterCharacter::OnActionJump(const FInputActionValue& inputActionValue)
{
	Jump();
}

void ABlacterCharacter::OnActionLookUp(const FInputActionValue& inputActionValue)
{
	float inputValue = inputActionValue.Get<float>();
	AddControllerPitchInput(inputValue);
}

void ABlacterCharacter::OnActionTurn(const FInputActionValue& inputActionValue)
{
	float inputValue = inputActionValue.Get<float>();
	AddControllerYawInput(inputValue);
}




