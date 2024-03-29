#include "D1PlayerState.h"

#include "AbilitySystem/D1AbilitySystemComponent.h"
#include "AbilitySystem/Attributes/D1AttributeSet.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1PlayerState)

AD1PlayerState::AD1PlayerState(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    AbilitySystemComponent = CreateDefaultSubobject<UD1AbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
	
	AttributeSet = CreateDefaultSubobject<UD1AttributeSet>("D1AttributeSet");

	NetUpdateFrequency = 100.f;
	MinNetUpdateFrequency = 50.f;
}

UAbilitySystemComponent* AD1PlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UD1AbilitySystemComponent* AD1PlayerState::GetD1AbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UD1AttributeSet* AD1PlayerState::GetAttributeSet() const
{
	return AttributeSet;
}
