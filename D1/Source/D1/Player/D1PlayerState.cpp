#include "D1PlayerState.h"

#include "AbilitySystem/D1AbilitySystemComponent.h"
#include "AbilitySystem/Attributes/D1PlayerSet.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1PlayerState)

AD1PlayerState::AD1PlayerState(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	NetUpdateFrequency = 100.f;
	
    AbilitySystemComponent = CreateDefaultSubobject<UD1AbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
	
	PlayerSet = CreateDefaultSubobject<UD1PlayerSet>("PlayerSet");
}

UAbilitySystemComponent* AD1PlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UD1AbilitySystemComponent* AD1PlayerState::GetD1AbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

const UD1PlayerSet* AD1PlayerState::GetPlayerSet() const
{
	return PlayerSet;
}
