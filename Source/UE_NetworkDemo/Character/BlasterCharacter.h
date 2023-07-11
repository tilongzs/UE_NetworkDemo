#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "../Plugins/EnhancedInput/Source/EnhancedInput/Public/inputActionValue.h"
#include "BlasterCharacter.generated.h"

UCLASS()
class UE_NETWORKDEMO_API ABlasterCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ABlasterCharacter();

	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void OnRep_PlayerState() override;
	virtual void PossessedBy(class AController* NewController) override;

	bool IsWeaponEquipped();
protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

	UPROPERTY(VisibleAnywhere, Category = "Camera")
		class USpringArmComponent* _cameraBoom = nullptr;
	UPROPERTY(VisibleAnywhere, Category = "Camera")
		class UCameraComponent* _followCamera = nullptr;
	UPROPERTY(VisibleAnywhere)
		TSet<AActor*>			_overlapActors;
	UPROPERTY(VisibleAnywhere, Replicated)
		class AWeapon*			_equippedWeapon = nullptr; // 当前装备的武器

	// 按键绑定
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
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "EnhancedInput|Action")
		class UInputAction* IA_PickUp;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "EnhancedInput|Action")
		class UInputAction* IA_Drop;

	void OnActionMoveForward(const FInputActionValue& inputActionValue);
	void OnActionMoveRight(const FInputActionValue& inputActionValue);
	void OnActionJump(const FInputActionValue& inputActionValue);
	void OnActionLookUp(const FInputActionValue& inputActionValue);
	void OnActionTurn(const FInputActionValue& inputActionValue);
	void OnActionPickUp(const FInputActionValue& inputActionValue);
	void OnActionDrop(const FInputActionValue& inputActionValue);
	/*********************************************************************************************/

	UFUNCTION(BlueprintImplementableEvent)
	void EventPlayerStateUpdate(class APlayerState* newPlayerState);
	UFUNCTION() 
	void OnCapsuleBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnCapsuleEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	UFUNCTION(Server, Reliable)
	void RPC_PickUp();
	UFUNCTION(Server, Reliable)
	void RPC_Drop();
};
