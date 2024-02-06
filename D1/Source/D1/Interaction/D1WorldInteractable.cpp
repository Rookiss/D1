#include "D1WorldInteractable.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1WorldInteractable)

AD1WorldInteractable::AD1WorldInteractable(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InteractionInfo.Interactable = this;

	SceneComponent = CreateDefaultSubobject<USceneComponent>("SceneComponent");
	SceneComponent->SetupAttachment(GetRootComponent());
	
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("StaticMeshComponent");
	StaticMeshComponent->SetupAttachment(SceneComponent);
	StaticMeshComponent->SetCollisionProfileName("Interactable_BlockDynamic");
}
