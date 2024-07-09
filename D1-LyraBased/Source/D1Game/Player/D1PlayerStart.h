#pragma once

#include "GameplayTagContainer.h"
#include "GameFramework/PlayerStart.h"
#include "D1PlayerStart.generated.h"

enum class ED1PlayerStartLocationOccupancy
{
	Empty,
	Partial,
	Full
};

UCLASS()
class AD1PlayerStart : public APlayerStart
{
	GENERATED_BODY()
	
public:
	AD1PlayerStart(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	bool TryClaim(AController* OccupyingController);

public:
	bool IsClaimed() const;
	const FGameplayTagContainer& GetGameplayTags() { return StartPointTags; }
	ED1PlayerStartLocationOccupancy GetLocationOccupancy(AController* const ControllerPawnToFit) const;
	
protected:
	UPROPERTY(Transient)
	TObjectPtr<AController> ClaimingController = nullptr;
	
	UPROPERTY(EditAnywhere)
	FGameplayTagContainer StartPointTags;
};
