#include "D1ElectricField.h"

#include "Kismet/GameplayStatics.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1ElectricField)

AD1ElectricField::AD1ElectricField(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	bAlwaysRelevant = true;
	AActor::SetReplicateMovement(true);
	
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetIsReplicated(true);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetRootComponent(MeshComponent);
}
