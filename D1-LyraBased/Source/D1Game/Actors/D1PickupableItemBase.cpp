#include "D1PickupableItemBase.h"

#include "Components/ArrowComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1PickupableItemBase)

AD1PickupableItemBase::AD1PickupableItemBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ArrowComponent = CreateDefaultSubobject<UArrowComponent>("ArrowComponent");
	SetRootComponent(ArrowComponent);
	
    ItemMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("ItemMeshComponent");
	ItemMeshComponent->SetupAttachment(ArrowComponent);
}

void AD1PickupableItemBase::InitializeActor(UD1ItemInstance* InItemInstance, int32 InItemCount)
{
	
}
