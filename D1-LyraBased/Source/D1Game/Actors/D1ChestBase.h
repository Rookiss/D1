#pragma once

#include "Interaction2/D1WorldInteractable.h"
#include "D1ChestBase.generated.h"

UCLASS(BlueprintType, Blueprintable)
class AD1ChestBase : public AD1WorldInteractable
{
	GENERATED_BODY()
	
public:
	AD1ChestBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	virtual FD1InteractionInfo GetInteractionInfo() const override;

protected:
	UPROPERTY(BlueprintReadWrite)
	bool bIsOpened = false;

private:
	UPROPERTY(EditDefaultsOnly)
	FD1InteractionInfo OpenedInteractionInfo;
	
	UPROPERTY(EditDefaultsOnly)
	FD1InteractionInfo ClosedInteractionInfo;
};
