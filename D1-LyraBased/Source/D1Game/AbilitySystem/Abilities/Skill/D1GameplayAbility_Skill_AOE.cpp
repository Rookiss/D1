#include "D1GameplayAbility_Skill_AOE.h"

#include "D1GameplayTags.h"
#include "GameplayCueFunctionLibrary.h"
#include "Abilities/GameplayAbilityTargetActor_GroundTrace.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitConfirmCancel.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Actors/D1AOEBase.h"
#include "Actors/D1WeaponBase.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "UI/HUD/D1SkillInputWidget.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1GameplayAbility_Skill_AOE)

UD1GameplayAbility_Skill_AOE::UD1GameplayAbility_Skill_AOE(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1GameplayAbility_Skill_AOE::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (K2_CheckAbilityCooldown() == false || K2_CheckAbilityCost() == false)
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}
	
	if (UAbilityTask_PlayMontageAndWait* PlayMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("CastStartMontage"), CastStartMontage, 1.f, NAME_None, true, 1.f, 0.f, false))
	{
		PlayMontageTask->ReadyForActivation();
	}

	CastStartMontageBeginEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, D1GameplayTags::GameplayEvent_Montage_Begin, nullptr, true, true);
	if (CastStartMontageBeginEventTask)
	{
		CastStartMontageBeginEventTask->EventReceived.AddDynamic(this, &ThisClass::OnCastStartMontageBegin);
		CastStartMontageBeginEventTask->ReadyForActivation();
	}

	SkillConfirmCancelTask = UAbilityTask_WaitConfirmCancel::WaitConfirmCancel(this);
	if (SkillConfirmCancelTask)
	{
		SkillConfirmCancelTask->OnConfirm.AddDynamic(this, &ThisClass::OnInputConfirm);
		SkillConfirmCancelTask->OnCancel.AddDynamic(this, &ThisClass::OnInputCancel);
		SkillConfirmCancelTask->ReadyForActivation();
	}
}

void UD1GameplayAbility_Skill_AOE::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	ResetSkill();
	
	FGameplayCueParameters Parameters;
	UGameplayCueFunctionLibrary::RemoveGameplayCueOnActor(GetAvatarActorFromActorInfo(), CastGameplayCueTag, Parameters);
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	FlushPressedInput(MainHandInputAction);
	FlushPressedInput(OffHandInputAction);
}

void UD1GameplayAbility_Skill_AOE::OnCastStartMontageBegin(FGameplayEventData Payload)
{
	FGameplayCueParameters Parameters;
	Parameters.EffectCauser = GetFirstWeaponActor();
	UGameplayCueFunctionLibrary::AddGameplayCueOnActor(GetAvatarActorFromActorInfo(), CastGameplayCueTag, Parameters);

	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(this);
	
	FSkillInputInitializeMessage Message;
	Message.bShouldShow = true;
	MessageSubsystem.BroadcastMessage(D1GameplayTags::Message_HUD_Spell_Input, Message);

	WaitTargetData();
}

void UD1GameplayAbility_Skill_AOE::OnSpellMontageBegin(FGameplayEventData Payload)
{
	if (HasAuthority(&CurrentActivationInfo) == false)
		return;
	
	if (TargetDataHandle.Data.IsValidIndex(0))
	{
		if (FGameplayAbilityTargetData* TargetData = TargetDataHandle.Data[0].Get())
		{
			if (const FHitResult* HitResultPtr = TargetData->GetHitResult())
			{
				const FHitResult& HitResult = *HitResultPtr;
				const FVector HitLocation = HitResult.Location;
				const FVector StartLocation = GetActorInfo().SkeletalMeshComponent->GetSocketLocation("head");
				
				if (FVector::DistSquared(HitLocation, StartLocation) > MaxRange * MaxRange)
					return;

				FTransform SpawnTransform = FTransform::Identity;
				SpawnTransform.SetLocation(HitResult.Location);
				
				AD1AOEBase* AOE = GetWorld()->SpawnActorDeferred<AD1AOEBase>(
					AOESpawnerClass,
					SpawnTransform,
					GetAvatarActorFromActorInfo(),
					Cast<APawn>(GetAvatarActorFromActorInfo()),
					ESpawnActorCollisionHandlingMethod::AlwaysSpawn
				);
	
				AOE->FinishSpawning(SpawnTransform);
			}
		}
	}

	TargetDataHandle.Clear();
}

