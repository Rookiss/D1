#include "D1TargetPointMonster.h"

#include "AIController.h"
#include "Character/LyraPawnExtensionComponent.h"
#include "GameModes/LyraGameMode.h"
#include "Kismet/GameplayStatics.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1TargetPointMonster)

AD1TargetPointMonster::AD1TargetPointMonster(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void AD1TargetPointMonster::InitializeSpawningActor(AActor* InSpawningActor)
{
	Super::InitializeSpawningActor(InSpawningActor);

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParameters.OverrideLevel = InSpawningActor->GetLevel();
	SpawnParameters.ObjectFlags |= RF_Transient;
	
	AAIController* NewController = GetWorld()->SpawnActor<AAIController>(AIControllerClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParameters);
	if (NewController)
	{
		NewController->Possess(Cast<APawn>(InSpawningActor));
	}
	
	if (APawn* ControlledPawn = NewController->GetPawn())
	{
		if (ULyraPawnExtensionComponent* PawnExtensionComponent = ControlledPawn->FindComponentByClass<ULyraPawnExtensionComponent>())
		{
			ALyraGameMode* LyraGameMode = Cast<ALyraGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
			check(LyraGameMode);
			
			if (const ULyraPawnData* PawnData = LyraGameMode->GetPawnDataForController(NewController))
			{
				PawnExtensionComponent->SetPawnData(PawnData);
			}
		}
	}
}
