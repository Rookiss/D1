#include "D1GameplayTags.h"

namespace D1GameplayTags
{
	UE_DEFINE_GAMEPLAY_TAG(State_DamageImmunity,					"State.DamageImmunity");
	UE_DEFINE_GAMEPLAY_TAG(State_InputBlocked,						"State.InputBlocked");
	UE_DEFINE_GAMEPLAY_TAG(State_Dead,								"State.Dead");
	UE_DEFINE_GAMEPLAY_TAG(State_Jump,								"State.Jump");
	UE_DEFINE_GAMEPLAY_TAG(State_Attack,							"State.Attack");
	UE_DEFINE_GAMEPLAY_TAG(State_Block,								"State.Block");
	UE_DEFINE_GAMEPLAY_TAG(State_Sprint,							"State.Sprint");
	UE_DEFINE_GAMEPLAY_TAG(State_Interact,							"State.Interact");
	UE_DEFINE_GAMEPLAY_TAG(State_ChangeEquip,						"State.ChangeEquip");
	UE_DEFINE_GAMEPLAY_TAG(State_ChangeSkill,						"State.ChangeSkill");
	UE_DEFINE_GAMEPLAY_TAG(State_OpenUI,							"State.OpenUI");
	
	UE_DEFINE_GAMEPLAY_TAG(Event_Interact,							"Event.Interact");
	UE_DEFINE_GAMEPLAY_TAG(Event_ChangeEquip,						"Event.ChangeEquip");
	UE_DEFINE_GAMEPLAY_TAG(Event_ChangeSkill,						"Event.ChangeSkill");
	UE_DEFINE_GAMEPLAY_TAG(Event_Montage_Begin,						"Event.Montage.Begin");
	UE_DEFINE_GAMEPLAY_TAG(Event_Montage_Tick,						"Event.Montage.Tick");
	UE_DEFINE_GAMEPLAY_TAG(Event_Montage_End,						"Event.Montage.End");
	UE_DEFINE_GAMEPLAY_TAG(Event_Trace,								"Event.Trace");
	UE_DEFINE_GAMEPLAY_TAG(Event_Ability_Confirm,					"Event.Ability.Confirm");
	UE_DEFINE_GAMEPLAY_TAG(Event_Ability_Cancel,					"Event.Ability.Cancel");
	UE_DEFINE_GAMEPLAY_TAG(Event_Ability_Notify,					"Event.Ability.Notify");
		
	UE_DEFINE_GAMEPLAY_TAG(Input_Action_Move,						"Input.Action.Move");
	UE_DEFINE_GAMEPLAY_TAG(Input_Action_Look,						"Input.Action.Look");
	UE_DEFINE_GAMEPLAY_TAG(Input_Action_Crouch,						"Input.Action.Crouch");
	UE_DEFINE_GAMEPLAY_TAG(Input_Action_Jump,						"Input.Action.Jump");
	UE_DEFINE_GAMEPLAY_TAG(Input_Action_Sprint,						"Input.Action.Sprint");
	UE_DEFINE_GAMEPLAY_TAG(Input_Action_Inventory,					"Input.Action.Inventory");
	UE_DEFINE_GAMEPLAY_TAG(Input_Action_Interact,					"Input.Action.Interact");
	UE_DEFINE_GAMEPLAY_TAG(Input_Action_Attack_MainHand,			"Input.Action.Attack.MainHand");
	UE_DEFINE_GAMEPLAY_TAG(Input_Action_Attack_OffHand,				"Input.Action.Attack.OffHand");
	UE_DEFINE_GAMEPLAY_TAG(Input_Action_ChangeEquip_Primary,		"Input.Action.ChangeEquip.Primary");
	UE_DEFINE_GAMEPLAY_TAG(Input_Action_ChangeEquip_Secondary,		"Input.Action.ChangeEquip.Secondary");
	UE_DEFINE_GAMEPLAY_TAG(Input_Action_ChangeSkill,				"Input.Action.ChangeSkill");
	UE_DEFINE_GAMEPLAY_TAG(Input_Action_Ability_Confirm,			"Input.Action.Ability.Confirm");
	UE_DEFINE_GAMEPLAY_TAG(Input_Action_Ability_Cancel,				"Input.Action.Ability.Cancel");
	
