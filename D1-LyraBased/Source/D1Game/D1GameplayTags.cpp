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
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Crouch,									"InputTag.Crouch");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Attack_MainHand,						"InputTag.Attack.MainHand");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Attack_OffHand,							"InputTag.Attack.OffHand");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_ChangeEquip_Primary,					"InputTag.ChangeEquip.Primary");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_ChangeEquip_Secondary,					"InputTag.ChangeEquip.Secondary");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_ChangeEquip_Tertiary,					"InputTag.ChangeEquip.Tertiary");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_ChangeEquip_Quaternary,					"InputTag.ChangeEquip.Quaternary");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Widget_OpenInventory,					"InputTag.Widget.OpenInventory");
	
	// Ability
	UE_DEFINE_GAMEPLAY_TAG(Ability_ActivateFail_IsDead,						"Ability.ActivateFail.IsDead");
	UE_DEFINE_GAMEPLAY_TAG(Ability_ActivateFail_Cooldown,					"Ability.ActivateFail.Cooldown");
	UE_DEFINE_GAMEPLAY_TAG(Ability_ActivateFail_Cost,						"Ability.ActivateFail.Cost");
	UE_DEFINE_GAMEPLAY_TAG(Ability_ActivateFail_TagsBlocked,				"Ability.ActivateFail.TagsBlocked");
	UE_DEFINE_GAMEPLAY_TAG(Ability_ActivateFail_TagsMissing,				"Ability.ActivateFail.TagsMissing");
	UE_DEFINE_GAMEPLAY_TAG(Ability_ActivateFail_Networking,					"Ability.ActivateFail.Networking");
	UE_DEFINE_GAMEPLAY_TAG(Ability_ActivateFail_ActivationGroup,			"Ability.ActivateFail.ActivationGroup");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Behavior_SurvivesDeath,					"Ability.Behavior.SurvivesDeath");
	
	UE_DEFINE_GAMEPLAY_TAG(Ability_Attack,									"Ability.Attack");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Attack_1,								"Ability.Attack.1");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Attack_2,								"Ability.Attack.2");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Attack_3,								"Ability.Attack.3");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Attack_Bow,								"Ability.Attack.Bow");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Block,									"Ability.Block");
	UE_DEFINE_GAMEPLAY_TAG(Ability_ChangeEquip,								"Ability.ChangeEquip");
	UE_DEFINE_GAMEPLAY_TAG(Ability_ADS,										"Ability.ADS");

	// Status
	UE_DEFINE_GAMEPLAY_TAG(Status_Crouching,								"Status.Crouching");
	UE_DEFINE_GAMEPLAY_TAG(Status_AutoRunning,								"Status.AutoRunning");
	UE_DEFINE_GAMEPLAY_TAG(Status_Death,									"Status.Death");
	UE_DEFINE_GAMEPLAY_TAG(Status_Death_Dying,								"Status.Death.Dying");
	UE_DEFINE_GAMEPLAY_TAG(Status_Death_Dead,								"Status.Death.Dead");
	UE_DEFINE_GAMEPLAY_TAG(Status_Attack,									"Status.Attack");
	UE_DEFINE_GAMEPLAY_TAG(Status_Block,									"Status.Block");
	UE_DEFINE_GAMEPLAY_TAG(Status_ChangeEquip,								"Status.ChangeEquip");
	UE_DEFINE_GAMEPLAY_TAG(Status_ADS,										"Status.ADS");
	UE_DEFINE_GAMEPLAY_TAG(Status_Succeeded,								"Status.Succeeded");
	UE_DEFINE_GAMEPLAY_TAG(Status_Failed,									"Status.Failed");
	
	// Gameplay Event
	UE_DEFINE_GAMEPLAY_TAG(GameplayEvent_Death,								"GameplayEvent.Death");
	UE_DEFINE_GAMEPLAY_TAG(GameplayEvent_Reset,								"GameplayEvent.Reset");
	UE_DEFINE_GAMEPLAY_TAG(GameplayEvent_RequestReset,						"GameplayEvent.RequestReset");
	UE_DEFINE_GAMEPLAY_TAG(GameplayEvent_Montage_Begin,						"GameplayEvent.Montage.Begin");
	UE_DEFINE_GAMEPLAY_TAG(GameplayEvent_Montage_Tick,						"GameplayEvent.Montage.Tick");
	UE_DEFINE_GAMEPLAY_TAG(GameplayEvent_Montage_End,						"GameplayEvent.Montage.End");
	UE_DEFINE_GAMEPLAY_TAG(GameplayEvent_Trace,								"GameplayEvent.Trace");
	UE_DEFINE_GAMEPLAY_TAG(GameplayEvent_ChangeEquip,						"GameplayEvent.ChangeEquip");
	UE_DEFINE_GAMEPLAY_TAG(GameplayEvent_ArrowVisibility,					"GameplayEvent.ArrowVisibility");
	
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

	// SetByCaller
	UE_DEFINE_GAMEPLAY_TAG(SetByCaller_IncomingHeal,						"SetByCaller.IncomingHeal");
	UE_DEFINE_GAMEPLAY_TAG(SetByCaller_IncomingDamage,						"SetByCaller.IncomingDamage");
	UE_DEFINE_GAMEPLAY_TAG(SetByCaller_PhysicalWeaponDamage,				"SetByCaller.PhysicalWeaponDamage");
	UE_DEFINE_GAMEPLAY_TAG(SetByCaller_MoveSpeed,						    "SetByCaller.MoveSpeed");
	UE_DEFINE_GAMEPLAY_TAG(SetByCaller_Strength,						    "SetByCaller.Strength");
	UE_DEFINE_GAMEPLAY_TAG(SetByCaller_Vigor,								"SetByCaller.Vigor");
	UE_DEFINE_GAMEPLAY_TAG(SetByCaller_Agility,								"SetByCaller.Agility");
	UE_DEFINE_GAMEPLAY_TAG(SetByCaller_Dexterity,							"SetByCaller.Dexterity");
	UE_DEFINE_GAMEPLAY_TAG(SetByCaller_Will,								"SetByCaller.Will");
	UE_DEFINE_GAMEPLAY_TAG(SetByCaller_Knowledge,							"SetByCaller.Knowledge");
	UE_DEFINE_GAMEPLAY_TAG(SetByCaller_Resourcefulness,						"SetByCaller.Resourcefulness");
	
	// GameplayCue
	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Impact_Weapon,						"GameplayCue.Impact.Weapon");
	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_HitReact,							"GameplayCue.HitReact");

	// Cooldown
	UE_DEFINE_GAMEPLAY_TAG(Cooldown_Bow_Fire,								"Cooldown.Bow.Fire");
	UE_DEFINE_GAMEPLAY_TAG(Cooldown_Jump,									"Cooldown.Jump");

	// GamePhase
	UE_DEFINE_GAMEPLAY_TAG(GamePhase_WaitForPlayers,					    "GamePhase.WaitForPlayers");

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
