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

		TSet<FObjectKey> RemoveKeys;
		GrantedInteractionAbilities.GetKeys(RemoveKeys);
		
		if (OverlapResults.Num() > 0)
		{
			TArray<FD1InteractionInfo> InteractableInfos;
			for (const FOverlapResult& OverlapResult : OverlapResults)
			{
				if (ID1Interactable* Interactable = Cast<ID1Interactable>(OverlapResult.GetActor()))
				{
					InteractableInfos.Add(Interactable->GetInteractionInfo());
				}
			}
			
			for (FD1InteractionInfo& Info : InteractableInfos)
			{
				if (Info.InteractionAbilityToGrant)
				{
					FObjectKey ObjectKey(Info.InteractionAbilityToGrant);
					
					if (GrantedInteractionAbilities.Find(ObjectKey))
					{
						RemoveKeys.Remove(ObjectKey);
					}
					else
					{
						FGameplayAbilitySpec Spec(Info.InteractionAbilityToGrant, 1, INDEX_NONE, this);
						FGameplayAbilitySpecHandle Handle = AbilitySystemComponent->GiveAbility(Spec);
						GrantedInteractionAbilities.Add(ObjectKey, Handle);
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
