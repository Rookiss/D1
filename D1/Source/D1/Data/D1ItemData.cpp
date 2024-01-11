#include "D1ItemData.h"

#include "D1LogChannels.h"
#include "Inventory/Fragments/D1ItemFragment.h"
#include "UObject/ObjectSaveContext.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1ItemData)

const UD1ItemFragment* FD1ItemDefinition::FindFragmentByClass(TSubclassOf<UD1ItemFragment> FragmentClass) const
{
	if (FragmentClass)
	{
		for (UD1ItemFragment* Fragment : Fragments)
		{
			if (Fragment && Fragment->IsA(FragmentClass))
			{
				return Fragment;
			}
		}
	}
	return nullptr;
}

void UD1ItemData::PreSave(FObjectPreSaveContext SaveContext)
{
	Super::PreSave(SaveContext);

	ItemIDToDef.Empty();

	for (const auto& Pair : ItemNameToDef)
	{
		const FString& Name = Pair.Key;
		const FD1ItemDefinition& ItemDef = Pair.Value;
		const int32 ID = ItemDef.ID;
		if (Name.IsEmpty() == false)
		{
			if (ID <= 0 || ItemIDToDef.Contains(ID))
			{
				UE_LOG(LogD1, Error, TEXT("Invalid ID or Duplicate ID [%d]"), ID);
			}
			else
			{
				ItemIDToDef.Emplace(ItemDef.ID, ItemDef);
			}
		}
	}
}

const FD1ItemDefinition& UD1ItemData::GetItemDefByID(int32 ItemID) const
{
	const FD1ItemDefinition* ItemDef = ItemIDToDef.Find(ItemID);
	ensureAlwaysMsgf(ItemDef, TEXT("Can't find Item Def from ID [%d]."), ItemID);
	return *ItemDef;
}
