#pragma once

#include "GameFramework/Character.h"
#include "D1PocketWorldCharacter.generated.h"

UCLASS()
class AD1PocketWorldCharacter : public ACharacter
{
	GENERATED_BODY()
	
public:
	AD1PocketWorldCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};
