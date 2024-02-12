#pragma once

#include "UObject/Interface.h"
#include "D1Interactable.generated.h"

USTRUCT(BlueprintType)
struct FD1InteractionInfo
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText InteractionTitle;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText InteractionContent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float HoldTime = 0.f;
};

UINTERFACE(MinimalAPI, BlueprintType)
class UD1Interactable : public UInterface
{
	GENERATED_BODY()
};

class ID1Interactable
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Interact(AActor* Instigator);
	virtual void Interact_Implementation(AActor* Instigator) { }
	
public:
	virtual const FD1InteractionInfo& GetInteractionInfo() const abstract;
};
