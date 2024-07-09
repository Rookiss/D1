#include "D1StatueBase.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1StatueBase)

AD1StatueBase::AD1StatueBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	SetRootComponent(MeshComponent);
	MeshComponent->SetCollisionProfileName(TEXT("Interactable"));
}

FD1InteractionInfo AD1StatueBase::GetPreInteractionInfo(const FD1InteractionQuery& InteractionQuery) const
{
	return InteractionInfo;
}
