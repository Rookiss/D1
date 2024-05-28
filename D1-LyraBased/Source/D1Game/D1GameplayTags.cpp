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
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Widget_OpenGameStatus,					"InputTag.Widget.OpenGameStatus");
	
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
	
	// SetByCaller
	UE_DEFINE_GAMEPLAY_TAG(SetByCaller_Damage,								"SetByCaller.Damage");
	UE_DEFINE_GAMEPLAY_TAG(SetByCaller_Heal,								"SetByCaller.Heal");

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
	UE_DEFINE_GAMEPLAY_TAG(Message_Item_WeaponEquipStateChanged,	        "Message.Item.WeaponEquipStateChanged");
	UE_DEFINE_GAMEPLAY_TAG(Message_Item_EquipmentEntryChanged,		        "Message.Item.EquipmentEntryChanged");
	UE_DEFINE_GAMEPLAY_TAG(Message_Item_InventoryEntryChanged,		        "Message.Item.InventoryEntryChanged");

	UE_DEFINE_GAMEPLAY_TAG(Message_Notification,							"Message.Notification");
	UE_DEFINE_GAMEPLAY_TAG(Message_Response_ApplyBattle,					"Message.Response.ApplyBattle");
	UE_DEFINE_GAMEPLAY_TAG(Message_Response_CancelBattle,					"Message.Response.CancelBattle");
	UE_DEFINE_GAMEPLAY_TAG(Message_Response_ApplyBetting,					"Message.Response.ApplyBetting");
	UE_DEFINE_GAMEPLAY_TAG(Message_Response_CancelBetting,					"Message.Response.CancelBetting");
	UE_DEFINE_GAMEPLAY_TAG(Message_NextBattlePlayersChanged,				"Message.NextBattlePlayersChanged");
	UE_DEFINE_GAMEPLAY_TAG(Message_BettingCoinsChanged,						"Message.BettingCoinsChanged");
	UE_DEFINE_GAMEPLAY_TAG(Message_TeamChanged,								"Message.TeamChanged");
	UE_DEFINE_GAMEPLAY_TAG(Message_CoinChanged,								"Message.CoinChanged");
	UE_DEFINE_GAMEPLAY_TAG(Message_TimerMessage_Increase,					"Message.TimerMessage.Increase");
	UE_DEFINE_GAMEPLAY_TAG(Message_TimerMessage_Decrease,					"Message.TimerMessage.Decrease");

	// Attribute
	UE_DEFINE_GAMEPLAY_TAG(Attribute_Primary_Health,				        "Attribute.Primary.Health");
	UE_DEFINE_GAMEPLAY_TAG(Attribute_Primary_MaxHealth, 			        "Attribute.Primary.MaxHealth");
	UE_DEFINE_GAMEPLAY_TAG(Attribute_Primary_Stamina,						"Attribute.Primary.Stamina");
	UE_DEFINE_GAMEPLAY_TAG(Attribute_Primary_MaxStamina,				    "Attribute.Primary.MaxStamina");
	UE_DEFINE_GAMEPLAY_TAG(Attribute_Primary_MoveSpeed,				        "Attribute.Primary.MoveSpeed");
	UE_DEFINE_GAMEPLAY_TAG(Attribute_Primary_MoveSpeedPercent,		        "Attribute.Primary.MoveSpeePercent");
	UE_DEFINE_GAMEPLAY_TAG(Attribute_Primary_BaseDamage,					"Attribute.Primary.BaseDamage");
	UE_DEFINE_GAMEPLAY_TAG(Attribute_Primary_BaseDefense,			        "Attribute.Primary.BaseDefense");

	// GameplayCue
	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Impact_Weapon,						"GameplayCue.Impact.Weapon");

	// Cooldown
	UE_DEFINE_GAMEPLAY_TAG(Cooldown_Bow_Fire,								"Cooldown.Bow.Fire");
	UE_DEFINE_GAMEPLAY_TAG(Cooldown_Jump,									"Cooldown.Jump");

	// GamePhase
	UE_DEFINE_GAMEPLAY_TAG(GamePhase_WaitForPlayers,					    "GamePhase.WaitForPlayers");
	UE_DEFINE_GAMEPLAY_TAG(GamePhase_Betting,							    "GamePhase.Betting");
	UE_DEFINE_GAMEPLAY_TAG(GamePhase_Battle,							    "GamePhase.Battle");

	// HUD
	UE_DEFINE_GAMEPLAY_TAG(HUD_Slot_Timer,									"HUD.Slot.Timer");
	
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
