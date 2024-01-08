#include "D1PlayerState.h"

#include "AbilitySystem/D1AbilitySystemComponent.h"
#include "AbilitySystem/Attributes/D1PrimarySet.h"
#include "AbilitySystem/Attributes/D1SecondarySet.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1PlayerState)

AD1PlayerState::AD1PlayerState(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	NetUpdateFrequency = 100.f;
	
    D1ASC = CreateDefaultSubobject<UD1AbilitySystemComponent>("D1AbilitySystemComponent");
	D1ASC->SetIsReplicated(true);
	D1ASC->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	PrimarySet = CreateDefaultSubobject<UD1PrimarySet>("PrimarySet");
	SecondarySet = CreateDefaultSubobject<UD1SecondarySet>("SecondarySet");
}

UAbilitySystemComponent* AD1PlayerState::GetAbilitySystemComponent() const
{
	return D1ASC;
}

UD1AbilitySystemComponent* AD1PlayerState::GetD1AbilitySystemComponent() const
{
	return D1ASC;
}

const UD1PrimarySet* AD1PlayerState::GetPrimarySet() const
{
	return PrimarySet;
}

const UD1SecondarySet* AD1PlayerState::GetSecondarySet() const
{
	return SecondarySet;
}
