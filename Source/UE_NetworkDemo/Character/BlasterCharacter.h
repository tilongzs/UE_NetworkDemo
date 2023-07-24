#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "../Plugins/EnhancedInput/Source/EnhancedInput/Public/inputActionValue.h"
#include "Engine/NetSerialization.h"
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

	inline bool IsWeaponEquipped() { return _equippedWeapon != nullptr; }
	inline bool IsAiming() { return _isAiming; }
	class USkeletalMeshComponent* GetWeaponMesh();
	void Server_OnDamage(float damage, class APawn* instigator);

	UFUNCTION(BlueprintCallable)
		inline float GetDefaultHealth() { return _defaultHealth; }
	UFUNCTION(BlueprintCallable)
		inline float GetCurrentHealth() { return _currentHealth; }
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
	UPROPERTY(VisibleAnywhere, Replicated)
		bool					_isAiming = false;
	float						_defaultFOV = 90.f;
	float						_currentFOV = 90.f;
	float						_zoomInterpSpeed = 40.f;
	FTimerHandle				_fireTimer;
	FVector_NetQuantize			_fireImpactPoint;
	UPROPERTY(VisibleAnywhere)
	float						_defaultHealth = 100.f;
	UPROPERTY(VisibleAnywhere, Replicated)
	float						_currentHealth = _defaultHealth;

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
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "EnhancedInput|Action")
		class UInputAction* IA_Crouch;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "EnhancedInput|Action")
		class UInputAction* IA_Aiming;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "EnhancedInput|Action")
		class UInputAction* IA_Fire;

	void OnActionMoveForward(const FInputActionValue& inputActionValue);
	void OnActionMoveRight(const FInputActionValue& inputActionValue);
	void OnActionJump(const FInputActionValue& inputActionValue);
	void OnActionLookUp(const FInputActionValue& inputActionValue);
	void OnActionTurn(const FInputActionValue& inputActionValue);
	void OnActionPickUp(const FInputActionValue& inputActionValue);
	void OnActionDrop(const FInputActionValue& inputActionValue);
	void OnActionCrouch(const FInputActionValue& inputActionValue);
	void OnActionAimingStart(const FInputActionValue& inputActionValue);
	void OnActionAimingComplete(const FInputActionValue& inputActionValue);
	void OnActionFireStart(const FInputActionValue& inputActionValue);
	void OnActionFireComplete(const FInputActionValue& inputActionValue);
	/*********************************************************************************************/

	UFUNCTION(BlueprintImplementableEvent)
		void EventPlayerStateUpdate(class APlayerState* newPlayerState);
	UFUNCTION() 
		void OnCapsuleBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		void OnCapsuleEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	/// <summary>
	/// 捡起、丢弃武器
	/// </summary>
	void PickUp();
	UFUNCTION(Server, Reliable)
		void Server_PickUp();
	
	void Drop();
	UFUNCTION(Server, Reliable)	
		void Server_Drop();

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDelegatePickUp, bool, isPickUp);
	UPROPERTY(BlueprintAssignable)
	FDelegatePickUp	_dlgPickup;
	/***************************************************************************************/
	
	void Aim(bool isAiming);
	UFUNCTION(Server, Reliable)
		void Server_Aim(bool isAiming);
	void UpdateZoom(float DeltaTime);

	void OnTimerFire();
	UFUNCTION(Server, Reliable)
		void Server_Fire(bool isStop, const FVector_NetQuantize& fireImpactPoint);
	UFUNCTION(NetMulticast, Reliable)
		void Multicast_Fire(const FVector_NetQuantize& fireImpactPoint);

	void TraceUnderCrosshairs(FVector& fireImpactPoint);
};
