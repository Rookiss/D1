#include "D1GameplayAbility_Interact_Info.h"

#include "Interaction/D1Interactable.h"
#include "Interaction/D1InteractionQuery.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1GameplayAbility_Interact_Info)

UD1GameplayAbility_Interact_Info::UD1GameplayAbility_Interact_Info(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

bool UD1GameplayAbility_Interact_Info::InitializeAbility(AActor* TargetActor)
{
	TScriptInterface<ID1Interactable> TargetInteractable(TargetActor);
	if (TargetInteractable)
	{
		FD1InteractionQuery InteractionQuery;
		InteractionQuery.RequestingAvatar = GetAvatarActorFromActorInfo();
		InteractionQuery.RequestingController = GetControllerFromActorInfo();

		Interactable = TargetInteractable;
		InteractableActor = TargetActor;

		TArray<FD1InteractionInfo> InteractionInfos;
		FD1InteractionInfoBuilder InteractionInfoBuilder(Interactable, InteractionInfos);
		Interactable->GatherPostInteractionInfos(InteractionQuery, InteractionInfoBuilder);
		InteractionInfo = InteractionInfos[0];

		return true;
	}

	return false;
}
