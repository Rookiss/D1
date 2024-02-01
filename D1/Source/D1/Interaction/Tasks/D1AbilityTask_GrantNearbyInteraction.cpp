#include "D1AbilityTask_GrantNearbyInteraction.h"

#include "AbilitySystemComponent.h"
#include "Interaction/D1Interactable.h"
#include "Interaction/D1InteractionInfo.h"
#include "Physics/D1CollisionChannels.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1AbilityTask_GrantNearbyInteraction)

UD1AbilityTask_GrantNearbyInteraction::UD1AbilityTask_GrantNearbyInteraction(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1AbilityTask_GrantNearbyInteraction::Activate()
{
	Super::Activate();

	SetWaitingOnAvatar();
	GetWorld()->GetTimerManager().SetTimer(QueryTimerHandle, this, &ThisClass::QueryInteractables, InteractionScanRate, true);
}

void UD1AbilityTask_GrantNearbyInteraction::OnDestroy(bool bInOwnerFinished)
{
	GetWorld()->GetTimerManager().ClearTimer(QueryTimerHandle);

	Super::OnDestroy(bInOwnerFinished);
}

UD1AbilityTask_GrantNearbyInteraction* UD1AbilityTask_GrantNearbyInteraction::GrantNearbyInteraction(UGameplayAbility* OwningAbility, float InteractionScanRange, float InteractionScanRate)
{
	UD1AbilityTask_GrantNearbyInteraction* TaskObject = NewAbilityTask<UD1AbilityTask_GrantNearbyInteraction>(OwningAbility);
	TaskObject->InteractionScanRange = InteractionScanRange;
	TaskObject->InteractionScanRate = InteractionScanRate;
	return TaskObject;
}

void UD1AbilityTask_GrantNearbyInteraction::QueryInteractables()
{
	UWorld* World = GetWorld();
	AActor* AvatarActor = GetAvatarActor();

	if (World && AvatarActor)
	{
		FCollisionQueryParams Params(SCENE_QUERY_STAT(UD1AbilityTask_GrantNearbyInteraction)/*Used for Profiling*/, false);

		TArray<FOverlapResult> OverlapResults;
		World->OverlapMultiByChannel(OverlapResults, AvatarActor->GetActorLocation(), FQuat::Identity, D1_TraceChannel_Interaction, FCollisionShape::MakeSphere(InteractionScanRange), Params);

		if (OverlapResults.Num() > 0)
		{
			TArray<TScriptInterface<ID1Interactable>> Interactables;
			for (const FOverlapResult& OverlapResult : OverlapResults)
			{
				TScriptInterface<ID1Interactable> InteractableActor(OverlapResult.GetActor());
				if (InteractableActor)
				{
					Interactables.AddUnique(InteractableActor);
				}

				TScriptInterface<ID1Interactable> InteractableComponent(OverlapResult.GetComponent());
				if (InteractableComponent)
				{
					Interactables.AddUnique(InteractableComponent);
				}
			}
			
			// TODO
			TArray<FD1InteractionInfo> Infos;
			// for (TScriptInterface<ID1Interactable>& Interactable : Interactables)
			// {
			// 	// Interactable->Gather
			// 	
			// }

			TSet<FObjectKey> RemoveKeys;
			AppliedInteractionAbilities.GetKeys(RemoveKeys);
			
			for (FD1InteractionInfo& Info : Infos)
			{
				if (Info.InteractionAbilityToGrant)
				{
					FObjectKey ObjectKey(Info.InteractionAbilityToGrant);
					RemoveKeys.Remove(ObjectKey);
					
					if (AppliedInteractionAbilities.Find(ObjectKey) == false)
					{
						FGameplayAbilitySpec Spec(Info.InteractionAbilityToGrant, 1, INDEX_NONE, this);
						FGameplayAbilitySpecHandle Handle = AbilitySystemComponent->GiveAbility(Spec);
						AppliedInteractionAbilities.Add(ObjectKey, Handle);
					}
				}
			}
		}
	}
}
