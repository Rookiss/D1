#include "D1AbilitySystemComponent.h"

#include "D1GameplayTags.h"
#include "D1GlobalAbilitySystem.h"
#include "D1LogChannels.h"
#include "Abilities/D1GameplayAbility.h"
#include "Animation/D1AnimInstance.h"
#include "Data/D1AbilitySystemData.h"
#include "System/D1AssetManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1AbilitySystemComponent)

UD1AbilitySystemComponent::UD1AbilitySystemComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}

void UD1AbilitySystemComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UD1GlobalAbilitySystem* GlobalAbilitySystem = UWorld::GetSubsystem<UD1GlobalAbilitySystem>(GetWorld()))
	{
		GlobalAbilitySystem->UnregisterASC(this);
	}
	
	Super::EndPlay(EndPlayReason);
}

void UD1AbilitySystemComponent::InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor)
{
	FGameplayAbilityActorInfo* ActorInfo = AbilityActorInfo.Get();
	check(ActorInfo);
	check(InOwnerActor);

	const bool bHasNewPawnAvatar = Cast<APawn>(InAvatarActor) && (InAvatarActor != ActorInfo->AvatarActor);
	
	Super::InitAbilityActorInfo(InOwnerActor, InAvatarActor);

	if (bHasNewPawnAvatar)
	{
		if (UD1GlobalAbilitySystem* GlobalAbilitySystem = UWorld::GetSubsystem<UD1GlobalAbilitySystem>(GetWorld()))
		{
			GlobalAbilitySystem->RegisterASC(this);
		}

		if (UD1AnimInstance* AnimInstance = Cast<UD1AnimInstance>(ActorInfo->GetAnimInstance()))
		{
			AnimInstance->InitializedWithAbilitySystem(this);
		}

		TryActivateAbilitiesOnSpawn();
	}
}

void UD1AbilitySystemComponent::OnGiveAbility(FGameplayAbilitySpec& AbilitySpec)
{
	Super::OnGiveAbility(AbilitySpec);

	for (const FGameplayTag& AbilityTag : AbilitySpec.Ability->AbilityTags)
	{
		if (AbilityTag.MatchesTag(FGameplayTag::RequestGameplayTag("Ability")) && AbilityChangedDelegate.IsBound())
		{
			AbilityChangedDelegate.Broadcast(true, AbilityTag);
			return;
		}
	}
}

void UD1AbilitySystemComponent::OnRemoveAbility(FGameplayAbilitySpec& AbilitySpec)
{
	Super::OnRemoveAbility(AbilitySpec);

	for (const FGameplayTag& Tag : AbilitySpec.Ability->AbilityTags)
	{
		if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag("Ability")) && AbilityChangedDelegate.IsBound())
		{
			AbilityChangedDelegate.Broadcast(false, Tag);
			return;
		}
	}
}

void UD1AbilitySystemComponent::TryActivateAbilitiesOnSpawn()
{
	ABILITYLIST_SCOPE_LOCK();
	for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
	{
		const UD1GameplayAbility* AbilityCDO = CastChecked<UD1GameplayAbility>(AbilitySpec.Ability);
		AbilityCDO->TryActivateAbilityOnGiveOrSpawn(AbilityActorInfo.Get(), AbilitySpec);
	}
}

