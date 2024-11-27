#include "D1GameplayAbility_Interact_Door.h"

#include "Actors/D1DoorBase.h"
#include "Kismet/KismetMathLibrary.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1GameplayAbility_Interact_Door)

UD1GameplayAbility_Interact_Door::UD1GameplayAbility_Interact_Door(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1GameplayAbility_Interact_Door::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (TriggerEventData == nullptr || bInitialized == false)
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}

	if (HasAuthority(&CurrentActivationInfo) == false)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		return;
	}

	AD1DoorBase* DoorActor = Cast<AD1DoorBase>(InteractableActor);
	if (DoorActor == nullptr)
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}
	
	EDoorState CurrentDoorState = DoorActor->GetDoorState();
	if (CurrentDoorState == EDoorState::Open_Forward || CurrentDoorState == EDoorState::Open_Backward)
	{
		DoorActor->SetDoorState(EDoorState::Close);
	}
	else
	{
		if (const AActor* Instigator = TriggerEventData->Instigator)
		{
			FVector InteractableForward = InteractableActor->GetActorForwardVector();
			FVector InteractableToInstigator = UKismetMathLibrary::Vector_Normal2D(Instigator->GetActorLocation() - InteractableActor->GetActorLocation());
			float Dot = FVector::DotProduct(InteractableForward, InteractableToInstigator);

			DoorActor->SetDoorState(Dot < 0.f ? EDoorState::Open_Backward : EDoorState::Open_Forward);
		}
	}

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
