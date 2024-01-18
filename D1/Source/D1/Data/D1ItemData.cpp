#include "D1ItemData.h"

#include "EditorDialogLibrary.h"
#include "Inventory/Fragments/D1ItemFragment_Equippable.h"
#include "Inventory/Fragments/D1ItemFragment_FixedStats.h"
#include "Inventory/Fragments/D1ItemFragment_RandomStats.h"
#include "Inventory/Fragments/D1ItemFragment_Stackable.h"
#include "UObject/ObjectSaveContext.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1ItemData)

void UD1ItemData::PreSave(FObjectPreSaveContext SaveContext)
{
	Super::PreSave(SaveContext);
	
	ItemIDToDef.Empty();

	ItemNameToDef.ValueSort([](const FD1ItemDefinition& A, const FD1ItemDefinition& B)
	{
		return A.ItemID < B.ItemID;
	});
	
	FString ErrorMsg;
	for (const auto& Pair : ItemNameToDef)
	{
		const FString& Name = Pair.Key;
		const FD1ItemDefinition& ItemDef = Pair.Value;
		const int32 ID = ItemDef.ItemID;
		
		if (ID <= 0 || ItemIDToDef.Contains(ID))
		{
			ErrorMsg.Append(FString::Printf(TEXT("Invalid or Duplicated ID : [Name : %s] - [ID : %d]\n"), *Name, ID));
			continue;
		}
		
		if (ItemDef.FindFragmentByClass<UD1ItemFragment_Stackable>())
		{
			if (ItemDef.FindFragmentByClass<UD1ItemFragment_Equippable>())
			{
				ErrorMsg.Append(FString::Printf(TEXT("Conflict Fragments : [ID : %d] : [Stackable] <-> [Equippable]"), ID));
				continue;
			}

			if (ItemDef.FindFragmentByClass<UD1ItemFragment_RandomStats>())
			{
				ErrorMsg.Append(FString::Printf(TEXT("Conflict Fragments : [ID : %d] : [Stackable] <-> [RandomStats]"), ID));
				continue;
			}
		}

		if (ItemDef.FindFragmentByClass<UD1ItemFragment_FixedStats>() && ItemDef.FindFragmentByClass<UD1ItemFragment_RandomStats>())
		{
			ErrorMsg.Append(FString::Printf(TEXT("Conflict Fragments : [ID : %d] : [FixedStats] <-> [RandomStats]"), ID));
			continue;
		}
		
		ItemIDToDef.Emplace(ItemDef.ItemID, ItemDef);
	}

	if (ErrorMsg.IsEmpty() == false)
	{
		UEditorDialogLibrary::ShowMessage(FText::FromString(TEXT("Invalid Data Asset")), FText::FromString(ErrorMsg),
			EAppMsgType::Ok, EAppReturnType::Ok, EAppMsgCategory::Error);
	}
}

const FD1ItemDefinition& UD1ItemData::GetItemDefByID(int32 ItemID) const
{
	const FD1ItemDefinition* ItemDef = ItemIDToDef.Find(ItemID);
	ensureAlwaysMsgf(ItemDef, TEXT("Can't find Item Def from ID [%d]."), ItemID);
	return *ItemDef;
}
