#include "D1ContractManagerComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1ContractManagerComponent)

UD1ContractManagerComponent::UD1ContractManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1ContractManagerComponent::AddAllowedActor(AActor* Actor)
{
	AllowedActors.Add(Actor);
}

void UD1ContractManagerComponent::RemoveAllowedActor(AActor* Actor)
{
	AllowedActors.Remove(Actor);
}

bool UD1ContractManagerComponent::IsAllowedActor(AActor* Actor) const
{
	return AllowedActors.Contains(TWeakObjectPtr<AActor>(Actor));
}
