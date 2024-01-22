#include "D1ItemData.h"

#include "EditorDialogLibrary.h"
#include "Item/Fragments/D1ItemFragment_Consumable.h"
#include "Item/Fragments/D1ItemFragment_Equippable.h"
#include "Item/Fragments/D1ItemFragment_Stackable.h"
#include "UObject/ObjectSaveContext.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1ItemData)

UD1ItemData::UD1ItemData()
{
	// 28, 22, 17, 12, 9, 6, 4, 2
	ItemProbabilities.SetNum((int32)EItemRarity::Count);
	for (int32 i = 0; i < ItemProbabilities.Num(); i++)
	{
		ItemProbabilities[i].Rarity = (EItemRarity)i;
	}
}

void UD1ItemData::PreSave(FObjectPreSaveContext SaveContext)
{
	Super::PreSave(SaveContext);

	FString ErrorMsg;
	
	int32 TotalProbability = 0;
	for (const FD1ItemProbability& ItemProbability : ItemProbabilities)
	{
		TotalProbability += ItemProbability.Probability;
	}

	if (TotalProbability != 100)
	{
		ErrorMsg.Append(FString::Printf(TEXT("Total Probability is Low[%d%%] than 100%%"), TotalProbability));
	}
	
	ItemIDToDef.Empty();
	ItemNameToDef.ValueSort([](const FD1ItemDefinition& A, const FD1ItemDefinition& B)
	{
		return A.ItemID < B.ItemID;
	});
	
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
		}

		if (ItemDef.FindFragmentByClass<UD1ItemFragment_Consumable>() && ItemDef.FindFragmentByClass<UD1ItemFragment_Equippable>())
		{
			ErrorMsg.Append(FString::Printf(TEXT("Conflict Fragments : [ID : %d] : [Consumable] <-> [Equippable]"), ID));
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
