#include "D1ItemData.h"

#include "EditorDialogLibrary.h"
#include "Inventory/Fragments/D1ItemFragment.h"
#include "UObject/ObjectSaveContext.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1ItemData)

void UD1ItemData::PreSave(FObjectPreSaveContext SaveContext)
{
	Super::PreSave(SaveContext);
	
	ItemIDToDef.Empty();

	ItemNameToDef.ValueSort([](const FD1ItemDefinition& A, const FD1ItemDefinition& B)
	{
		return A.ID < B.ID;
	});
	
	FString ErrorMsg;
	for (const auto& Pair : ItemNameToDef)
	{
		const FString& Name = Pair.Key;
		const FD1ItemDefinition& ItemDef = Pair.Value;
		const int32 ID = ItemDef.ID;
		
		if (ID <= 0 || ItemIDToDef.Contains(ID))
		{
			ErrorMsg.Append(FString::Printf(TEXT("Invalid ID [Name : %s] [ID : %d]\n"), *Name, ID));
			continue;
		}

		// TODO: Check Fragments
		
		ItemIDToDef.Emplace(ItemDef.ID, ItemDef);
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
