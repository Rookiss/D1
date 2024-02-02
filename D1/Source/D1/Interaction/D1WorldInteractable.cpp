#include "D1WorldInteractable.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1WorldInteractable)

AD1WorldInteractable::AD1WorldInteractable(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}

void AD1WorldInteractable::AddInteractionInfo(TArray<FD1InteractionInfo>& OutInteractionInfos)
{
	FD1InteractionInfo& InfoEntry = OutInteractionInfos.Add_GetRef(InteractionInfo);
	InfoEntry.Interactable = this;
}
