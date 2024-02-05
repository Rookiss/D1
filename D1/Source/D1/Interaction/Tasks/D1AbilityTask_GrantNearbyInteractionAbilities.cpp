#include "D1AbilityTask_GrantNearbyInteractionAbilities.h"

#include "AbilitySystemComponent.h"
#include "Interaction/D1Interactable.h"
#include "Interaction/D1InteractionInfo.h"
#include "Physics/D1CollisionChannels.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1AbilityTask_GrantNearbyInteractionAbilities)

UD1AbilityTask_GrantNearbyInteractionAbilities::UD1AbilityTask_GrantNearbyInteractionAbilities(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

UD1AbilityTask_GrantNearbyInteractionAbilities* UD1AbilityTask_GrantNearbyInteractionAbilities::GrantNearbyInteractionAbilities(UGameplayAbility* OwningAbility, float InteractionScanRange, float InteractionScanRate)
{
	UD1AbilityTask_GrantNearbyInteractionAbilities* TaskObject = NewAbilityTask<UD1AbilityTask_GrantNearbyInteractionAbilities>(OwningAbility);
	TaskObject->InteractionScanRange = InteractionScanRange;
	TaskObject->InteractionScanRate = InteractionScanRate;
	return TaskObject;
}

void UD1AbilityTask_GrantNearbyInteractionAbilities::Activate()
{
	SetWaitingOnAvatar();
	GetWorld()->GetTimerManager().SetTimer(QueryTimerHandle, this, &ThisClass::QueryInteractables, InteractionScanRate, true);
}

void UD1AbilityTask_GrantNearbyInteractionAbilities::OnDestroy(bool bInOwnerFinished)
{
	GetWorld()->GetTimerManager().ClearTimer(QueryTimerHandle);

	Super::OnDestroy(bInOwnerFinished);
}

void UD1AbilityTask_GrantNearbyInteractionAbilities::QueryInteractables()
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
			}
			
			TArray<FD1InteractionInfo> InteractableInfos;
			for (TScriptInterface<ID1Interactable>& Interactable : Interactables)
			{
				InteractableInfos.Add(Interactable->GetInteractionInfo());
			}

			TSet<FObjectKey> RemoveKeys;
			AppliedInteractionAbilities.GetKeys(RemoveKeys);
			
			for (FD1InteractionInfo& Info : InteractableInfos)
			{
				if (Info.InteractionAbilityToGrant)
				{
					FObjectKey ObjectKey(Info.InteractionAbilityToGrant);
					
					if (AppliedInteractionAbilities.Find(ObjectKey))
					{
						RemoveKeys.Remove(ObjectKey);
					}
					else
					{
						FGameplayAbilitySpec Spec(Info.InteractionAbilityToGrant, 1, INDEX_NONE, this);
						FGameplayAbilitySpecHandle Handle = AbilitySystemComponent->GiveAbility(Spec);
						AppliedInteractionAbilities.Add(ObjectKey, Handle);
					}
				}
			}

			for (const FObjectKey& RemoveKey : RemoveKeys)
			{
				AbilitySystemComponent->ClearAbility(AppliedInteractionAbilities[RemoveKey]);
			}
		}
	}
}
