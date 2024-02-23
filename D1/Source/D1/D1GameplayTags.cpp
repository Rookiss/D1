#include "D1GameplayTags.h"

namespace D1GameplayTags
{
	UE_DEFINE_GAMEPLAY_TAG(State_DamageImmunity,					"State.DamageImmunity");
	UE_DEFINE_GAMEPLAY_TAG(State_InputBlocked,						"State.InputBlocked");
	UE_DEFINE_GAMEPLAY_TAG(State_Dead,								"State.Dead");
	UE_DEFINE_GAMEPLAY_TAG(State_Jump,								"State.Jump");
	UE_DEFINE_GAMEPLAY_TAG(State_Attack,							"State.Attack");
	UE_DEFINE_GAMEPLAY_TAG(State_Interact,							"State.Interact");
	UE_DEFINE_GAMEPLAY_TAG(State_EquipWeapon,						"State.EquipWeapon");
	UE_DEFINE_GAMEPLAY_TAG(State_Weapon_Armed,						"State.Weapon.Armed");
	UE_DEFINE_GAMEPLAY_TAG(State_Weapon_Blocking,					"State.Weapon.Blocking");
	
	UE_DEFINE_GAMEPLAY_TAG(Event_Interact,							"Event.Interact");
	UE_DEFINE_GAMEPLAY_TAG(Event_Attack_Begin,						"Event.Attack.Begin");
	UE_DEFINE_GAMEPLAY_TAG(Event_Attack_Tick,						"Event.Attack.Tick");
	UE_DEFINE_GAMEPLAY_TAG(Event_Attack_End,						"Event.Attack.End");
	UE_DEFINE_GAMEPLAY_TAG(Event_EquipWeapon_ToggleArming,			"Event.EquipWeapon.ToggleArming");
	UE_DEFINE_GAMEPLAY_TAG(Event_EquipWeapon_Primary,				"Event.EquipWeapon.Primary");
	UE_DEFINE_GAMEPLAY_TAG(Event_EquipWeapon_Secondary,				"Event.EquipWeapon.Secondary");
	UE_DEFINE_GAMEPLAY_TAG(Event_EquipWeapon_CycleBackward,			"Event.EquipWeapon.CycleBackward");
	UE_DEFINE_GAMEPLAY_TAG(Event_EquipWeapon_CycleForward,			"Event.EquipWeapon.CycleForward");
	UE_DEFINE_GAMEPLAY_TAG(Event_Montage_Begin,						"Event.Montage.Begin");
	UE_DEFINE_GAMEPLAY_TAG(Event_Montage_End,						"Event.Montage.End");
		
	UE_DEFINE_GAMEPLAY_TAG(Input_Action_Move,						"Input.Action.Move");
	UE_DEFINE_GAMEPLAY_TAG(Input_Action_Look,						"Input.Action.Look");
	UE_DEFINE_GAMEPLAY_TAG(Input_Action_Crouch,						"Input.Action.Crouch");
	UE_DEFINE_GAMEPLAY_TAG(Input_Action_Jump,						"Input.Action.Jump");
	UE_DEFINE_GAMEPLAY_TAG(Input_Action_Inventory,					"Input.Action.Inventory");
	UE_DEFINE_GAMEPLAY_TAG(Input_Action_Interact,					"Input.Action.Interact");
	UE_DEFINE_GAMEPLAY_TAG(Input_Action_Attack_MainHand,			"Input.Action.Attack.MainHand");
	UE_DEFINE_GAMEPLAY_TAG(Input_Action_Attack_OffHand,				"Input.Action.Attack.OffHand");
	UE_DEFINE_GAMEPLAY_TAG(Input_Action_EquipWeapon_ToggleArming,	"Input.Action.EquipWeapon.ToggleArming");
	UE_DEFINE_GAMEPLAY_TAG(Input_Action_EquipWeapon_Primary,		"Input.Action.EquipWeapon.Primary");
	UE_DEFINE_GAMEPLAY_TAG(Input_Action_EquipWeapon_Secondary,		"Input.Action.EquipWeapon.Secondary");
	UE_DEFINE_GAMEPLAY_TAG(Input_Action_EquipWeapon_CycleBackward,	"Input.Action.EquipWeapon.CycleBackward");
	UE_DEFINE_GAMEPLAY_TAG(Input_Action_EquipWeapon_CycleForward,	"Input.Action.EquipWeapon.CycleForward");
	
	UE_DEFINE_GAMEPLAY_TAG(Ability_Death,							"Ability.Death");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Jump,							"Ability.Jump");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Interact,						"Ability.Interact");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Attack,							"Ability.Attack");
	UE_DEFINE_GAMEPLAY_TAG(Ability_EquipWeapon,						"Ability.EquipWeapon");
		
	UE_DEFINE_GAMEPLAY_TAG(Attribute_Primary_Health,				"Attribute.Primary.Health");
	UE_DEFINE_GAMEPLAY_TAG(Attribute_Primary_MaxHealth, 			"Attribute.Primary.MaxHealth");
	UE_DEFINE_GAMEPLAY_TAG(Attribute_Primary_Mana,					"Attribute.Primary.Mana");
	UE_DEFINE_GAMEPLAY_TAG(Attribute_Primary_MaxMana,				"Attribute.Primary.MaxMana");
		
	UE_DEFINE_GAMEPLAY_TAG(Attribute_Primary_AttackSpeed,			"Attribute.Primary.AttackSpeed");
	UE_DEFINE_GAMEPLAY_TAG(Attribute_Primary_MoveSpeed,				"Attribute.Primary.MoveSpeed");
		
	UE_DEFINE_GAMEPLAY_TAG(Attribute_Primary_BaseDamage,			"Attribute.Primary.BaseDamage");
	UE_DEFINE_GAMEPLAY_TAG(Attribute_Primary_BaseDefense,			"Attribute.Primary.BaseDefense");
		
	UE_DEFINE_GAMEPLAY_TAG(Attribute_Secondary_Strength,			"Attribute.Secondary.Strength");
	UE_DEFINE_GAMEPLAY_TAG(Attribute_Secondary_Will,				"Attribute.Secondary.Will");
	UE_DEFINE_GAMEPLAY_TAG(Attribute_Secondary_Vigor,				"Attribute.Secondary.Vigor");
	UE_DEFINE_GAMEPLAY_TAG(Attribute_Secondary_Agility,				"Attribute.Secondary.Agility");
	UE_DEFINE_GAMEPLAY_TAG(Attribute_Secondary_Knowledge,			"Attribute.Secondary.Knowledge");
		
	UE_DEFINE_GAMEPLAY_TAG(Attack_Physical,							"Attack.Physical");
	UE_DEFINE_GAMEPLAY_TAG(Attack_Magical,							"Attack.Magical");
}
