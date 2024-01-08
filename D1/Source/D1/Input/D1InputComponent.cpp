#include "D1InputComponent.h"

#include "EnhancedInputSubsystems.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1InputComponent)

UD1InputComponent::UD1InputComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1InputComponent::RemoveBinds(TArray<uint32>& OutBindHandles)
{
	for (uint32 Handle : OutBindHandles)
	{
		RemoveBindingByHandle(Handle);
	}
	OutBindHandles.Reset();
}
