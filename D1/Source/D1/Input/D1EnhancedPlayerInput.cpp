#include "D1EnhancedPlayerInput.h"

#include "EnhancedActionKeyMapping.h"
#include "GameplayTagContainer.h"
#include "Data/D1InputData.h"
#include "System/D1AssetManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1EnhancedPlayerInput)

UD1EnhancedPlayerInput::UD1EnhancedPlayerInput()
{
    
}

void UD1EnhancedPlayerInput::FlushPressedInput(UInputAction* InputAction)
{
	const TArray<FEnhancedActionKeyMapping>& KeyMappings = GetEnhancedActionMappings();
	for (const FEnhancedActionKeyMapping& KeyMapping : KeyMappings)
	{
		if (KeyMapping.Action == InputAction)
		{
			UWorld* World = GetWorld();
			check(World);
			float TimeSeconds = World->GetRealTimeSeconds();

			if (FKeyState* KeyState = GetKeyStateMap().Find(KeyMapping.Key))
			{
				KeyState->RawValue = FVector(0.f, 0.f, 0.f);
				KeyState->bDown = false;
				KeyState->bDownPrevious = false;
				KeyState->LastUpDownTransitionTime = TimeSeconds;
			}
			
			return;
		}
	}
}
