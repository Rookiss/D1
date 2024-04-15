#pragma once

#include "EnhancedPlayerInput.h"
#include "D1EnhancedPlayerInput.generated.h"

struct FGameplayTag;

UCLASS()
class UD1EnhancedPlayerInput : public UEnhancedPlayerInput
{
	GENERATED_BODY()
	
public:
	UD1EnhancedPlayerInput();

public:
	void FlushPressedInput(UInputAction* InputAction);
};
