#include "D1ItemData.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif

#include "Item/Fragments/D1ItemFragment_Consumable.h"
#include "Item/Fragments/D1ItemFragment_Equippable_Armor.h"
#include "Item/Fragments/D1ItemFragment_Equippable_Weapon.h"
#include "System/LyraAssetManager.h"
#include "UObject/ObjectSaveContext.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1ItemData)

const UD1ItemData& UD1ItemData::Get()
{
	return ULyraAssetManager::Get().GetItemData();
}

#if WITH_EDITORONLY_DATA
void UD1ItemData::PreSave(FObjectPreSaveContext SaveContext)
{
	Super::PreSave(SaveContext);

	ItemTemplateIDToClass.KeySort([](const int32 A, const int32 B)
	{
		return A < B;
	});

	ItemTemplateClassToID.Empty();
	for (const auto& Pair : ItemTemplateIDToClass)
	{
		ItemTemplateClassToID.Emplace(Pair.Value, Pair.Key);
	}
}
#endif // WITH_EDITORONLY_DATA

#if WITH_EDITOR
EDataValidationResult UD1ItemData::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = Super::IsDataValid(Context);
	
	TSet<int32> ItemTemplateIDSet;
	TSet<TSubclassOf<UD1ItemTemplate>> ItemTemplateClassSet;

	for (const auto& Pair : ItemTemplateIDToClass)
	{
		// ID Check
		const int32 ItemTemplateID = Pair.Key;
		
		if (ItemTemplateID <= 0)
		{
			Context.AddError(FText::FromString(FString::Printf(TEXT("Invalid ID : [ID : %d]\n"), ItemTemplateID)));
			Result = EDataValidationResult::Invalid;
		}
		
		if (ItemTemplateIDSet.Contains(ItemTemplateID))
		{
			Context.AddError(FText::FromString(FString::Printf(TEXT("Duplicated ID : [ID : %d]\n"), ItemTemplateID)));
			Result = EDataValidationResult::Invalid;
		}
		
		ItemTemplateIDSet.Add(ItemTemplateID);

		// Class Check
		const TSubclassOf<UD1ItemTemplate> ItemTemplateClass = Pair.Value;
		
		if (ItemTemplateClass == nullptr)
		{
			Context.AddError(FText::FromString(FString::Printf(TEXT("Invalid Class : [ID : %d]\n"), ItemTemplateID)));
			Result = EDataValidationResult::Invalid;
		}

		if (ItemTemplateClassSet.Contains(ItemTemplateClass))
		{
			Context.AddError(FText::FromString(FString::Printf(TEXT("Duplicated Class : [ID : %d]\n"), ItemTemplateID)));
			Result = EDataValidationResult::Invalid;
		}
		
		ItemTemplateClassSet.Add(ItemTemplateClass);

		// Fragment Check
		if (const UD1ItemTemplate* ItemTemplate = ItemTemplateClass.GetDefaultObject())
		{
			const UD1ItemFragment_Consumable* Consumable = ItemTemplate->FindFragmentByClass<UD1ItemFragment_Consumable>();
			const UD1ItemFragment_Equippable* Equippable = ItemTemplate->FindFragmentByClass<UD1ItemFragment_Equippable>();
			const UD1ItemFragment_Equippable_Armor* Armor = ItemTemplate->FindFragmentByClass<UD1ItemFragment_Equippable_Armor>();
			const UD1ItemFragment_Equippable_Weapon* Weapon = ItemTemplate->FindFragmentByClass<UD1ItemFragment_Equippable_Weapon>();
		
			if (Equippable)
			{
				if (Armor && Weapon)
				{
					Context.AddError(FText::FromString(FString::Printf(TEXT("Conflict Fragments : [ID : %d] : [Armor] <-> [Weapon]"), ItemTemplateID)));
					Result = EDataValidationResult::Invalid;
				}

				if (Consumable)
				{
					Context.AddError(FText::FromString(FString::Printf(TEXT("Conflict Fragments : [ID : %d] : [Consumable] <-> [Equippable]"), ItemTemplateID)));
					Result = EDataValidationResult::Invalid;
				}

				if (Armor && Armor->ArmorType == EArmorType::Count)
				{
					Context.AddError(FText::FromString(FString::Printf(TEXT("Armor Type is Invalid : [ID : %d] : [EArmorType::Count]"), ItemTemplateID)));
					Result = EDataValidationResult::Invalid;
				}

				if (Weapon)
				{
					if (Weapon->WeaponType == EWeaponType::Count)
					{
						Context.AddError(FText::FromString(FString::Printf(TEXT("Weapon Type is Invalid : [ID : %d] : [EWeaponType::Count]"), ItemTemplateID)));
						Result = EDataValidationResult::Invalid;
					}
				
					if (Weapon->WeaponHandType == EWeaponHandType::Count)
					{
						Context.AddError(FText::FromString(FString::Printf(TEXT("Weapon Hand Type is Invalid : [ID : %d] : [EWeaponHandType::Count]"), ItemTemplateID)));
						Result = EDataValidationResult::Invalid;
					}
				}
			}
		}
	}
	return Result;
}
#endif // WITH_EDITOR

const UD1ItemTemplate& UD1ItemData::FindItemTemplateByID(int32 ItemTemplateID) const
{
	const TSubclassOf<UD1ItemTemplate>* ItemTemplateClass = ItemTemplateIDToClass.Find(ItemTemplateID);
	ensureAlwaysMsgf(ItemTemplateClass, TEXT("Can't find ItemTemplateClass from ID [%d]"), ItemTemplateID);
	return *(ItemTemplateClass->GetDefaultObject());
}

int32 UD1ItemData::FindItemTemplateIDByClass(TSubclassOf<UD1ItemTemplate> ItemTemplateClass) const
{
	const int32* ItemTemplateID = ItemTemplateClassToID.Find(ItemTemplateClass);
	ensureAlwaysMsgf(ItemTemplateID, TEXT("Can't find ItemTemplateID from Class"));
	return *ItemTemplateID;
}
