#include "D1Monster.h"

#include "AbilitySystem/D1AbilitySystemComponent.h"
#include "AbilitySystem/Attributes/D1PrimarySet.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1Monster)

AD1Monster::AD1Monster(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	NetUpdateFrequency = 100.f;

	D1ASC = CreateDefaultSubobject<UD1AbilitySystemComponent>("D1AbilitySystemComponent");
	D1ASC->SetIsReplicated(true);
	D1ASC->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
	
	PrimarySet = CreateDefaultSubobject<UD1PrimarySet>("PrimarySet");
}

void AD1Monster::BeginPlay()
{
	Super::BeginPlay();

	InitAbilityActorInfo();
	
	if (HasAuthority())
	{
		ApplyAbilitySystemData(InitialAbilitySystemDataName);
	}
}

void AD1Monster::InitAbilityActorInfo()
{
	Super::InitAbilityActorInfo();

	D1ASC->InitAbilityActorInfo(this, this);
}

const UD1PrimarySet* AD1Monster::GetPrimarySet() const
{
	return PrimarySet;
}
