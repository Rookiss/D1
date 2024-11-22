#include "D1EnhancedPlayerInput.h"

#include "EnhancedActionKeyMapping.h"
#include "GameplayTagContainer.h"

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
			APlayerController* PlayerController = GetOuterAPlayerController();
			ULocalPlayer* LocalPlayer = PlayerController ? Cast<ULocalPlayer>(PlayerController->Player) : nullptr;
			if (LocalPlayer)
			{
				if (FKeyState* KeyState = GetKeyStateMap().Find(KeyMapping.Key))
				{
					if (KeyState->bDown)
					{
						FInputKeyParams Params(KeyMapping.Key, IE_Released, 0.f);
						Params.NumSamples = 1;
						InputKey(Params);
					}
				}
			}
			
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
			
			bIsFlushingInputThisFrame = true;
		}
	}
}
