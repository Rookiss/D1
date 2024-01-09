#include "D1InventoryItemInstance.h"

#include "D1InventoryItemDefinition.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1InventoryItemInstance)

UD1InventoryItemInstance::UD1InventoryItemInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1InventoryItemInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	UObject::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, ItemDef);
}

const UD1InventoryItemFragment* UD1InventoryItemInstance::FindFragmentByClass(TSubclassOf<UD1InventoryItemFragment> FragmentClass) const
{
	if (ItemDef && FragmentClass)
	{
		return GetDefault<UD1InventoryItemDefinition>(ItemDef)->FindFragmentByClass(FragmentClass);
	}
	return nullptr;
}
