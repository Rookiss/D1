#include "D1ItemInstance.h"

#include "Data/D1ItemData.h"
#include "Fragments/D1ItemFragment.h"
#include "Net/UnrealNetwork.h"
#include "System/D1AssetManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1InventoryItemInstance)

UD1ItemInstance::UD1ItemInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1ItemInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, ItemID);
	DOREPLIFETIME(ThisClass, StatTags);
}

void UD1ItemInstance::AddStatTagStack(const FGameplayTag& StatTag, int32 StackCount)
{
	StatTags.AddStack(StatTag, StackCount);
}

void UD1ItemInstance::RemoveStatTagStack(const FGameplayTag& StatTag, int32 StackCount)
{
	StatTags.RemoveStack(StatTag, StackCount);
}

int32 UD1ItemInstance::GetStackCountByTag(const FGameplayTag& StatTag) const
{
	return StatTags.GetStackCountByTag(StatTag);
}

bool UD1ItemInstance::HasStatTag(const FGameplayTag& StatTag) const
{
	return StatTags.ContainsTag(StatTag);
}

const UD1ItemFragment* UD1ItemInstance::FindFragmentByClass(TSubclassOf<UD1ItemFragment> FragmentClass) const
{
	if (const UD1ItemData* ItemData = UD1AssetManager::GetAssetByName<UD1ItemData>("ItemData"))
	{
		const FD1ItemDefinition& ItemDef = ItemData->GetItemDefByID(ItemID);
		return ItemDef.FindFragmentByClass(FragmentClass);
	}
	return nullptr;
}
