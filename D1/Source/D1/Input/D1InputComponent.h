#pragma once

#include "EnhancedInputComponent.h"
#include "GameplayTagContainer.h"
#include "Data/D1InputData.h"
#include "D1InputComponent.generated.h"

struct FGameplayTag;

UCLASS()
class UD1InputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()
	
public:
	UD1InputComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	template<class UserClass, typename FuncType>
	void BindNativeAction(const UD1InputData* InputData, const FGameplayTag& InputTag, ETriggerEvent TriggerEvent, UserClass* Object, FuncType Func, TArray<uint32>& OutBindHandles);

	template<class UserClass, typename PressedFuncType, typename ReleasedFuncType>
	void BindAbilityActions(const UD1InputData* InputData, UserClass* Object, PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc, TArray<uint32>& OutBindHandles);

	void RemoveBinds(TArray<uint32>& OutBindHandles);
};

template <class UserClass, typename FuncType>
void UD1InputComponent::BindNativeAction(const UD1InputData* InputData, const FGameplayTag& InputTag, ETriggerEvent TriggerEvent, UserClass* Object, FuncType Func, TArray<uint32>& OutBindHandles)
{
	check(InputData);

	if (const UInputAction* InputAction = InputData->FindNativeInputActionForTag(InputTag))
	{
		OutBindHandles.Add(BindAction(InputAction, TriggerEvent, Object, Func).GetHandle());
	}
}

template <class UserClass, typename PressedFuncType, typename ReleasedFuncType>
void UD1InputComponent::BindAbilityActions(const UD1InputData* InputData, UserClass* Object, PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc, TArray<uint32>& OutBindHandles)
{
	check(InputData);
	
	for (const FD1InputAction& Action : InputData->AbilityInputActions)
	{
		if (Action.InputAction && Action.InputTag.IsValid())
		{
			if (PressedFunc)
			{
				OutBindHandles.Add(BindAction(Action.InputAction, ETriggerEvent::Triggered, Object, PressedFunc, Action.InputTag).GetHandle());
			}

			if (ReleasedFunc)
			{
				OutBindHandles.Add(BindAction(Action.InputAction, ETriggerEvent::Completed, Object, ReleasedFunc, Action.InputTag).GetHandle());
			}
		}
	}
}
