#include "D1PickupableItemBase.h"

#include "Components/BoxComponent.h"
#include "Data/D1ItemData.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Item/D1ItemInstance.h"
#include "Item/Fragments/D1ItemFragment_Equipable.h"
#include "Kismet/KismetSystemLibrary.h"
#include "System/LyraAssetManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1PickupableItemBase)

AD1PickupableItemBase::AD1PickupableItemBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
	
	bReplicates = true;
	bAlwaysRelevant = true;
	AActor::SetReplicateMovement(true);

	MovementCollision = CreateDefaultSubobject<UBoxComponent>("BoxCollision");
	MovementCollision->SetCollisionProfileName("BlockOnlyWorldObject");
	SetRootComponent(MovementCollision);

	PickupCollision = CreateDefaultSubobject<UBoxComponent>("PickupCollision");
	PickupCollision->SetCollisionProfileName("Pickupable");
	PickupCollision->SetupAttachment(GetRootComponent());
	
    MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("MeshComponent");
	MeshComponent->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
	MeshComponent->SetCollisionProfileName("NoCollision");
	MeshComponent->SetupAttachment(GetRootComponent());

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovement");
	ProjectileMovement->bShouldBounce = true;
	ProjectileMovement->Bounciness = 0.f;
	ProjectileMovement->Friction = 1.f;
	ProjectileMovement->BounceVelocityStopSimulatingThreshold = 0.f;
	ProjectileMovement->Velocity = FVector::ZeroVector;
}

void AD1PickupableItemBase::OnRep_PickupInfo()
{
	Super::OnRep_PickupInfo();

	TSoftObjectPtr<UStaticMesh> PickupableMeshPath = nullptr;
		
	if (const UD1ItemInstance* ItemInstance = PickupInfo.PickupInstance.ItemInstance)
	{
		const UD1ItemTemplate& ItemTemplate = UD1ItemData::Get().FindItemTemplateByID(ItemInstance->GetItemTemplateID());
		PickupableMeshPath = ItemTemplate.PickupableMesh;
	}
	else if (TSubclassOf<UD1ItemTemplate> ItemTemplateClass = PickupInfo.PickupTemplate.ItemTemplateClass)
	{
		const UD1ItemTemplate* ItemTemplate = ItemTemplateClass->GetDefaultObject<UD1ItemTemplate>();
		PickupableMeshPath = ItemTemplate->PickupableMesh;
	}

	if (PickupableMeshPath.IsNull() == false)
	{
		if (UStaticMesh* PickupableMesh = ULyraAssetManager::GetAssetByPath(PickupableMeshPath))
		{
			MeshComponent->SetStaticMesh(PickupableMesh);

			if (bAutoCollisionResize)
			{
				float Radius;
				FVector Origin, BoxExtent;
				UKismetSystemLibrary::GetComponentBounds(MeshComponent, Origin, BoxExtent, Radius);

				FVector MovementCollisionExtent = FVector(FMath::Min(MaxMovementCollisionExtent.X, BoxExtent.X), FMath::Min(MaxMovementCollisionExtent.Y, BoxExtent.Y), BoxExtent.Z);
				MovementCollision->SetBoxExtent(MovementCollisionExtent);
				
				FVector PickupCollisionExtent = FVector(FMath::Max(MinPickupCollisionExtent.X, BoxExtent.X), FMath::Max(MinPickupCollisionExtent.Y, BoxExtent.Y), BoxExtent.Z);
				PickupCollision->SetBoxExtent(PickupCollisionExtent);
			}
		}
	}
}

void AD1PickupableItemBase::GetMeshComponents(TArray<UMeshComponent*>& OutMeshComponents) const
{
	OutMeshComponents.Add(MeshComponent);
}
