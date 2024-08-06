#include "D1ItemTemplate.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif // WITH_EDITOR

#include "Fragments/D1ItemFragment_Equippable.h"
#include "Fragments/D1ItemFragment_Equippable_Armor.h"
#include "Fragments/D1ItemFragment_Equippable_Utility.h"
#include "Fragments/D1ItemFragment_Equippable_Weapon.h"

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
	
	const UD1ItemFragment_Equippable* EquippableFragment = nullptr;
	for (UD1ItemFragment* Fragment : Fragments)
	{
		if (UD1ItemFragment_Equippable* CurrentEquippable = Cast<UD1ItemFragment_Equippable>(Fragment))
		{
			if (EquippableFragment)
			{
				Context.AddError(FText::FromString(FString::Printf(TEXT("Duplicated Equippable Fragment"))));
				return EDataValidationResult::Invalid;
			}

			EquippableFragment = CurrentEquippable;
		}
	}
	
	if (EquippableFragment)
	{
		if (EquippableFragment->EquipmentType == EEquipmentType::Count)
		{
			Context.AddError(FText::FromString(FString::Printf(TEXT("Equipment Type is Invalid : [EEquipmentType::Count]"))));
			return EDataValidationResult::Invalid;
		}
		
		const UD1ItemFragment_Equippable_Armor* ArmorFragment = Cast<UD1ItemFragment_Equippable_Armor>(EquippableFragment);
		const UD1ItemFragment_Equippable_Weapon* WeaponFragment = Cast<UD1ItemFragment_Equippable_Weapon>(EquippableFragment);
		const UD1ItemFragment_Equippable_Utility* UtilityFragment = Cast<UD1ItemFragment_Equippable_Utility>(EquippableFragment);
		
		if (ArmorFragment)
		{
			if (ArmorFragment->ArmorType == EArmorType::Count)
			{
				Context.AddError(FText::FromString(FString::Printf(TEXT("Armor Type is Invalid : [EArmorType::Count]"))));
				Result = EDataValidationResult::Invalid;
			}
		}
		else if (WeaponFragment)
		{
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
		else if (UtilityFragment)
		{
			// TODO: Utility Type Check...etc
		}

		if (ArmorFragment || WeaponFragment)
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
