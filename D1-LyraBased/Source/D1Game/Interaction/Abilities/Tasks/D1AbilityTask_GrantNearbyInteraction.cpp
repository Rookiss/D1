#include "D1AbilityTask_GrantNearbyInteraction.h"

#include "AbilitySystemComponent.h"
#include "Engine/OverlapResult.h"
#include "Interaction/D1Interactable.h"
#include "Interaction/D1InteractionQuery.h"
#include "Physics/LyraCollisionChannels.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1AbilityTask_GrantNearbyInteraction)

UD1AbilityTask_GrantNearbyInteraction* UD1AbilityTask_GrantNearbyInteraction::GrantAbilitiesForNearbyInteractables(UGameplayAbility* OwningAbility, float InteractionAbilityScanRange, float InteractionAbilityScanRate)
{
	UD1AbilityTask_GrantNearbyInteraction* Task = NewAbilityTask<UD1AbilityTask_GrantNearbyInteraction>(OwningAbility);
	Task->InteractionAbilityScanRange = InteractionAbilityScanRange;
	Task->InteractionAbilityScanRate = InteractionAbilityScanRate;
	return Task;
}

void UD1AbilityTask_GrantNearbyInteraction::Activate()
{
	Super::Activate();
	
	SetWaitingOnAvatar();

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(QueryTimerHandle, this, &UD1AbilityTask_GrantNearbyInteraction::QueryInteractables, InteractionAbilityScanRate, true);
	}
}

void UD1AbilityTask_GrantNearbyInteraction::OnDestroy(bool bInOwnerFinished)
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(QueryTimerHandle);
	}
	
	Super::OnDestroy(bInOwnerFinished);
}

void UD1AbilityTask_GrantNearbyInteraction::QueryInteractables()
{
	UWorld* World = GetWorld();
	AActor* AvatarActor = GetAvatarActor();
	
	if (World && AvatarActor)
	{
		TSet<FObjectKey> RemoveKeys;
		GrantedInteractionAbilities.GetKeys(RemoveKeys);
		
		FCollisionQueryParams Params(SCENE_QUERY_STAT(UD1AbilityTask_GrantNearbyInteraction), false);

		TArray<FOverlapResult> OverlapResults;
		World->OverlapMultiByChannel(OverlapResults, AvatarActor->GetActorLocation(), FQuat::Identity, D1_TraceChannel_Interaction, FCollisionShape::MakeSphere(InteractionAbilityScanRange), Params);
		
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
			
			FD1InteractionQuery InteractionQuery;
			InteractionQuery.RequestingAvatar = AvatarActor;
			InteractionQuery.RequestingController = Cast<AController>(AvatarActor->GetOwner());
		
			TArray<FD1InteractionInfo> InteractionInfos;
			for (TScriptInterface<ID1Interactable>& Interactable : Interactables)
			{
				FD1InteractionInfoBuilder InteractionInfoBuilder(Interactable, InteractionInfos);
				Interactable->GatherPostInteractionInfos(InteractionQuery, InteractionInfoBuilder);
			}
		
			for (FD1InteractionInfo& InteractionInfo : InteractionInfos)
			{
				if (InteractionInfo.AbilityToGrant)
				{
					FObjectKey ObjectKey(InteractionInfo.AbilityToGrant);
					if (GrantedInteractionAbilities.Find(ObjectKey))
					{
						RemoveKeys.Remove(ObjectKey);
					}
					else
					{
						FGameplayAbilitySpec Spec(InteractionInfo.AbilityToGrant, 1, INDEX_NONE, this);
						FGameplayAbilitySpecHandle SpecHandle = AbilitySystemComponent->GiveAbility(Spec);
						GrantedInteractionAbilities.Add(ObjectKey, SpecHandle);
					}
				}
			}
		}
		
		for (const FObjectKey& RemoveKey : RemoveKeys)
		{
			AbilitySystemComponent->ClearAbility(GrantedInteractionAbilities[RemoveKey]);
			GrantedInteractionAbilities.Remove(RemoveKey);
		}
	}
}
