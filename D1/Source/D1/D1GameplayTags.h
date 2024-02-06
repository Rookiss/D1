#pragma once

#include "NativeGameplayTags.h"

namespace D1GameplayTags
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(ASC_DamageImmunity);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(ASC_InputBlocked);
	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Action_Move);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Action_Look);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Action_Crouch);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Action_Jump);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Action_Inventory);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Action_Interaction);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Death);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Jump);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Interaction);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attribute_Primary_Health);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attribute_Primary_MaxHealth);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attribute_Primary_Mana);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attribute_Primary_MaxMana);
	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attribute_Primary_AttackSpeed);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attribute_Primary_MoveSpeed);
	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attribute_Primary_BaseDamage);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attribute_Primary_BaseDefense);
	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attribute_Secondary_Strength);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attribute_Secondary_Will);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attribute_Secondary_Vigor);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attribute_Secondary_Agility);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attribute_Secondary_Knowledge);
	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attack_Physical);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attack_Magical);
}
