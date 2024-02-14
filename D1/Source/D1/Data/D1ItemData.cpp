#include "D1ItemData.h"

#include "Item/Fragments/D1ItemFragment_Consumable.h"
#include "Item/Fragments/D1ItemFragment_Stackable.h"
#include "Item/Fragments/D1ItemFragment_Equippable_Armor.h"
#include "Item/Fragments/D1ItemFragment_Equippable_Weapon.h"
#include "UObject/ObjectSaveContext.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif


#include UE_INLINE_GENERATED_CPP_BY_NAME(D1ItemData)

UD1ItemData::UD1ItemData()
{
	ItemRarityProbabilities.SetNum(static_cast<int32>(EItemRarity::Count));
	for (int32 i = 0; i < ItemRarityProbabilities.Num(); i++)
	{
		ItemRarityProbabilities[i].Rarity = static_cast<EItemRarity>(i);
	}
}

void UD1ItemData::PreSave(FObjectPreSaveContext SaveContext)
{
	Super::PreSave(SaveContext);
	
	ItemIDToDef.Empty();
	ItemNameToDef.ValueSort([](const FD1ItemDefinition& A, const FD1ItemDefinition& B)
	{
		return A.ItemID < B.ItemID;
	});
	
	for (const auto& Pair : ItemNameToDef)
	{
		const FD1ItemDefinition& ItemDef = Pair.Value;
		ItemIDToDef.Emplace(ItemDef.ItemID, ItemDef);
	}
}

#if WITH_EDITOR
EDataValidationResult UD1ItemData::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = Super::IsDataValid(Context);

	int32 TotalProbability = 0;
	for (const FD1ItemRarityProbability& ItemRarityProbability : ItemRarityProbabilities)
	{
		TotalProbability += ItemRarityProbability.Probability;
	}

	if (TotalProbability != 100)
	{
		Context.AddError(FText::FromString(FString::Printf(TEXT("Total Probability [%d%%] is not 100%%"), TotalProbability)));
		Result = EDataValidationResult::Invalid;
	}

	TSet<int32> ItemIDSet;
	
	for (const auto& Pair : ItemNameToDef)
	{
		const FString& Name = Pair.Key;
		const FD1ItemDefinition& ItemDef = Pair.Value;
		const int32 ItemID = ItemDef.ItemID;

		const UD1ItemFragment_Consumable* Consumable = ItemDef.FindFragmentByClass<UD1ItemFragment_Consumable>();
		const UD1ItemFragment_Equippable* Equippable = ItemDef.FindFragmentByClass<UD1ItemFragment_Equippable>();
		const UD1ItemFragment_Equippable_Armor* Armor = ItemDef.FindFragmentByClass<UD1ItemFragment_Equippable_Armor>();
		const UD1ItemFragment_Equippable_Weapon* Weapon = ItemDef.FindFragmentByClass<UD1ItemFragment_Equippable_Weapon>();
		const UD1ItemFragment_Stackable* Stackable = ItemDef.FindFragmentByClass<UD1ItemFragment_Stackable>();
		
		if (ItemID <= 0)
		{
			Context.AddError(FText::FromString(FString::Printf(TEXT("Invalid ID : [ID : %d] - [Name : %s]\n"), ItemID, *Name)));
			Result = EDataValidationResult::Invalid;
		}
		
		if (ItemIDSet.Contains(ItemID))
		{
			Context.AddError(FText::FromString(FString::Printf(TEXT("Duplicated ID : [ID : %d] - [Name : %s]\n"), ItemID, *Name)));
			Result = EDataValidationResult::Invalid;
		}
		ItemIDSet.Add(ItemID);

		if (Equippable)
		{
			if (Armor && Weapon)
			{
				Context.AddError(FText::FromString(FString::Printf(TEXT("Conflict Fragments : [ID : %d] : [Armor] <-> [Weapon]"), ItemID)));
				Result = EDataValidationResult::Invalid;
			}
			
			if (Stackable)
			{
				Context.AddError(FText::FromString(FString::Printf(TEXT("Conflict Fragments : [ID : %d] : [Stackable] <-> [Equippable]"), ItemID)));
				Result = EDataValidationResult::Invalid;
			}

			if (Consumable)
			{
				Context.AddError(FText::FromString(FString::Printf(TEXT("Conflict Fragments : [ID : %d] : [Consumable] <-> [Equippable]"), ItemID)));
				Result = EDataValidationResult::Invalid;
			}

			if (Armor && Armor->ArmorType == EArmorType::Count)
			{
				Context.AddError(FText::FromString(FString::Printf(TEXT("Armor Type is Invalid : [ID : %d] : [EArmorType::Count]"), ItemID)));
				Result = EDataValidationResult::Invalid;
			}

			if (Weapon)
			{
				if (Weapon->WeaponType == EWeaponType::Count)
				{
					Context.AddError(FText::FromString(FString::Printf(TEXT("Weapon Type is Invalid : [ID : %d] : [EWeaponType::Count]"), ItemID)));
					Result = EDataValidationResult::Invalid;
				}
				
				if (Weapon->WeaponHandType == EWeaponHandType::Count)
				{
					Context.AddError(FText::FromString(FString::Printf(TEXT("Weapon Hand Type is Invalid : [ID : %d] : [EWeaponHandType::Count]"), ItemID)));
					Result = EDataValidationResult::Invalid;
				}
			}
		}
	}
	return Result;
}
#endif // #if WITH_EDITOR

const FD1ItemDefinition& UD1ItemData::GetItemDefByID(int32 ItemID) const
{
	const FD1ItemDefinition* ItemDef = ItemIDToDef.Find(ItemID);
	ensureAlwaysMsgf(ItemDef, TEXT("Can't find Item Def from ID [%d]."), ItemID);
	return *ItemDef;
}
