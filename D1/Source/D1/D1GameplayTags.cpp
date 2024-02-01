#include "D1GameplayTags.h"

#include "GameplayTagsManager.h"

namespace D1GameplayTags
{
	UE_DEFINE_GAMEPLAY_TAG(ASC_DamageImmunity,						"ASC.DamageImmunity");
	UE_DEFINE_GAMEPLAY_TAG(ASC_InputBlocked,						"ASC.InputBlocked");
	
	UE_DEFINE_GAMEPLAY_TAG(Input_Action_Move,						"Input.Action.Move");
	UE_DEFINE_GAMEPLAY_TAG(Input_Action_Look,						"Input.Action.Look");
	UE_DEFINE_GAMEPLAY_TAG(Input_Action_Crouch,						"Input.Action.Crouch");
	UE_DEFINE_GAMEPLAY_TAG(Input_Action_Jump,						"Input.Action.Jump");
	UE_DEFINE_GAMEPLAY_TAG(Input_Action_Inventory,					"Input.Action.Inventory");
	
	UE_DEFINE_GAMEPLAY_TAG(Ability_Death,							"Ability.Death");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Jump,							"Ability.Jump");

	UE_DEFINE_GAMEPLAY_TAG(Attribute_Primary_Health,				"Attribute.Primary.Health");
	UE_DEFINE_GAMEPLAY_TAG(Attribute_Primary_MaxHealth, 			"Attribute.Primary.MaxHealth");
	UE_DEFINE_GAMEPLAY_TAG(Attribute_Primary_Mana,					"Attribute.Primary.Mana");
	UE_DEFINE_GAMEPLAY_TAG(Attribute_Primary_MaxMana,				"Attribute.Primary.MaxMana");
	
	UE_DEFINE_GAMEPLAY_TAG(Attribute_Secondary_Strength,			"Attribute.Secondary.Strength");
	UE_DEFINE_GAMEPLAY_TAG(Attribute_Secondary_Vigor,				"Attribute.Secondary.Vigor");
	UE_DEFINE_GAMEPLAY_TAG(Attribute_Secondary_Agility,				"Attribute.Secondary.Agility");
	UE_DEFINE_GAMEPLAY_TAG(Attribute_Secondary_Dexterity,			"Attribute.Secondary.Dexterity");
	UE_DEFINE_GAMEPLAY_TAG(Attribute_Secondary_Will,				"Attribute.Secondary.Will");
	UE_DEFINE_GAMEPLAY_TAG(Attribute_Secondary_Knowledge,			"Attribute.Secondary.Knowledge");
	UE_DEFINE_GAMEPLAY_TAG(Attribute_Secondary_Resourcefulness, 	"Attribute.Secondary.Resourcefulness");
	
	// UE_DEFINE_GAMEPLAY_TAG(Equipment_Weapon_Primary_LeftHand,	"Equipment.Weapon.Primary.LeftHand");
	// UE_DEFINE_GAMEPLAY_TAG(Equipment_Weapon_Primary_RightHand,	"Equipment.Weapon.Primary.RightHand");
	// UE_DEFINE_GAMEPLAY_TAG(Equipment_Weapon_Primary_TwoHand,		"Equipment.Weapon.Primary.TwoHand");
	//
	// UE_DEFINE_GAMEPLAY_TAG(Equipment_Weapon_Secondary_LeftHand,  "Equipment.Weapon.Secondary.LeftHand");
	// UE_DEFINE_GAMEPLAY_TAG(Equipment_Weapon_Secondary_RightHand, "Equipment.Weapon.Secondary.RightHand");
	// UE_DEFINE_GAMEPLAY_TAG(Equipment_Weapon_Secondary_TwoHand,   "Equipment.Weapon.Secondary.TwoHand");
	//
	// UE_DEFINE_GAMEPLAY_TAG(Equipment_Armor_Head,					"Equipment.Armor.Head");
	// UE_DEFINE_GAMEPLAY_TAG(Equipment_Armor_Chest,				"Equipment.Armor.Chest");
	// UE_DEFINE_GAMEPLAY_TAG(Equipment_Armor_Legs,					"Equipment.Armor.Legs");
	// UE_DEFINE_GAMEPLAY_TAG(Equipment_Armor_Hand,					"Equipment.Armor.Hand");
	// UE_DEFINE_GAMEPLAY_TAG(Equipment_Armor_Foot,					"Equipment.Armor.Foot");
}
