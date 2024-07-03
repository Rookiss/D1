#include "D1WorldInteractable.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "D1InteractionQuery.h"
#include "AbilitySystem/Attributes/LyraCombatSet.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1WorldInteractable)

AD1WorldInteractable::AD1WorldInteractable(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}

void AD1WorldInteractable::GatherPostInteractionInfos(const FD1InteractionQuery& InteractionQuery, FD1InteractionInfoBuilder& InteractionInfoBuilder) const
{
	FD1InteractionInfo InteractionInfo = GetPreInteractionInfo(InteractionQuery);
	
	if (UAbilitySystemComponent* AbilitySystem = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InteractionQuery.RequestingAvatar.Get()))
	{
		float Resourcefulness = AbilitySystem->GetNumericAttribute(ULyraCombatSet::GetResourcefulnessAttribute());
		InteractionInfo.Duration = FMath::Max<float>(1.f, InteractionInfo.Duration - Resourcefulness * 0.5f);
	}
	
	InteractionInfoBuilder.AddInteractionInfo(InteractionInfo);
}
