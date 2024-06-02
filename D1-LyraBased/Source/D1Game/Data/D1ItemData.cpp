#include "D1ItemData.h"

#include "Item/Fragments/D1ItemFragment_Consumable.h"
#include "Item/Fragments/D1ItemFragment_Equippable_Armor.h"
#include "Item/Fragments/D1ItemFragment_Equippable_Weapon.h"
#include "UObject/ObjectSaveContext.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif

#include "System/LyraAssetManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1ItemData)

const UD1ItemData& UD1ItemData::Get()
{
	return ULyraAssetManager::Get().GetItemData();
}

void UD1ItemData::PreSave(FObjectPreSaveContext SaveContext)
{
	Super::PreSave(SaveContext);
	
	ItemIDToTemplate.Empty();
	ItemNameToTemplate.ValueSort([](const FD1ItemTemplate& A, const FD1ItemTemplate& B)
	{
		return A.TemplateID < B.TemplateID;
	});
	
	for (const auto& Pair : ItemNameToTemplate)
	{
		const FD1ItemTemplate& ItemDef = Pair.Value;
		ItemIDToTemplate.Emplace(ItemDef.TemplateID, ItemDef);
	}
}

#if WITH_EDITOR
EDataValidationResult UD1ItemData::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = Super::IsDataValid(Context);
	
	TSet<int32> ItemIDSet;
	
	for (const auto& Pair : ItemNameToTemplate)
	{
		const FString& Name = Pair.Key;
		const FD1ItemTemplate& ItemDef = Pair.Value;
		const int32 ItemID = ItemDef.TemplateID;

		const UD1ItemFragment_Consumable* Consumable = ItemDef.FindFragmentByClass<UD1ItemFragment_Consumable>();
		const UD1ItemFragment_Equippable* Equippable = ItemDef.FindFragmentByClass<UD1ItemFragment_Equippable>();
		const UD1ItemFragment_Equippable_Armor* Armor = ItemDef.FindFragmentByClass<UD1ItemFragment_Equippable_Armor>();
		const UD1ItemFragment_Equippable_Weapon* Weapon = ItemDef.FindFragmentByClass<UD1ItemFragment_Equippable_Weapon>();
		// const UD1ItemFragment_Stackable* Stackable = ItemDef.FindFragmentByClass<UD1ItemFragment_Stackable>();
		
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
			
			// if (Stackable)
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

const FD1ItemTemplate& UD1ItemData::FindItemTemplateByID(int32 TemplateID) const
{
	const FD1ItemTemplate* ItemTemplate = ItemIDToTemplate.Find(TemplateID);
	ensureAlwaysMsgf(ItemTemplate, TEXT("Can't find ItemTemplate from ID [%d]."), TemplateID);
	return *ItemTemplate;
}
