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

void AD1ElectricField::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (HasAuthority() == false)
		return;
	
	// if (ABattleRoyalGameState* GameState = Cast<ABattleRoyalGameState>(UGameplayStatics::GetGameState(this)))
	// {
	// 	MeshComponent->SetVisibility(GameState->bVisibleCurrCircle, true);
	// 	SetActorLocation(GameState->CurrCirclePos);
	//
	// 	float Scale = GameState->CurrCircleRadius / 100.f * 2.f;
	// 	SetActorScale3D(FVector(Scale, Scale, 2000.f));
	// }
}
