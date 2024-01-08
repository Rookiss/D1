#include "D1GameplayTags.h"

#include "GameplayTagsManager.h"

namespace D1GameplayTags
{
	UE_DEFINE_GAMEPLAY_TAG(ASC_DamageImmunity, "ASC.DamageImmunity");
	UE_DEFINE_GAMEPLAY_TAG(ASC_InputBlocked, "ASC.InputBlocked");
	
	UE_DEFINE_GAMEPLAY_TAG(Input_Action_Move, "Input.Action.Move");
	UE_DEFINE_GAMEPLAY_TAG(Input_Action_Look, "Input.Action.Look");
	UE_DEFINE_GAMEPLAY_TAG(Input_Action_Jump, "Input.Action.Jump");
	
	UE_DEFINE_GAMEPLAY_TAG(Ability_Death, "Ability.Death");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Jump, "Ability.Jump");

	UE_DEFINE_GAMEPLAY_TAG(Attribute_Primary_Health, "Attribute.Primary.Health");
	UE_DEFINE_GAMEPLAY_TAG(Attribute_Primary_MaxHealth, "Attribute.Primary.MaxHealth");
	UE_DEFINE_GAMEPLAY_TAG(Attribute_Primary_Mana, "Attribute.Primary.Mana");
	UE_DEFINE_GAMEPLAY_TAG(Attribute_Primary_MaxMana, "Attribute.Primary.MaxMana");
	
	UE_DEFINE_GAMEPLAY_TAG(Attribute_Secondary_Strength, "Attribute.Secondary.Strength");
	UE_DEFINE_GAMEPLAY_TAG(Attribute_Secondary_Vigor, "Attribute.Secondary.Vigor");
	UE_DEFINE_GAMEPLAY_TAG(Attribute_Secondary_Agility, "Attribute.Secondary.Agility");
	UE_DEFINE_GAMEPLAY_TAG(Attribute_Secondary_Dexterity, "Attribute.Secondary.Dexterity");
	UE_DEFINE_GAMEPLAY_TAG(Attribute_Secondary_Will, "Attribute.Secondary.Will");
	UE_DEFINE_GAMEPLAY_TAG(Attribute_Secondary_Knowledge, "Attribute.Secondary.Knowledge");
	UE_DEFINE_GAMEPLAY_TAG(Attribute_Secondary_Resourcefulness, "Attribute.Secondary.Resourcefulness");
}