void UD1AbilitySystemComponent::CancelAbilitiesByFunc(TShouldCancelAbilityFunc ShouldCancelFunc, bool bReplicateCancelAbility)
{
	ABILITYLIST_SCOPE_LOCK();
	for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
	{
		if (AbilitySpec.IsActive() == false)
			continue;

		UD1GameplayAbility* AbilityCDO = CastChecked<UD1GameplayAbility>(AbilitySpec.Ability);
		if (AbilityCDO->GetInstancingPolicy() != EGameplayAbilityInstancingPolicy::NonInstanced)
		{
			TArray<UGameplayAbility*> Instances = AbilitySpec.GetAbilityInstances();
			for (UGameplayAbility* AbilityInstance : Instances)
			{
				UD1GameplayAbility* D1AbilityInstance = CastChecked<UD1GameplayAbility>(AbilityInstance);
				if (ShouldCancelFunc(D1AbilityInstance, AbilitySpec.Handle))
				{
					if (D1AbilityInstance->CanBeCanceled())
					{
						D1AbilityInstance->CancelAbility(AbilitySpec.Handle, AbilityActorInfo.Get(), D1AbilityInstance->GetCurrentActivationInfo(), bReplicateCancelAbility);
					}
					else
					{
						UE_LOG(LogD1AbilitySystem, Error, TEXT("CancelAbilitiesByFunc : Can't cancel ability [%s] because CanBeCanceled is false"), *D1AbilityInstance->GetName());
					}
				}
			}
		}
		else
		{
			if (ShouldCancelFunc(AbilityCDO, AbilitySpec.Handle))
			{
				check(AbilityCDO->CanBeCanceled());
				AbilityCDO->CancelAbility(AbilitySpec.Handle, AbilityActorInfo.Get(), FGameplayAbilityActivationInfo(), bReplicateCancelAbility);
			}
		}
	}
}

void UD1AbilitySystemComponent::CancelInputActivatedAbilities(bool bReplicateCancelAbility)
{
	auto ShouldCancelFunc = [this](const UD1GameplayAbility* Ability, FGameplayAbilitySpecHandle Handle)
	{
		const ED1AbilityActivationPolicy ActivationPolicy = Ability->GetActivationPolicy();
		return ((ActivationPolicy == ED1AbilityActivationPolicy::InputPressed) || (ActivationPolicy == ED1AbilityActivationPolicy::InputHeld));
	};

	CancelAbilitiesByFunc(ShouldCancelFunc, bReplicateCancelAbility);
}

void UD1AbilitySystemComponent::AbilitySpecInputPressed(FGameplayAbilitySpec& Spec)
{
	Super::AbilitySpecInputPressed(Spec);

	if (Spec.IsActive())
	{
		InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed, Spec.Handle, Spec.ActivationInfo.GetActivationPredictionKey());
	}
}

void UD1AbilitySystemComponent::AbilitySpecInputReleased(FGameplayAbilitySpec& Spec)
{
	Super::AbilitySpecInputReleased(Spec);

	if (Spec.IsActive())
	{
		InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputReleased, Spec.Handle, Spec.ActivationInfo.GetActivationPredictionKey());
	}
}

