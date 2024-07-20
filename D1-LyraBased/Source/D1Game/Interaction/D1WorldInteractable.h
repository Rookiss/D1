#pragma once

#include "D1Interactable.h"
#include "D1WorldInteractable.generated.h"

UCLASS(Abstract, BlueprintType, Blueprintable)
class AD1WorldInteractable : public AActor, public ID1Interactable
{
	GENERATED_BODY()
	
public:
	AD1WorldInteractable(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void OnInteraction();

public:
	UPROPERTY(BlueprintReadWrite)
	bool bIsUsed = false;

	UPROPERTY(BlueprintReadWrite)
	bool bIsSomeoneInteracting = false;
};
