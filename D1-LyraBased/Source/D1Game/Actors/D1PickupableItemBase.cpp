#include "D1PickupableItemBase.h"

#include "Components/ArrowComponent.h"
#include "Data/D1ItemData.h"
#include "Item/D1ItemInstance.h"
#include "Item/Fragments/D1ItemFragment_Equippable.h"
#include "System/LyraAssetManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1PickupableItemBase)

AD1PickupableItemBase::AD1PickupableItemBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bReplicates = true;
	
	ArrowComponent = CreateDefaultSubobject<UArrowComponent>("ArrowComponent");
	SetRootComponent(ArrowComponent);
	
    PickupableMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("PickupableMeshComponent");
	PickupableMeshComponent->SetupAttachment(ArrowComponent);
	PickupableMeshComponent->SetCollisionProfileName(TEXT("Interactable"));
	PickupableMeshComponent->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
}

void AD1PickupableItemBase::OnRep_PickupInfo()
{
	Super::OnRep_PickupInfo();

	if (PickupableMeshComponent->GetStaticMesh() == nullptr)
	{
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
				PickupableMeshComponent->SetStaticMesh(PickupableMesh);
			}
		}
	}
}

void AD1PickupableItemBase::InitializeActor(UD1ItemInstance* InItemInstance, int32 InItemCount)
{
	if (HasAuthority() == false)
		return;
	
	if (InItemInstance == nullptr || InItemCount <= 0)
	{
		Destroy();
		return;
	}

	PickupInfo.PickupInstance.ItemInstance = InItemInstance;
	PickupInfo.PickupInstance.ItemCount = InItemCount;
	OnRep_PickupInfo();
}
