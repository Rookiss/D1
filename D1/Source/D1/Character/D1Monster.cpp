#include "D1Monster.h"

#include "AbilitySystem/D1AbilitySystemComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1Monster)

AD1Monster::AD1Monster(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AbilitySystemComponent = CreateDefaultSubobject<UD1AbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
}

void AD1Monster::BeginPlay()
{
	Super::BeginPlay();

	InitAbilitySystem();
}

void AD1Monster::InitAbilitySystem()
{
	Super::InitAbilitySystem();

	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	ApplyAbilitySystemData("AbilitySystemData_Monster");
}
