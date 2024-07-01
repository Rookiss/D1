#include "D1WorldInteractable.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1WorldInteractable)

AD1WorldInteractable::AD1WorldInteractable(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}

void AD1WorldInteractable::GatherInteractionInfos(const FD1InteractionQuery& InteractionQuery, FD1InteractionInfoBuilder& InteractionInfoBuilder) const
{
	InteractionInfoBuilder.AddInteractionInfo(GetInteractionInfo());
}