	UE_DEFINE_GAMEPLAY_TAG(Ability_Death,							"Ability.Death");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Jump,							"Ability.Jump");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Sprint,							"Ability.Sprint");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Interact,						"Ability.Interact");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Attack,							"Ability.Attack");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Attack_1,						"Ability.Attack_1");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Attack_2,						"Ability.Attack_2");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Attack_3,						"Ability.Attack_3");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Blockk,							"Ability.Block");
	UE_DEFINE_GAMEPLAY_TAG(Ability_ChangeEquip,						"Ability.ChangeEquip");
	UE_DEFINE_GAMEPLAY_TAG(Ability_ChangeSkill,						"Ability.ChangeSkill");
	
	UE_DEFINE_GAMEPLAY_TAG(Cooldown_Sprint,							"Cooldown.Sprint");
	
	UE_DEFINE_GAMEPLAY_TAG(GameplayEffect_AutoRegenStamina,			"GameplayEffect.AutoRegenStamina");
		
	UE_DEFINE_GAMEPLAY_TAG(Attribute_Primary_Health,				"Attribute.Primary.Health");
	UE_DEFINE_GAMEPLAY_TAG(Attribute_Primary_MaxHealth, 			"Attribute.Primary.MaxHealth");
	UE_DEFINE_GAMEPLAY_TAG(Attribute_Primary_Mana,					"Attribute.Primary.Mana");
	UE_DEFINE_GAMEPLAY_TAG(Attribute_Primary_MaxMana,				"Attribute.Primary.MaxMana");
	UE_DEFINE_GAMEPLAY_TAG(Attribute_Primary_Stamina,				"Attribute.Primary.Stamina");
	UE_DEFINE_GAMEPLAY_TAG(Attribute_Primary_MaxStamina,			"Attribute.Primary.MaxStamina");
	UE_DEFINE_GAMEPLAY_TAG(Attribute_Primary_AttackSpeed,			"Attribute.Primary.AttackSpeed");
	UE_DEFINE_GAMEPLAY_TAG(Attribute_Primary_MoveSpeed,				"Attribute.Primary.MoveSpeed");
	UE_DEFINE_GAMEPLAY_TAG(Attribute_Primary_MoveSpeedPercent,		"Attribute.Primary.MoveSpeePercent");
	UE_DEFINE_GAMEPLAY_TAG(Attribute_Primary_PhysicalDamage,		"Attribute.Primary.PhysicalDamage");
	UE_DEFINE_GAMEPLAY_TAG(Attribute_Primary_MagicalDamage,			"Attribute.Primary.MagicalDamage");
	UE_DEFINE_GAMEPLAY_TAG(Attribute_Primary_BaseDefense,			"Attribute.Primary.BaseDefense");
		
	UE_DEFINE_GAMEPLAY_TAG(Attribute_Secondary_Strength,			"Attribute.Secondary.Strength");
	UE_DEFINE_GAMEPLAY_TAG(Attribute_Secondary_Will,				"Attribute.Secondary.Will");
	UE_DEFINE_GAMEPLAY_TAG(Attribute_Secondary_Vigor,				"Attribute.Secondary.Vigor");
	UE_DEFINE_GAMEPLAY_TAG(Attribute_Secondary_Agility,				"Attribute.Secondary.Agility");
	UE_DEFINE_GAMEPLAY_TAG(Attribute_Secondary_Knowledge,			"Attribute.Secondary.Knowledge");
		
	UE_DEFINE_GAMEPLAY_TAG(Attack_Physical,							"Attack.Physical");
	UE_DEFINE_GAMEPLAY_TAG(Attack_Magical,							"Attack.Magical");
	
	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Impact_Weapon,				"GameplayCue.Impact.Weapon");
	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Impact_Fireball,				"GameplayCue.Impact.Fireball");
	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Cast_Fireball,				"GameplayCue.Cast.Fireball");
	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Cast_Lightning,				"GameplayCue.Cast.Lightning");
	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Lightning,					"GameplayCue.Lightning");
}
