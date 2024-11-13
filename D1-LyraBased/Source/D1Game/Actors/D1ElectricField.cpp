#include "D1ElectricField.h"

#include "GameModes/LyraGameState.h"
#include "Kismet/GameplayStatics.h"
#include "System/D1ElectricFieldManagerComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1ElectricField)

class ALyraGameState;

AD1ElectricField::AD1ElectricField(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;

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
		if (UD1ElectricFieldManagerComponent* ElectricFieldManager = LyraGameState->FindComponentByClass<UD1ElectricFieldManagerComponent>())
		{
			ElectricFieldManager->ElectricFieldActor = this;
		}
	}
}
