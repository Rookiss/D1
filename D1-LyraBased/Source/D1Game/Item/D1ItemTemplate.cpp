#include "D1ItemTemplate.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif // WITH_EDITOR

#include "Fragments/D1ItemFragment_Equipable.h"
#include "Fragments/D1ItemFragment_Equipable_Armor.h"
#include "Fragments/D1ItemFragment_Equipable_Utility.h"
#include "Fragments/D1ItemFragment_Equipable_Weapon.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1ItemTemplate)

UD1ItemTemplate::UD1ItemTemplate(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

#if WITH_EDITOR
EDataValidationResult UD1ItemTemplate::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = UObject::IsDataValid(Context);

	if (SlotCount.X < 1 || SlotCount.Y < 1)
	{
		Context.AddError(FText::FromString(FString::Printf(TEXT("SlotCount is less than 1"))));
		Result = EDataValidationResult::Invalid;
	}
	
	if (MaxStackCount < 1)
	{
		Context.AddError(FText::FromString(FString::Printf(TEXT("MaxStackCount is less than 1"))));
		Result = EDataValidationResult::Invalid;
	}
	
	const UD1ItemFragment_Equipable* FoundEquipable = nullptr;
	for (UD1ItemFragment* Fragment : Fragments)
	{
		if (UD1ItemFragment_Equipable* CurrentEquippable = Cast<UD1ItemFragment_Equipable>(Fragment))
		{
			if (FoundEquipable)
			{
				Context.AddError(FText::FromString(FString::Printf(TEXT("Duplicated Equippable Fragment"))));
				return EDataValidationResult::Invalid;
			}

			FoundEquipable = CurrentEquippable;
		}
	}
	
	if (FoundEquipable)
	{
		if (FoundEquipable->EquipmentType == EEquipmentType::Count)
		{
			Context.AddError(FText::FromString(FString::Printf(TEXT("Equipment Type is Invalid : [EEquipmentType::Count]"))));
			return EDataValidationResult::Invalid;
		}
		
		if (FoundEquipable->EquipmentType == EEquipmentType::Armor)
		{
			const UD1ItemFragment_Equipable_Armor* ArmorFragment = Cast<UD1ItemFragment_Equipable_Armor>(FoundEquipable);
			
			if (ArmorFragment->ArmorType == EArmorType::Count)
			{
				Context.AddError(FText::FromString(FString::Printf(TEXT("Armor Type is Invalid : [EArmorType::Count]"))));
				Result = EDataValidationResult::Invalid;
			}
		}
		else if (FoundEquipable->EquipmentType == EEquipmentType::Weapon)
		{
			const UD1ItemFragment_Equipable_Weapon* WeaponFragment = Cast<UD1ItemFragment_Equipable_Weapon>(FoundEquipable);
			
			if (WeaponFragment->WeaponType == EWeaponType::Count)
			{
				Context.AddError(FText::FromString(FString::Printf(TEXT("Weapon Type is Invalid : [EWeaponType::Count]"))));
				Result = EDataValidationResult::Invalid;
			}
				
			if (WeaponFragment->WeaponHandType == EWeaponHandType::Count)
			{
				Context.AddError(FText::FromString(FString::Printf(TEXT("Weapon Hand Type is Invalid : [EWeaponHandType::Count]"))));
				Result = EDataValidationResult::Invalid;
			}
		}
		else if (FoundEquipable->EquipmentType == EEquipmentType::Utility)
		{
			const UD1ItemFragment_Equipable_Utility* UtilityFragment = Cast<UD1ItemFragment_Equipable_Utility>(FoundEquipable);

			if (UtilityFragment->UtilityType == EUtilityType::Count)
			{
				Context.AddError(FText::FromString(FString::Printf(TEXT("Utility Type is Invalid : [EUtilityType::Count]"))));
				Result = EDataValidationResult::Invalid;
			}
		}

		if (FoundEquipable->EquipmentType == EEquipmentType::Armor || FoundEquipable->EquipmentType == EEquipmentType::Weapon)
		{
			if (MaxStackCount != 1)
			{
				Context.AddError(FText::FromString(FString::Printf(TEXT("Armor or Weapon Type must have MaxStackCount of 1: [MaxStackCount != 1]"))));
				Result = EDataValidationResult::Invalid;
			}
		}
	}
	
	return Result;
}
#endif // WITH_EDITOR

const UD1ItemFragment* UD1ItemTemplate::FindFragmentByClass(TSubclassOf<UD1ItemFragment> FragmentClass) const
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
