#pragma once

#include "GameFramework/CharacterMovementComponent.h"
#include "D1CharacterMovementComponent.generated.h"

UCLASS()
class UD1CharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()
	
public:
	UD1CharacterMovementComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual float GetMaxSpeed() const override;
};
