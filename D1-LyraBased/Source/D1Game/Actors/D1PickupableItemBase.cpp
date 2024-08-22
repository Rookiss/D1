#include "D1PickupableItemBase.h"

#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "Data/D1ItemData.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Item/D1ItemInstance.h"
#include "Item/Fragments/D1ItemFragment_Equippable.h"
#include "Kismet/KismetSystemLibrary.h"
#include "System/LyraAssetManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1PickupableItemBase)

AD1PickupableItemBase::AD1PickupableItemBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bReplicates = true;
	bAlwaysRelevant = true;
	AActor::SetReplicateMovement(true);
	
	BoxCollision = CreateDefaultSubobject<UBoxComponent>("BoxCollision");
	BoxCollision->SetCollisionProfileName(TEXT("Pickupable"));
	SetRootComponent(BoxCollision);
	
    MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("MeshComponent");
	MeshComponent->SetCollisionProfileName(TEXT("NoCollision"));
	MeshComponent->SetupAttachment(GetRootComponent());
	
	MeshComponent->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovement");
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
			FVector Origin, BoxExtent;
			float Radius;
			UKismetSystemLibrary::GetComponentBounds(MeshComponent, Origin, BoxExtent, Radius);
			BoxExtent.X = FMath::Max(15.f, BoxExtent.X);
			BoxExtent.Y = FMath::Max(15.f, BoxExtent.Y);
			BoxCollision->SetBoxExtent(BoxExtent);
		}
	}
}

void AD1PickupableItemBase::GetMeshComponents(TArray<UMeshComponent*>& OutMeshComponents) const
{
	OutMeshComponents.Add(MeshComponent);
}
