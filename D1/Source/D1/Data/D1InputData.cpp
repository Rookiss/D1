#include "D1InputData.h"

#include "..\D1LogChannels.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1InputData)

const UInputAction* UD1InputData::FindNativeInputActionForTag(const FGameplayTag& InputTag) const
{
	for (const FD1InputAction& Action : NativeInputActions)
	{
		if (Action.InputAction && Action.InputTag == InputTag)
		{
			return Action.InputAction;
		}
	}

	UE_LOG(LogD1, Error, TEXT("Can't find NativeInputAction for InputTag [%s] on InputConfig [%s]."), *InputTag.ToString(), *GetNameSafe(this));
	return nullptr;
}
