#include "D1InventoryItemInstance.h"

#include "D1InventoryItemDefinition.h"
#include "Fragments/InventoryItemFragment.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1InventoryItemInstance)

UD1InventoryItemInstance::UD1InventoryItemInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1InventoryItemInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, StatTags);
	DOREPLIFETIME(ThisClass, ItemDef);
}

void UD1InventoryItemInstance::AddStatTagStack(const FGameplayTag& StatTag, int32 StackCount)
{
	StatTags.AddStack(StatTag, StackCount);
}

void UD1InventoryItemInstance::RemoveStatTagStack(const FGameplayTag& StatTag, int32 StackCount)
{
	StatTags.RemoveStack(StatTag, StackCount);
}

int32 UD1InventoryItemInstance::GetStackCountByTag(const FGameplayTag& StatTag) const
{
	return StatTags.GetStackCountByTag(StatTag);
}

bool UD1InventoryItemInstance::HasStatTag(const FGameplayTag& StatTag) const
{
	return StatTags.ContainsTag(StatTag);
}

const UInventoryItemFragment* UD1InventoryItemInstance::FindFragmentByClass(TSubclassOf<UInventoryItemFragment> FragmentClass) const
{
	if (ItemDef && FragmentClass)
	{
		return GetDefault<UD1InventoryItemDefinition>(ItemDef)->FindFragmentByClass(FragmentClass);
	}
	return nullptr;
}
