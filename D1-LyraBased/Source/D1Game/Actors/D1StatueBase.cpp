#include "D1StatueBase.h"

#include "Components/ArrowComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1StatueBase)

AD1StatueBase::AD1StatueBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowComponent"));
	SetRootComponent(ArrowComponent);
	
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(GetRootComponent());
	MeshComponent->SetCollisionProfileName(TEXT("Interactable"));
	MeshComponent->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));

	bCanUsed = true;
}

FD1InteractionInfo AD1StatueBase::GetPreInteractionInfo(const FD1InteractionQuery& InteractionQuery) const
{
	return InteractionInfo;
}
