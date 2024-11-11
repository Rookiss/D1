#include "D1GameplayTags.h"

#include "Engine/EngineTypes.h"
#include "GameplayTagsManager.h"
#include "D1LogChannels.h"

namespace D1GameplayTags
{
	// InitState
	UE_DEFINE_GAMEPLAY_TAG(InitState_Spawned,						        "InitState.Spawned");
	UE_DEFINE_GAMEPLAY_TAG(InitState_DataAvailable,					        "InitState.DataAvailable");
	UE_DEFINE_GAMEPLAY_TAG(InitState_DataInitialized,				        "InitState.DataInitialized");
	UE_DEFINE_GAMEPLAY_TAG(InitState_GameplayReady,					        "InitState.GameplayReady");

	// Input
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Move,									"InputTag.Move");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Look_Mouse,								"InputTag.Look.Mouse");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Look_Stick,								"InputTag.Look.Stick");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Jump,									"InputTag.Jump");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Dash,									"InputTag.Dash");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Sprint,									"InputTag.Sprint");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Crouch,									"InputTag.Crouch");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Interact,								"InputTag.Interact");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_ChangeEquip_Primary,					"InputTag.ChangeEquip.Primary");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_ChangeEquip_Secondary,					"InputTag.ChangeEquip.Secondary");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_ChangeEquip_Tertiary,					"InputTag.ChangeEquip.Tertiary");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_ChangeEquip_Quaternary,					"InputTag.ChangeEquip.Quaternary");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_ChangeEquip_Quinary,					"InputTag.ChangeEquip.Quinary");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_ChangeEquip_Senary,						"InputTag.ChangeEquip.Senary");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Ability_Confirm,						"InputTag.Ability.Confirm");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Ability_Cancel,							"InputTag.Ability.Cancel");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Attack_MainHand,						"InputTag.Attack.MainHand");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Attack_OffHand,							"InputTag.Attack.OffHand");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Attack_Skill_1,							"InputTag.Attack.Skill.1");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Attack_Skill_2,							"InputTag.Attack.Skill.2");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Widget_Inventory,						"InputTag.Widget.Inventory");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Widget_WorldMap,						"InputTag.Widget.WorldMap");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Widget_ClassSelection,					"InputTag.Widget.ClassSelection");
	
	// Ability
	UE_DEFINE_GAMEPLAY_TAG(Ability_ActivateFail_IsDead,						"Ability.ActivateFail.IsDead");
	UE_DEFINE_GAMEPLAY_TAG(Ability_ActivateFail_Cooldown,					"Ability.ActivateFail.Cooldown");
	UE_DEFINE_GAMEPLAY_TAG(Ability_ActivateFail_Cost,						"Ability.ActivateFail.Cost");
	UE_DEFINE_GAMEPLAY_TAG(Ability_ActivateFail_TagsBlocked,				"Ability.ActivateFail.TagsBlocked");
	UE_DEFINE_GAMEPLAY_TAG(Ability_ActivateFail_TagsMissing,				"Ability.ActivateFail.TagsMissing");
	UE_DEFINE_GAMEPLAY_TAG(Ability_ActivateFail_Networking,					"Ability.ActivateFail.Networking");
	UE_DEFINE_GAMEPLAY_TAG(Ability_ActivateFail_ActivationGroup,			"Ability.ActivateFail.ActivationGroup");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Behavior_SurvivesDeath,					"Ability.Behavior.SurvivesDeath");
	
	UE_DEFINE_GAMEPLAY_TAG(Ability_Crouch,									"Ability.Crouch");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Death,									"Ability.Death");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Jump,									"Ability.Jump");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Knockback,								"Ability.Knockback");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Stun,									"Ability.Stun");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Dash_Check,								"Ability.Dash.Check");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Dash_Active,								"Ability.Dash.Active");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Sprint_Check,							"Ability.Sprint.Check");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Sprint_Active,							"Ability.Sprint.Active");
	UE_DEFINE_GAMEPLAY_TAG(Ability_ChangeEquip,								"Ability.ChangeEquip");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Attack,									"Ability.Attack");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Attack_1,								"Ability.Attack.1");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Attack_2,								"Ability.Attack.2");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Attack_3,								"Ability.Attack.3");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Attack_Bow,								"Ability.Attack.Bow");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Attack_Skill,							"Ability.Attack.Skill");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Attack_Skill_1,							"Ability.Attack.Skill.1");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Attack_Skill_2,							"Ability.Attack.Skill.2");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Throw,									"Ability.Throw");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Drink,									"Ability.Drink");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Block,									"Ability.Block");
	UE_DEFINE_GAMEPLAY_TAG(Ability_ADS,										"Ability.ADS");
	UE_DEFINE_GAMEPLAY_TAG(Ability_ADS_Bow,									"Ability.ADS.Bow");
	UE_DEFINE_GAMEPLAY_TAG(Ability_ADS_Throw,								"Ability.ADS.Throw");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Interact,								"Ability.Interact");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Interact_Active,							"Ability.Interact.Active");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Interact_Object,							"Ability.Interact.Object");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Widget_Inventory,						"Ability.Widget.Inventory");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Widget_WorldMap,							"Ability.Widget.WorldMap");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Widget_ClassSelection,					"Ability.Widget.ClassSelection");
	UE_DEFINE_GAMEPLAY_TAG(Ability_HitReact,								"Ability.HitReact");

	// Status
	UE_DEFINE_GAMEPLAY_TAG(Status_Crouch,									"Status.Crouch");
	UE_DEFINE_GAMEPLAY_TAG(Status_Jump,										"Status.Jump");
	UE_DEFINE_GAMEPLAY_TAG(Status_Knockback,								"Status.Knockback");
	UE_DEFINE_GAMEPLAY_TAG(Status_Stun,										"Status.Stun");
	UE_DEFINE_GAMEPLAY_TAG(Status_Death,									"Status.Death");
	UE_DEFINE_GAMEPLAY_TAG(Status_Death_Dying,								"Status.Death.Dying");
	UE_DEFINE_GAMEPLAY_TAG(Status_Death_Dead,								"Status.Death.Dead");
	UE_DEFINE_GAMEPLAY_TAG(Status_Attack,									"Status.Attack");
	UE_DEFINE_GAMEPLAY_TAG(Status_Skill,									"Status.Skill");
	UE_DEFINE_GAMEPLAY_TAG(Status_Throw,									"Status.Throw");
	UE_DEFINE_GAMEPLAY_TAG(Status_Drink,									"Status.Drink");
	UE_DEFINE_GAMEPLAY_TAG(Status_Block,									"Status.Block");
	UE_DEFINE_GAMEPLAY_TAG(Status_ChangeEquip,								"Status.ChangeEquip");
	UE_DEFINE_GAMEPLAY_TAG(Status_ADS,										"Status.ADS");
	UE_DEFINE_GAMEPLAY_TAG(Status_ADS_Bow,									"Status.ADS.Bow");
	UE_DEFINE_GAMEPLAY_TAG(Status_ADS_Ready,								"Status.ADS.Ready");
	UE_DEFINE_GAMEPLAY_TAG(Status_ADS_Throw,								"Status.ADS.Throw");
	UE_DEFINE_GAMEPLAY_TAG(Status_Dash,										"Status.Dash");
	UE_DEFINE_GAMEPLAY_TAG(Status_Sprint,									"Status.Sprint");
	UE_DEFINE_GAMEPLAY_TAG(Status_Succeeded,								"Status.Succeeded");
	UE_DEFINE_GAMEPLAY_TAG(Status_Failed,									"Status.Failed");
	UE_DEFINE_GAMEPLAY_TAG(Status_Interact,									"Status.Interact");
	UE_DEFINE_GAMEPLAY_TAG(Status_HitReact,									"Status.HitReact");
	UE_DEFINE_GAMEPLAY_TAG(Status_RejectHitReact,							"Status.RejectHitReact");
	UE_DEFINE_GAMEPLAY_TAG(Status_StaminaRecoveryBlock,						"Status.StaminaRecoveryBlock");
	UE_DEFINE_GAMEPLAY_TAG(Status_PiercingShot,								"Status.PiercingShot");
	
	// Gameplay Event
	UE_DEFINE_GAMEPLAY_TAG(GameplayEvent_Death,								"GameplayEvent.Death");
	UE_DEFINE_GAMEPLAY_TAG(GameplayEvent_Montage_Begin,						"GameplayEvent.Montage.Begin");
	UE_DEFINE_GAMEPLAY_TAG(GameplayEvent_Montage_Tick,						"GameplayEvent.Montage.Tick");
	UE_DEFINE_GAMEPLAY_TAG(GameplayEvent_Montage_End,						"GameplayEvent.Montage.End");
	UE_DEFINE_GAMEPLAY_TAG(GameplayEvent_Montage_Spell,						"GameplayEvent.Montage.Spell");
	UE_DEFINE_GAMEPLAY_TAG(GameplayEvent_Montage_Play,						"GameplayEvent.Montage.Play");
	UE_DEFINE_GAMEPLAY_TAG(GameplayEvent_Trace,								"GameplayEvent.Trace");
	UE_DEFINE_GAMEPLAY_TAG(GameplayEvent_Reset,								"GameplayEvent.Reset");
	UE_DEFINE_GAMEPLAY_TAG(GameplayEvent_CameraShake,						"GameplayEvent.CameraShake");
	UE_DEFINE_GAMEPLAY_TAG(GameplayEvent_ChangeEquip,						"GameplayEvent.ChangeEquip");
	UE_DEFINE_GAMEPLAY_TAG(GameplayEvent_ArrowVisibility,					"GameplayEvent.ArrowVisibility");
	UE_DEFINE_GAMEPLAY_TAG(GameplayEvent_Potion_Drink,						"GameplayEvent.Potion.Drink");
	UE_DEFINE_GAMEPLAY_TAG(GameplayEvent_Potion_Fill,						"GameplayEvent.Potion.Fill");
	UE_DEFINE_GAMEPLAY_TAG(GameplayEvent_Knockback,							"GameplayEvent.Knockback");
	UE_DEFINE_GAMEPLAY_TAG(GameplayEvent_Stun,								"GameplayEvent.Stun");
	UE_DEFINE_GAMEPLAY_TAG(GameplayEvent_Bow_ADS,							"GameplayEvent.Bow.ADS");
	UE_DEFINE_GAMEPLAY_TAG(GameplayEvent_HitReact,							"GameplayEvent.HitReact");
	
	// Cheat
	UE_DEFINE_GAMEPLAY_TAG(Cheat_GodMode,									"Cheat.GodMode");
	UE_DEFINE_GAMEPLAY_TAG(Cheat_UnlimitedHealth,							"Cheat.UnlimitedHealth");
	
	// Movement
	UE_DEFINE_GAMEPLAY_TAG(Movement_Mode_Walking,							"Movement.Mode.Walking");
	UE_DEFINE_GAMEPLAY_TAG(Movement_Mode_NavWalking,						"Movement.Mode.NavWalking");
	UE_DEFINE_GAMEPLAY_TAG(Movement_Mode_Falling,							"Movement.Mode.Falling");
	UE_DEFINE_GAMEPLAY_TAG(Movement_Mode_Swimming,							"Movement.Mode.Swimming");
	UE_DEFINE_GAMEPLAY_TAG(Movement_Mode_Flying,							"Movement.Mode.Flying");
	UE_DEFINE_GAMEPLAY_TAG(Movement_Mode_Custom,							"Movement.Mode.Custom");

	// Message
	UE_DEFINE_GAMEPLAY_TAG(Message_Notification,							"Message.Notification");
	UE_DEFINE_GAMEPLAY_TAG(Message_Interaction_Notice,						"Message.Interaction.Notice");
	UE_DEFINE_GAMEPLAY_TAG(Message_Interaction_Progress,					"Message.Interaction.Progress");
	UE_DEFINE_GAMEPLAY_TAG(Message_Initialize_MyInventory,					"Message.Initialize.MyInventory");
	UE_DEFINE_GAMEPLAY_TAG(Message_Initialize_OtherInventory,				"Message.Initialize.OtherInventory");
	UE_DEFINE_GAMEPLAY_TAG(Message_Initialize_MyEquipment,					"Message.Initialize.MyEquipment");
	UE_DEFINE_GAMEPLAY_TAG(Message_Initialize_OtherEquipment,				"Message.Initialize.OtherEquipment");
	UE_DEFINE_GAMEPLAY_TAG(Message_HUD_Spell_Input,							"Message.HUD.Spell.Input");
	UE_DEFINE_GAMEPLAY_TAG(Message_HUD_Spell_Progress_Construct,			"Message.HUD.Spell.Progress.Construct");
	UE_DEFINE_GAMEPLAY_TAG(Message_HUD_Spell_Progress_Refresh,				"Message.HUD.Spell.Progress.Refresh");

	// SetByCaller
	UE_DEFINE_GAMEPLAY_TAG(SetByCaller_BaseDamage,							"SetByCaller.BaseDamage");
	UE_DEFINE_GAMEPLAY_TAG(SetByCaller_BaseHealth,							"SetByCaller.BaseHealth");
	UE_DEFINE_GAMEPLAY_TAG(SetByCaller_BaseMana,							"SetByCaller.BaseMana");
	UE_DEFINE_GAMEPLAY_TAG(SetByCaller_BaseStamina,							"SetByCaller.BaseStamina");
	UE_DEFINE_GAMEPLAY_TAG(SetByCaller_IncomingDamage,						"SetByCaller.IncomingDamage");
	UE_DEFINE_GAMEPLAY_TAG(SetByCaller_IncomingHealth,						"SetByCaller.IncomingHealth");
	UE_DEFINE_GAMEPLAY_TAG(SetByCaller_IncomingMana,						"SetByCaller.IncomingMana");
	UE_DEFINE_GAMEPLAY_TAG(SetByCaller_IncomingStamina,						"SetByCaller.IncomingStamina");
	UE_DEFINE_GAMEPLAY_TAG(SetByCaller_MoveSpeed,							"SetByCaller.MoveSpeed");
	UE_DEFINE_GAMEPLAY_TAG(SetByCaller_MoveSpeedPercent,					"SetByCaller.MoveSpeedPercent");
	UE_DEFINE_GAMEPLAY_TAG(SetByCaller_Strength,						    "SetByCaller.Strength");
	UE_DEFINE_GAMEPLAY_TAG(SetByCaller_Defense,								"SetByCaller.Defense");
	UE_DEFINE_GAMEPLAY_TAG(SetByCaller_Vigor,								"SetByCaller.Vigor");
	UE_DEFINE_GAMEPLAY_TAG(SetByCaller_Agility,								"SetByCaller.Agility");
	UE_DEFINE_GAMEPLAY_TAG(SetByCaller_Resourcefulness,						"SetByCaller.Resourcefulness");
	
	// GameplayCue
	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Weapon_Attack,						"GameplayCue.Weapon.Attack");
	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Weapon_Skill,						"GameplayCue.Weapon.Skill");
	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Weapon_Impact,						"GameplayCue.Weapon.Impact");
	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Weapon_Cast_Fireball,				"GameplayCue.Weapon.Cast.Fireball");
	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Weapon_Cast_Skill,					"GameplayCue.Weapon.Cast.Skill");
	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Weapon_Skill_Targeting_Burst,		"GameplayCue.Weapon.Skill.Targeting.Burst");
	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Weapon_Skill_Targeting_Loop,			"GameplayCue.Weapon.Skill.Targeting.Loop");
	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Weapon_Overlay,						"GameplayCue.Weapon.Overlay");
	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Statue_Loop,							"GameplayCue.Statue.Loop");
	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Statue_Success,						"GameplayCue.Statue.Success");
	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Statue_Fail,							"GameplayCue.Statue.Fail");
	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Skill_Buff,							"GameplayCue.Skill.Buff");

	// Cooldown
	UE_DEFINE_GAMEPLAY_TAG(Cooldown_Bow_Fire,								"Cooldown.Bow.Fire");
	UE_DEFINE_GAMEPLAY_TAG(Cooldown_Jump,									"Cooldown.Jump");
	UE_DEFINE_GAMEPLAY_TAG(Cooldown_Dash,									"Cooldown.Dash");
	UE_DEFINE_GAMEPLAY_TAG(Cooldown_Statue,									"Cooldown.Statue");
	UE_DEFINE_GAMEPLAY_TAG(Cooldown_Sprint,									"Cooldown.Sprint");
	UE_DEFINE_GAMEPLAY_TAG(Cooldown_Skill_1,								"Cooldown.Skill.1");
	UE_DEFINE_GAMEPLAY_TAG(Cooldown_Skill_2,								"Cooldown.Skill.2");

	// GamePhase
	UE_DEFINE_GAMEPLAY_TAG(GamePhase_PreGame,								"GamePhase.PreGame");
	UE_DEFINE_GAMEPLAY_TAG(GamePhase_Game,									"GamePhase.Game");
	UE_DEFINE_GAMEPLAY_TAG(GamePhase_PostGame,								"GamePhase.PostGame");

	// HUD
	UE_DEFINE_GAMEPLAY_TAG(HUD_Slot_Reticle,								"HUD.Slot.Reticle");
	
	const TMap<uint8, FGameplayTag> MovementModeTagMap =
	{
		{ MOVE_Walking, Movement_Mode_Walking },
		{ MOVE_NavWalking, Movement_Mode_NavWalking },
		{ MOVE_Falling, Movement_Mode_Falling },
		{ MOVE_Swimming, Movement_Mode_Swimming },
		{ MOVE_Flying, Movement_Mode_Flying },
		{ MOVE_Custom, Movement_Mode_Custom }
	};

	const TMap<uint8, FGameplayTag> CustomMovementModeTagMap = {};

	FGameplayTag FindTagByString(const FString& TagString, bool bMatchPartialString)
	{
		const UGameplayTagsManager& Manager = UGameplayTagsManager::Get();
		FGameplayTag Tag = Manager.RequestGameplayTag(FName(*TagString), false);

		if (!Tag.IsValid() && bMatchPartialString)
		{
			FGameplayTagContainer AllTags;
			Manager.RequestAllGameplayTags(AllTags, true);

			for (const FGameplayTag& TestTag : AllTags)
			{
				if (TestTag.ToString().Contains(TagString))
				{
					UE_LOG(LogD1, Display, TEXT("Could not find exact match for tag [%s] but found partial match on tag [%s]."), *TagString, *TestTag.ToString());
					Tag = TestTag;
					break;
				}
			}
		}
		return Tag;
	}
}