void UD1AbilitySystemComponent::AbilityInputTagPressed(const FGameplayTag& InputTag)
{
	if (InputTag.IsValid())
	{
		for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
		{
			if (AbilitySpec.Ability && AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
			{
				InputPressedSpecHandles.AddUnique(AbilitySpec.Handle);
				InputHeldSpecHandles.AddUnique(AbilitySpec.Handle);
			}
		}
	}
}

void UD1AbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
	if (InputTag.IsValid())
	{
		for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
		{
			if (AbilitySpec.Ability && AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
			{
				InputReleasedSpecHandles.AddUnique(AbilitySpec.Handle);
				InputHeldSpecHandles.Remove(AbilitySpec.Handle);
			}
		}
	}
}

void UD1AbilitySystemComponent::ProcessAbilityInput(float DeltaTime, bool bGamePaused)
{
	if (HasMatchingGameplayTag(D1GameplayTags::ASC_InputBlocked))
	{
		ClearAbilityInput();
		return;
	}
	
	static TArray<FGameplayAbilitySpecHandle> AbilitiesToActivate;
	AbilitiesToActivate.Reset();
	
	for (const FGameplayAbilitySpecHandle& SpecHandle : InputHeldSpecHandles)
	{
		if (const FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle))
		{
			if (AbilitySpec->Ability && AbilitySpec->IsActive() == false)
			{
				const UD1GameplayAbility* AbilityCDO = CastChecked<UD1GameplayAbility>(AbilitySpec->Ability);
				if (AbilityCDO->GetActivationPolicy() == ED1AbilityActivationPolicy::InputHeld)
				{
					AbilitiesToActivate.AddUnique(AbilitySpec->Handle);
				}
			}
		}
	}
	
	for (const FGameplayAbilitySpecHandle& SpecHandle : InputPressedSpecHandles)
	{
		if (FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle))
		{
			if (AbilitySpec->Ability)
			{
				AbilitySpec->InputPressed = true;

				if (AbilitySpec->IsActive())
				{
					AbilitySpecInputPressed(*AbilitySpec);
				}
				else
				{
					const UD1GameplayAbility* D1AbilityCDO = CastChecked<UD1GameplayAbility>(AbilitySpec->Ability);
					if (D1AbilityCDO->GetActivationPolicy() == ED1AbilityActivationPolicy::InputPressed)
					{
						AbilitiesToActivate.AddUnique(AbilitySpec->Handle);
					}
				}
			}
		}
	}

	for (const FGameplayAbilitySpecHandle& AbilitySpecHandle : AbilitiesToActivate)
	{
		TryActivateAbility(AbilitySpecHandle);
	}

	for (const FGameplayAbilitySpecHandle& SpecHandle : InputReleasedSpecHandles)
	{
		if (FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle))
		{
			if (AbilitySpec->Ability)
			{
				AbilitySpec->InputPressed = false;

				if (AbilitySpec->IsActive())
				{
   					AbilitySpecInputReleased(*AbilitySpec);
				}
			}
		}
	}

	InputPressedSpecHandles.Reset();
	InputReleasedSpecHandles.Reset();
}

void UD1AbilitySystemComponent::ClearAbilityInput()
{
	InputPressedSpecHandles.Reset();
	InputHeldSpecHandles.Reset();
	InputReleasedSpecHandles.Reset();
}

void UD1AbilitySystemComponent::AddDynamicTagToSelf(const FGameplayTag& Tag)
{
	const TSubclassOf<UGameplayEffect> DynamicTagEffectClass = UD1AssetManager::GetSubclassByName<UGameplayEffect>("DynamicTagEffect");
	if (DynamicTagEffectClass == nullptr)
	{
		UE_LOG(LogD1AbilitySystem, Warning, TEXT("AddDynamicTagGameplayEffect : Unable to find DynamicTagGameplayEffect."));
		return;
	}

	const FGameplayEffectSpecHandle SpecHandle = MakeOutgoingSpec(DynamicTagEffectClass, 1.f, MakeEffectContext());
	FGameplayEffectSpec* Spec = SpecHandle.Data.Get();
	if (Spec == nullptr)
	{
		UE_LOG(LogD1AbilitySystem, Warning, TEXT("AddDynamicTagGameplayEffect : Unable to make outgoing spec for [%s]."), *GetNameSafe(DynamicTagEffectClass));
		return;
	}

	Spec->DynamicGrantedTags.AddTag(Tag);
	ApplyGameplayEffectSpecToSelf(*Spec);
}

void UD1AbilitySystemComponent::RemoveDynamicTagToSelf(const FGameplayTag& Tag)
{
	const TSubclassOf<UGameplayEffect> DynamicTagEffectClass = UD1AssetManager::GetSubclassByName<UGameplayEffect>("DynamicTagEffect");
	if (DynamicTagEffectClass == nullptr)
	{
		UE_LOG(LogD1AbilitySystem, Warning, TEXT("AddDynamicTagGameplayEffect : Unable to find DynamicTagGameplayEffect."));
		return;
	}

	FGameplayEffectQuery Query = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(FGameplayTagContainer(Tag));
	Query.EffectDefinition = DynamicTagEffectClass;
	RemoveActiveEffects(Query);
}
