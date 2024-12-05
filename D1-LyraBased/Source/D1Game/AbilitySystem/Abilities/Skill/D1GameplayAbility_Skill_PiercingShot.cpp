#include "D1GameplayAbility_Skill_PiercingShot.h"

#include "D1GameplayTags.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitConfirmCancel.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "Player/LyraPlayerController.h"
#include "UI/HUD/D1SkillInputWidget.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1GameplayAbility_Skill_PiercingShot)

UD1GameplayAbility_Skill_PiercingShot::UD1GameplayAbility_Skill_PiercingShot(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    AbilityTags.AddTag(D1GameplayTags::Ability_Attack_Skill_1);
	BlockAbilitiesWithTag.AddTag(D1GameplayTags::Ability_Jump);
	ActivationOwnedTags.AddTag(D1GameplayTags::Status_ADS_Bow);
	ActivationOwnedTags.AddTag(D1GameplayTags::Status_RejectHitReact);
	ActivationOwnedTags.AddTag(D1GameplayTags::Status_Skill);

	FD1EquipmentInfo EquipmentInfo;
	EquipmentInfo.WeaponHandType = EWeaponHandType::TwoHand;
	EquipmentInfo.RequiredWeaponType = EWeaponType::Bow;
	EquipmentInfos.Add(EquipmentInfo);
}

void UD1GameplayAbility_Skill_PiercingShot::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (K2_CheckAbilityCooldown() == false || K2_CheckAbilityCost() == false)
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}

	SetCameraMode(ADSCameraModeClass);

	if (ALyraPlayerController* LyraPlayerController = GetLyraPlayerControllerFromActorInfo())
	{
		LyraPlayerController->SetIgnoreMoveInput(true);
	}

	if (UAbilityTask_PlayMontageAndWait* ADSStartMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("ADSStartMontage"), ADSStartMontage, 1.f, NAME_None, true))
	{
		ADSStartMontageTask->ReadyForActivation();
	}
	
	PiercingShotBeginEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, D1GameplayTags::GameplayEvent_Montage_Begin, nullptr, true, true);
	if (PiercingShotBeginEventTask)
	{
		PiercingShotBeginEventTask->EventReceived.AddDynamic(this, &ThisClass::OnPiercingShotBegin);
		PiercingShotBeginEventTask->ReadyForActivation();
	}

	SkillCancelTask = UAbilityTask_WaitConfirmCancel::WaitConfirmCancel(this);
	if (SkillCancelTask)
	{
		SkillCancelTask->OnCancel.AddDynamic(this, &ThisClass::OnInputCancel);
		SkillCancelTask->ReadyForActivation();
	}
}

void UD1GameplayAbility_Skill_PiercingShot::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	ResetSkill();
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	FlushPressedInput(MainHandInputAction);
	FlushPressedInput(OffHandInputAction);
}

void UD1GameplayAbility_Skill_PiercingShot::OnPiercingShotBegin(FGameplayEventData Payload)
{
	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(this);
	
	FSkillInputInitializeMessage Message;
	Message.bShouldShow = true;
	MessageSubsystem.BroadcastMessage(D1GameplayTags::Message_HUD_Spell_Input, Message);

	SkillConfirmTask = UAbilityTask_WaitConfirmCancel::WaitConfirmCancel(this);
	if (SkillConfirmTask)
	{
		SkillConfirmTask->OnConfirm.AddDynamic(this, &ThisClass::OnInputConfirm);
		SkillConfirmTask->ReadyForActivation();
	}
}

void UD1GameplayAbility_Skill_PiercingShot::ConfirmSkill()
{
	ResetSkill();

	if (UAbilityTask_PlayMontageAndWait* ReleaseMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("ReleaseMontage"), ReleaseMontage, 1.f, NAME_None, false))
	{
		ReleaseMontageTask->OnCompleted.AddDynamic(this, &ThisClass::OnMontageFinished);
		ReleaseMontageTask->OnBlendOut.AddDynamic(this, &ThisClass::OnMontageFinished);
		ReleaseMontageTask->OnInterrupted.AddDynamic(this, &ThisClass::OnMontageFinished);
		ReleaseMontageTask->OnCancelled.AddDynamic(this, &ThisClass::OnMontageFinished);
		ReleaseMontageTask->ReadyForActivation();
	}

	if (HasAuthority(&CurrentActivationInfo))
	{
		SpawnProjectile();
	}
}

void UD1GameplayAbility_Skill_PiercingShot::CancelSkill()
{
	ResetSkill();

	if (UAbilityTask_PlayMontageAndWait* ADSEndMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("ADSEndMontage"), ADSEndMontage, 1.f, NAME_None, false))
	{
		ADSEndMontageTask->OnCompleted.AddDynamic(this, &ThisClass::OnMontageFinished);
		ADSEndMontageTask->OnBlendOut.AddDynamic(this, &ThisClass::OnMontageFinished);
		ADSEndMontageTask->OnInterrupted.AddDynamic(this, &ThisClass::OnMontageFinished);
		ADSEndMontageTask->OnCancelled.AddDynamic(this, &ThisClass::OnMontageFinished);
		ADSEndMontageTask->ReadyForActivation();
	}
}

void UD1GameplayAbility_Skill_PiercingShot::ResetSkill()
{
	ClearCameraMode();

	if (UGameplayMessageSubsystem::HasInstance(this))
	{
		UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(this);

		FSkillInputInitializeMessage Message;
		Message.bShouldShow = false;
		MessageSubsystem.BroadcastMessage(D1GameplayTags::Message_HUD_Spell_Input, Message);
	}

	if (ALyraPlayerController* LyraPlayerController = GetLyraPlayerControllerFromActorInfo())
	{
		LyraPlayerController->SetIgnoreMoveInput(false);
	}

	if (PiercingShotBeginEventTask)
	{
		PiercingShotBeginEventTask->EndTask();
	}
	
	if (SkillConfirmTask)
	{
		SkillConfirmTask->EndTask();
	}

	if (SkillCancelTask)
	{
		SkillCancelTask->EndTask();
	}
}

void UD1GameplayAbility_Skill_PiercingShot::OnInputConfirm()
{
	if (SkillCancelTask)
	{
		SkillCancelTask->EndTask();
	}

	bool bCommitted = CommitAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo);
	bCommitted ? ConfirmSkill() : CancelSkill();
}

void UD1GameplayAbility_Skill_PiercingShot::OnInputCancel()
{
	CancelSkill();
}

void UD1GameplayAbility_Skill_PiercingShot::OnMontageFinished()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
