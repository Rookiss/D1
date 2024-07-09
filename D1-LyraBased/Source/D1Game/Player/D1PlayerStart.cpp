#include "D1PlayerStart.h"

#include "LyraPlayerStart.h"
#include "GameFramework/GameModeBase.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1PlayerStart)

AD1PlayerStart::AD1PlayerStart(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

bool AD1PlayerStart::TryClaim(AController* OccupyingController)
{
	if (OccupyingController && IsClaimed() == false)
	{
		ClaimingController = OccupyingController;
		return true;
	}
	return false;
}

bool AD1PlayerStart::IsClaimed() const
{
	return ClaimingController != nullptr;
}

ED1PlayerStartLocationOccupancy AD1PlayerStart::GetLocationOccupancy(AController* const ControllerPawnToFit) const
{
	UWorld* const World = GetWorld();
	if (HasAuthority() && World)
	{
		if (AGameModeBase* AuthGameMode = World->GetAuthGameMode())
		{
			TSubclassOf<APawn> PawnClass = AuthGameMode->GetDefaultPawnClassForController(ControllerPawnToFit);
			const APawn* const PawnToFit = PawnClass ? GetDefault<APawn>(PawnClass) : nullptr;

			FVector ActorLocation = GetActorLocation();
			const FRotator ActorRotation = GetActorRotation();

			if (World->EncroachingBlockingGeometry(PawnToFit, ActorLocation, ActorRotation, nullptr) == false)
			{
				return ED1PlayerStartLocationOccupancy::Empty;
			}
			else if (World->FindTeleportSpot(PawnToFit, ActorLocation, ActorRotation))
			{
				return ED1PlayerStartLocationOccupancy::Partial;
			}
		}
	}

	return ED1PlayerStartLocationOccupancy::Full;
}
