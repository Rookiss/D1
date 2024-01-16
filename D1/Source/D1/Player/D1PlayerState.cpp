#include "D1PlayerState.h"

#include "AbilitySystem/D1AbilitySystemComponent.h"
#include "AbilitySystem/Attributes/D1MonsterSet.h"
#include "AbilitySystem/Attributes/D1PlayerSet.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1PlayerState)

AD1PlayerState::AD1PlayerState(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	NetUpdateFrequency = 100.f;
	
    D1AbilitySystemComponent = CreateDefaultSubobject<UD1AbilitySystemComponent>("D1AbilitySystemComponent");
	D1AbilitySystemComponent->SetIsReplicated(true);
	D1AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
	
	D1PlayerSet = CreateDefaultSubobject<UD1PlayerSet>("D1PlayerSet");
}

UAbilitySystemComponent* AD1PlayerState::GetAbilitySystemComponent() const
{
	return D1AbilitySystemComponent;
}

UD1AbilitySystemComponent* AD1PlayerState::GetD1AbilitySystemComponent() const
{
	return D1AbilitySystemComponent;
}

const UD1PlayerSet* AD1PlayerState::GetPlayerSet() const
{
	return D1PlayerSet;
}
