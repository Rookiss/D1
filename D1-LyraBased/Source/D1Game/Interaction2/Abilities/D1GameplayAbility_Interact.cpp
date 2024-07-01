#include "D1GameplayAbility_Interact.h"

#include "AbilitySystemComponent.h"
#include "D1GameplayTags.h"
#include "Interaction/Tasks/AbilityTask_GrantNearbyInteraction.h"
#include "Interaction2/D1Interactable.h"
#include "Interaction2/D1InteractionInfo.h"
#include "Player/LyraPlayerController.h"
#include "UI/IndicatorSystem/IndicatorDescriptor.h"
#include "UI/IndicatorSystem/LyraIndicatorManagerComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1GameplayAbility_Interact)

struct FInteractionOption;

UD1GameplayAbility_Interact::UD1GameplayAbility_Interact(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ActivationPolicy = ELyraAbilityActivationPolicy::OnSpawn;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

void UD1GameplayAbility_Interact::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	UAbilitySystemComponent* AbilitySystem = GetAbilitySystemComponentFromActorInfo();
	if (AbilitySystem && AbilitySystem->GetOwnerRole() == ROLE_Authority)
	{
		UAbilityTask_GrantNearbyInteraction* Task = UAbilityTask_GrantNearbyInteraction::GrantAbilitiesForNearbyInteractors(this, InteractionScanRange, InteractionScanRate);
		Task->ReadyForActivation();
	}
}

void UD1GameplayAbility_Interact::UpdateInteractions(const TArray<FD1InteractionInfo>& InteractionInfos)
{
	if (ALyraPlayerController* PlayerController = GetLyraPlayerControllerFromActorInfo())
	{
		if (ULyraIndicatorManagerComponent* IndicatorManager = ULyraIndicatorManagerComponent::GetComponent(PlayerController))
		{
			for (UIndicatorDescriptor* Indicator : Indicators)
			{
				IndicatorManager->RemoveIndicator(Indicator);
			}
			Indicators.Reset();

			for (const FD1InteractionInfo& InteractionInfo : InteractionInfos)
			{
				AActor* InteractableActor = nullptr;
				if (UObject* Object = InteractionInfo.Interactable.GetObject())
				{
					if (AActor* Actor = Cast<AActor>(Object))
					{
						InteractableActor = Actor;
					}
					else if (UActorComponent* ActorComponent = Cast<UActorComponent>(Object))
					{
						InteractableActor = ActorComponent->GetOwner();
					}
				}
				
				TSoftClassPtr<UUserWidget> InteractionWidgetClass = InteractionInfo.InteractionWidgetClass.IsNull() ? DefaultInteractionWidgetClass : InteractionInfo.InteractionWidgetClass;

				UIndicatorDescriptor* Indicator = NewObject<UIndicatorDescriptor>();
				Indicator->SetDataObject(InteractableActor);
				Indicator->SetSceneComponent(InteractableActor->GetRootComponent());
				Indicator->SetIndicatorClass(InteractionWidgetClass);
				IndicatorManager->AddIndicator(Indicator);

				Indicators.Add(Indicator);
			}
		}
	}

	CurrentInteractionInfos = InteractionInfos;
}

void UD1GameplayAbility_Interact::TriggerInteraction()
{
	if (CurrentInteractionInfos.Num() == 0)
		return;
	
	if (GetAbilitySystemComponentFromActorInfo())
	{
		const FD1InteractionInfo& InteractionInfo = CurrentInteractionInfos[0];

		AActor* Instigator = GetAvatarActorFromActorInfo();
		AActor* InteractableActor = nullptr;

		if (UObject* Object = InteractionInfo.Interactable.GetObject())
		{
			if (AActor* Actor = Cast<AActor>(Object))
			{
				InteractableActor = Actor;
			}
			else if (UActorComponent* ActorComponent = Cast<UActorComponent>(Object))
			{
				InteractableActor = ActorComponent->GetOwner();
			}
		}
		
		FGameplayEventData Payload;
		Payload.EventTag = D1GameplayTags::Ability_Interact_Active;
		Payload.Instigator = Instigator;
		Payload.Target = InteractableActor;
		
		InteractionInfo.Interactable->CustomizeInteractionEventData(D1GameplayTags::Ability_Interact_Active, Payload);
		
		AActor* TargetActor = const_cast<AActor*>(ToRawPtr(Payload.Target));
		
		FGameplayAbilityActorInfo ActorInfo;
		ActorInfo.InitFromActor(InteractableActor, TargetActor, InteractionInfo.TargetAbilitySystem);
		
		const bool bSuccess = InteractionInfo.TargetAbilitySystem->TriggerAbilityFromGameplayEvent(
			InteractionInfo.TargetInteractionAbilityHandle,
			&ActorInfo,
			D1GameplayTags::Ability_Interact_Active,
			&Payload,
			*InteractionInfo.TargetAbilitySystem
		);
	}
}
