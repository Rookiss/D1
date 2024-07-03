#include "D1WorldPickupable.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1WorldPickupable)

AD1WorldPickupable::AD1WorldPickupable(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}

void AD1WorldPickupable::GatherPostInteractionInfos(const FD1InteractionQuery& InteractionQuery, FD1InteractionInfoBuilder& InteractionInfoBuilder) const
{
	InteractionInfoBuilder.AddInteractionInfo(GetPreInteractionInfo(InteractionQuery));
}
