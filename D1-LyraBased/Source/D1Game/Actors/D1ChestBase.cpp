#include "D1ChestBase.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1ChestBase)

AD1ChestBase::AD1ChestBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

FD1InteractionInfo AD1ChestBase::GetInteractionInfo() const
{
	return bIsOpened ? OpenedInteractionInfo : ClosedInteractionInfo;
}
