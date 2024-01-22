#pragma once

#include "GameFramework/GameModeBase.h"
#include "D1GameMode.generated.h"

UCLASS()
class AD1GameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	AD1GameMode(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};