void UD1GameplayAbility_Skill_AOE::OnSpellMontageEnd(FGameplayEventData Payload)
{
	FGameplayCueParameters Parameters;
	UGameplayCueFunctionLibrary::RemoveGameplayCueOnActor(GetAvatarActorFromActorInfo(), CastGameplayCueTag, Parameters);
}

void UD1GameplayAbility_Skill_AOE::OnMontageFinished()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UD1GameplayAbility_Skill_AOE::OnInputConfirm()
{
	ConfirmTaskByInstanceName(TEXT("TargetDataTask"), true);
}

void UD1GameplayAbility_Skill_AOE::OnInputCancel()
{
	CancelSkill();
}

void UD1GameplayAbility_Skill_AOE::ConfirmSkill()
{
	ResetSkill();

	if (UAbilityTask_PlayMontageAndWait* PlayMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("SpellMontage"), SpellMontage, 1.f, NAME_None, false, 1.f, 0.f, false))
	{
		PlayMontageTask->OnCompleted.AddDynamic(this, &ThisClass::OnMontageFinished);
		PlayMontageTask->OnBlendOut.AddDynamic(this, &ThisClass::OnMontageFinished);
		PlayMontageTask->OnInterrupted.AddDynamic(this, &ThisClass::OnMontageFinished);
		PlayMontageTask->OnCancelled.AddDynamic(this, &ThisClass::OnMontageFinished);
		PlayMontageTask->ReadyForActivation();
	}

	if (HasAuthority(&CurrentActivationInfo))
	{
		if (UAbilityTask_WaitGameplayEvent* SpellMontageBeginEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, D1GameplayTags::GameplayEvent_Montage_Begin, nullptr, true, true))
		{
			SpellMontageBeginEvent->EventReceived.AddDynamic(this, &ThisClass::OnSpellMontageBegin);
			SpellMontageBeginEvent->ReadyForActivation();
		}

		if (UAbilityTask_WaitGameplayEvent* SpellMontageEndEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, D1GameplayTags::GameplayEvent_Montage_End, nullptr, true, true))
		{
			SpellMontageEndEvent->EventReceived.AddDynamic(this, &ThisClass::OnSpellMontageEnd);
			SpellMontageEndEvent->ReadyForActivation();
		}
	}
}

void UD1GameplayAbility_Skill_AOE::CancelSkill()
{
	ResetSkill();
	
	FGameplayCueParameters Parameters;
	UGameplayCueFunctionLibrary::RemoveGameplayCueOnActor(GetAvatarActorFromActorInfo(), CastGameplayCueTag, Parameters);

	if (UAbilityTask_PlayMontageAndWait* PlayMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("CastEndMontage"), CastEndMontage, 1.f, NAME_None, false, 1.f, 0.f, false))
	{
		PlayMontageTask->OnCompleted.AddDynamic(this, &ThisClass::OnMontageFinished);
		PlayMontageTask->OnBlendOut.AddDynamic(this, &ThisClass::OnMontageFinished);
		PlayMontageTask->OnInterrupted.AddDynamic(this, &ThisClass::OnMontageFinished);
		PlayMontageTask->OnCancelled.AddDynamic(this, &ThisClass::OnMontageFinished);
		PlayMontageTask->ReadyForActivation();
	}
}

void UD1GameplayAbility_Skill_AOE::ResetSkill()
{
	if (UGameplayMessageSubsystem::HasInstance(this))
	{
		UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(this);

		FSkillInputInitializeMessage Message;
		Message.bShouldShow = false;
		MessageSubsystem.BroadcastMessage(D1GameplayTags::Message_HUD_Spell_Input, Message);
	}

	if (CastStartMontageBeginEventTask)
	{
		CastStartMontageBeginEventTask->EndTask();
	}

	if (SkillConfirmCancelTask)
	{
		SkillConfirmCancelTask->EndTask();
	}
}

void UD1GameplayAbility_Skill_AOE::OnValidData(const FGameplayAbilityTargetDataHandle& Data)
{
	TargetDataHandle = Data;

	bool bCommitted = CommitAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo);
	bCommitted ? ConfirmSkill() : CancelSkill();
}

void UD1GameplayAbility_Skill_AOE::OnCancelled(const FGameplayAbilityTargetDataHandle& Data)
{
	CancelSkill();
}
