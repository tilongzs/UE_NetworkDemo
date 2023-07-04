﻿#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "../Plugins/EnhancedInput/Source/EnhancedInput/Public/inputActionValue.h"
#include "BlacterCharacter.generated.h"

UCLASS()
class UE_NETWORKDEMO_API ABlacterCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ABlacterCharacter();

	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
protected:
	UPROPERTY(VisibleAnywhere, Category = "Camera")
		class USpringArmComponent* _cameraBoom;
	UPROPERTY(VisibleAnywhere, Category = "Camera")
		class UCameraComponent* _followCamera;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="EnhancedInput")
		class UInputMappingContext* InputMappingContext;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "EnhancedInput|Action")
		class UInputAction* IA_MoveForward;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "EnhancedInput|Action")
		class UInputAction* IA_MoveRight;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "EnhancedInput|Action")
		class UInputAction* IA_Turn;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "EnhancedInput|Action")
		class UInputAction* IA_LookUp;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "EnhancedInput|Action")
		class UInputAction* IA_Jump;

	virtual void BeginPlay() override;

	UFUNCTION()
	void OnActionMoveForward(const FInputActionValue& inputActionValue);
	UFUNCTION()
	void OnActionMoveRight(const FInputActionValue& inputActionValue);
	UFUNCTION()
	void OnActionJump(const FInputActionValue& inputActionValue);
	UFUNCTION()
	void OnActionLookUp(const FInputActionValue& inputActionValue);
	UFUNCTION()
	void OnActionTurn(const FInputActionValue& inputActionValue);
};