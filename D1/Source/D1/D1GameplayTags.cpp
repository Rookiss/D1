#include "D1GameplayTags.h"

namespace D1GameplayTags
{
	UE_DEFINE_GAMEPLAY_TAG(State_Dead,							"State.Dead");
	UE_DEFINE_GAMEPLAY_TAG(State_DamageImmunity,				"State.DamageImmunity");
	UE_DEFINE_GAMEPLAY_TAG(State_InputBlocked,					"State.InputBlocked");

	UE_DEFINE_GAMEPLAY_TAG(Event_Interact_Active,				"Event.Interact.Active");
	
	UE_DEFINE_GAMEPLAY_TAG(Input_Action_Move,					"Input.Action.Move");
	UE_DEFINE_GAMEPLAY_TAG(Input_Action_Look,					"Input.Action.Look");
	UE_DEFINE_GAMEPLAY_TAG(Input_Action_Crouch,					"Input.Action.Crouch");
	UE_DEFINE_GAMEPLAY_TAG(Input_Action_Jump,					"Input.Action.Jump");
	UE_DEFINE_GAMEPLAY_TAG(Input_Action_Inventory,				"Input.Action.Inventory");
	UE_DEFINE_GAMEPLAY_TAG(Input_Action_Interact,				"Input.Action.Interact");
	
	UE_DEFINE_GAMEPLAY_TAG(Ability_Death,						"Ability.Death");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Jump,						"Ability.Jump");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Interact,					"Ability.Interact");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Attack,						"Ability.Attack");
	
	UE_DEFINE_GAMEPLAY_TAG(Attribute_Primary_Health,			"Attribute.Primary.Health");
	UE_DEFINE_GAMEPLAY_TAG(Attribute_Primary_MaxHealth, 		"Attribute.Primary.MaxHealth");
	UE_DEFINE_GAMEPLAY_TAG(Attribute_Primary_Mana,				"Attribute.Primary.Mana");
	UE_DEFINE_GAMEPLAY_TAG(Attribute_Primary_MaxMana,			"Attribute.Primary.MaxMana");
	
	UE_DEFINE_GAMEPLAY_TAG(Attribute_Primary_AttackSpeed,		"Attribute.Primary.AttackSpeed");
	UE_DEFINE_GAMEPLAY_TAG(Attribute_Primary_MoveSpeed,			"Attribute.Primary.MoveSpeed");
	
	UE_DEFINE_GAMEPLAY_TAG(Attribute_Primary_BaseDamage,		"Attribute.Primary.BaseDamage");
	UE_DEFINE_GAMEPLAY_TAG(Attribute_Primary_BaseDefense,		"Attribute.Primary.BaseDefense");
	
	UE_DEFINE_GAMEPLAY_TAG(Attribute_Secondary_Strength,		"Attribute.Secondary.Strength");
	UE_DEFINE_GAMEPLAY_TAG(Attribute_Secondary_Will,			"Attribute.Secondary.Will");
	UE_DEFINE_GAMEPLAY_TAG(Attribute_Secondary_Vigor,			"Attribute.Secondary.Vigor");
	UE_DEFINE_GAMEPLAY_TAG(Attribute_Secondary_Agility,			"Attribute.Secondary.Agility");
	UE_DEFINE_GAMEPLAY_TAG(Attribute_Secondary_Knowledge,		"Attribute.Secondary.Knowledge");
	
	UE_DEFINE_GAMEPLAY_TAG(Attack_Physical,						"Attack.Physical");
	UE_DEFINE_GAMEPLAY_TAG(Attack_Magical,						"Attack.Magical");
}	
