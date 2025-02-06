#include "D1WorldInteractable.h"

#include "D1GameplayTags.h"
#include "Character/LyraCharacter.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1WorldInteractable)

AD1WorldInteractable::AD1WorldInteractable(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bReplicates = true;
}

void AD1WorldInteractable::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, bWasConsumed);
}

void AD1WorldInteractable::OnInteractActiveStarted(AActor* Interactor)
{
	if (IsValid(Interactor) == false)
		return;
	
	if (HasAuthority())
	{
		CachedInteractors.Add(Interactor);
	}

	K2_OnInteractActiveStarted(Interactor);
}

void AD1WorldInteractable::OnInteractActiveEnded(AActor* Interactor)
{
	if (IsValid(Interactor) == false)
		return;
	
	if (HasAuthority())
	{
		CachedInteractors.RemoveSingleSwap(Interactor);
	}

	K2_OnInteractActiveEnded(Interactor);
}

void AD1WorldInteractable::OnInteractionSuccess(AActor* Interactor)
{
	if (IsValid(Interactor) == false)
		return;
	
	if (HasAuthority())
	{
		if (bShouldConsume)
		{
			bWasConsumed = true;

			TArray<TWeakObjectPtr<AActor>> MovedInteractors = MoveTemp(CachedInteractors);
		
			for (TWeakObjectPtr<AActor>& MovedInteractor : MovedInteractors)
			{
				if (ALyraCharacter* MovedCharacter = Cast<ALyraCharacter>(MovedInteractor.Get()))
				{
					if (Interactor == MovedCharacter)
						continue;
			
					if (UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(MovedCharacter))
					{
						FGameplayTagContainer CancelAbilitiesTag;
						CancelAbilitiesTag.AddTag(D1GameplayTags::Ability_Interact_Active);
						ASC->CancelAbilities(&CancelAbilitiesTag);
					}
				}
			}
		}
	}
	
	K2_OnInteractionSuccess(Interactor);
}

bool AD1WorldInteractable::CanInteraction(const FD1InteractionQuery& InteractionQuery) const
{
	return bShouldConsume ? (bWasConsumed == false) : true;
}
