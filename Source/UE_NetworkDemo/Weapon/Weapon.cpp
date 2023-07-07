#include "Weapon.h"
#include "../Common/Utils.h"
#include "../Character/BlasterCharacter.h"
#include "Components/SphereComponent.h"

AWeapon::AWeapon()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	
	_mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	SetRootComponent(_mesh);
	_mesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	_mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	_mesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);	

	_sphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereColision"));
	_sphereCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	_sphereCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	_sphereCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	_sphereCollision->SetupAttachment(RootComponent);
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();	

	if (HasAuthority())
	{
		SetReplicateMovement(true);
		_sphereCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		_mesh->SetSimulatePhysics(true);		
	}
}

void AWeapon::OnRep_AttachmentReplication()
{
	if (AttachmentReplication.AttachParent)
	{
		if (RootComponent)
		{
			USceneComponent* AttachParentComponent = (AttachmentReplication.AttachComponent ? ToRawPtr(AttachmentReplication.AttachComponent) : AttachmentReplication.AttachParent->GetRootComponent());

			if (AttachParentComponent)
			{
				RootComponent->SetRelativeLocation_Direct(AttachmentReplication.LocationOffset);
				RootComponent->SetRelativeRotation_Direct(AttachmentReplication.RotationOffset);
				RootComponent->SetRelativeScale3D_Direct(AttachmentReplication.RelativeScale3D);

				// If we're already attached to the correct Parent and Socket, then the update must be position only.
				// AttachToComponent would early out in this case.
				// Note, we ignore the special case for simulated bodies in AttachToComponent as AttachmentReplication shouldn't get updated
				// if the body is simulated (see AActor::GatherMovement).
				const bool bAlreadyAttached = (AttachParentComponent == RootComponent->GetAttachParent() && AttachmentReplication.AttachSocket == RootComponent->GetAttachSocketName() && AttachParentComponent->GetAttachChildren().Contains(RootComponent));
				if (bAlreadyAttached)
				{
					// Note, this doesn't match AttachToComponent, but we're assuming it's safe to skip physics (see comment above).
					RootComponent->UpdateComponentToWorld(EUpdateTransformFlags::SkipPhysicsUpdate, ETeleportType::None);
				}
				else
				{
					_mesh->SetSimulatePhysics(false);
					bool ret = false;
					ret = RootComponent->AttachToComponent(AttachParentComponent, FAttachmentTransformRules::KeepRelativeTransform, AttachmentReplication.AttachSocket);
					if (!ret)
					{
						LogWarning(TEXT("AttachToComponent failed"));
					}
				}
			}
		}
	}
	else
	{
		DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

		// Handle the case where an object was both detached and moved on the server in the same frame.
		// Calling this extraneously does not hurt but will properly fire events if the movement state changed while attached.
		// This is needed because client side movement is ignored when attached
		if (IsReplicatingMovement())
		{
			OnRep_ReplicatedMovement();
		}
	}
}

void AWeapon::SetState(EWeaponState weaponState)
{
	if (weaponState == EWeaponState::Equipped)
	{
		_sphereCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		_mesh->SetSimulatePhysics(false);
	}
	else
	{
		_sphereCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		_mesh->SetSimulatePhysics(true);
	}
}

void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

