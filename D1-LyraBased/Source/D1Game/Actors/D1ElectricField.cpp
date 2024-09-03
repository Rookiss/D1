#include "D1ElectricField.h"

#include "GameModes/LyraGameState.h"
#include "Kismet/GameplayStatics.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1ElectricField)

class ALyraGameState;

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

void AD1ElectricField::BeginPlay()
{
	Super::BeginPlay();

	if (ALyraGameState* LyraGameState = Cast<ALyraGameState>(UGameplayStatics::GetGameState(this)))
	{
		LyraGameState->CachedElectricFieldActor = this;
	}
}
