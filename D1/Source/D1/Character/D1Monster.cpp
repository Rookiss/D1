#include "D1Monster.h"

#include "AbilitySystem/D1AbilitySystemComponent.h"
#include "AbilitySystem/Attributes/D1MonsterSet.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1Monster)

AD1Monster::AD1Monster(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	NetUpdateFrequency = 100.f;

	AbilitySystemComponent = CreateDefaultSubobject<UD1AbilitySystemComponent>("D1AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
	
	MonsterSet = CreateDefaultSubobject<UD1MonsterSet>("D1MonsterSet");
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

	AbilitySystemComponent->InitAbilityActorInfo(this, this);
}

const UD1MonsterSet* AD1Monster::GetPrimarySet() const
{
	return MonsterSet;
}
