#include "D1ItemTemplate.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif // WITH_EDITOR

#include "Fragments/D1ItemFragment_Consumable.h"
#include "Fragments/D1ItemFragment_Equippable.h"
#include "Fragments/D1ItemFragment_Equippable_Armor.h"
#include "Fragments/D1ItemFragment_Equippable_Weapon.h"
#include "Fragments/D1ItemFragment_Stackable.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1ItemTemplate)

UD1ItemTemplate::UD1ItemTemplate(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

#if WITH_EDITOR
EDataValidationResult UD1ItemTemplate::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = UObject::IsDataValid(Context);

	// Fragment Check
	const UD1ItemFragment_Consumable* Consumable = FindFragmentByClass<UD1ItemFragment_Consumable>();
	const UD1ItemFragment_Stackable* Stackable = FindFragmentByClass<UD1ItemFragment_Stackable>();
	const UD1ItemFragment_Equippable* Equippable = FindFragmentByClass<UD1ItemFragment_Equippable>();
	const UD1ItemFragment_Equippable_Armor* Armor = FindFragmentByClass<UD1ItemFragment_Equippable_Armor>();
	const UD1ItemFragment_Equippable_Weapon* Weapon = FindFragmentByClass<UD1ItemFragment_Equippable_Weapon>();
		
	if (Equippable)
	{
		if (Armor && Weapon)
		{
			Context.AddError(FText::FromString(FString::Printf(TEXT("Conflict Fragments : [Armor] <-> [Weapon]"))));
			Result = EDataValidationResult::Invalid;
		}

		if (Stackable)
		{
			Context.AddError(FText::FromString(FString::Printf(TEXT("Conflict Fragments : [Stackable] <-> [Equippable]"))));
			Result = EDataValidationResult::Invalid;
		}

		if (Consumable)
		{
			Context.AddError(FText::FromString(FString::Printf(TEXT("Conflict Fragments : [Consumable] <-> [Equippable]"))));
			Result = EDataValidationResult::Invalid;
		}

		if (Armor && Armor->ArmorType == EArmorType::Count)
		{
			Context.AddError(FText::FromString(FString::Printf(TEXT("Armor Type is Invalid : [EArmorType::Count]"))));
			Result = EDataValidationResult::Invalid;
		}

		if (Weapon)
		{
			if (Weapon->WeaponType == EWeaponType::Count)
			{
				Context.AddError(FText::FromString(FString::Printf(TEXT("Weapon Type is Invalid : [EWeaponType::Count]"))));
				Result = EDataValidationResult::Invalid;
			}
				
			if (Weapon->WeaponHandType == EWeaponHandType::Count)
			{
				Context.AddError(FText::FromString(FString::Printf(TEXT("Weapon Hand Type is Invalid : [EWeaponHandType::Count]"))));
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
