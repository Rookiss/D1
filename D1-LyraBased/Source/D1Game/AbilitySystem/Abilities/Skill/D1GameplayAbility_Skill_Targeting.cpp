#include "D1GameplayAbility_Skill_Targeting.h"

#include "AbilitySystemComponent.h"
#include "D1GameplayTags.h"
#include "GameplayCueFunctionLibrary.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitConfirmCancel.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AbilitySystem/D1GameplayAbilityTargetActor_LineTraceHighlight.h"
#include "Actors/D1EquipmentBase.h"
#include "Character/LyraCharacter.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "UI/HUD/D1SkillInputWidget.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1GameplayAbility_Skill_Targeting)

UD1GameplayAbility_Skill_Targeting::UD1GameplayAbility_Skill_Targeting(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    AbilityTags.AddTag(D1GameplayTags::Ability_Attack_Skill_2);
	ActivationOwnedTags.AddTag(D1GameplayTags::Status_Skill);

	FD1EquipmentInfo EquipmentInfo;
	EquipmentInfo.WeaponHandType = EWeaponHandType::TwoHand;
	EquipmentInfo.RequiredWeaponType = EWeaponType::Staff;
	EquipmentInfos.Add(EquipmentInfo);
}

void UD1GameplayAbility_Skill_Targeting::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (K2_CheckAbilityCooldown() == false || K2_CheckAbilityCost() == false)
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}
	
	if (UAbilityTask_PlayMontageAndWait* CastStartMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("CastStartMontage"), CastStartMontage, 1.f, NAME_None, true))
	{
		CastStartMontageTask->ReadyForActivation();
	}

	CastStartBeginEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, D1GameplayTags::GameplayEvent_Montage_Begin, nullptr, true, true);
	if (CastStartBeginEventTask)
	{
		CastStartBeginEventTask->EventReceived.AddDynamic(this, &ThisClass::OnCastStartBegin);
		CastStartBeginEventTask->ReadyForActivation();
	}

	SkillConfirmCancelTask = UAbilityTask_WaitConfirmCancel::WaitConfirmCancel(this);
	if (SkillConfirmCancelTask)
	{
		SkillConfirmCancelTask->OnConfirm.AddDynamic(this, &ThisClass::OnInputConfirm);
		SkillConfirmCancelTask->OnCancel.AddDynamic(this, &ThisClass::OnInputCancel);
		SkillConfirmCancelTask->ReadyForActivation();
	}
}

void UD1GameplayAbility_Skill_Targeting::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	ResetSkill();
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	
	FlushPressedInput(MainHandInputAction);
	FlushPressedInput(OffHandInputAction);
}

void UD1GameplayAbility_Skill_Targeting::ConfirmSkill()
{
	ResetSkill();

	if (UAbilityTask_PlayMontageAndWait* SpellMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("SpellMontage"), SpellMontage, 1.f, NAME_None, false))
	{
		SpellMontageTask->OnCompleted.AddDynamic(this, &ThisClass::OnMontageFinished);
		SpellMontageTask->OnBlendOut.AddDynamic(this, &ThisClass::OnMontageFinished);
		SpellMontageTask->OnInterrupted.AddDynamic(this, &ThisClass::OnMontageFinished);
		SpellMontageTask->OnCancelled.AddDynamic(this, &ThisClass::OnMontageFinished);
		SpellMontageTask->ReadyForActivation();
	}

	if (HasAuthority(&CurrentActivationInfo))
	{
		if (UAbilityTask_WaitGameplayEvent* SpellBeginEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, D1GameplayTags::GameplayEvent_Montage_Begin, nullptr, true, true))
		{
			SpellBeginEventTask->EventReceived.AddDynamic(this, &ThisClass::OnSpellBegin);
			SpellBeginEventTask->ReadyForActivation();
		}
	}
}

void UD1GameplayAbility_Skill_Targeting::CancelSkill()
{
	ResetSkill();

	if (UAbilityTask_PlayMontageAndWait* CastEndMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("CastEndMontage"), CastEndMontage, 1.f, NAME_None, false))
	{
		CastEndMontageTask->OnCompleted.AddDynamic(this, &ThisClass::OnMontageFinished);
		CastEndMontageTask->OnBlendOut.AddDynamic(this, &ThisClass::OnMontageFinished);
		CastEndMontageTask->OnInterrupted.AddDynamic(this, &ThisClass::OnMontageFinished);
		CastEndMontageTask->OnCancelled.AddDynamic(this, &ThisClass::OnMontageFinished);
		CastEndMontageTask->ReadyForActivation();
	}
}

void UD1GameplayAbility_Skill_Targeting::ResetSkill()
{
	FGameplayCueParameters Parameters;
	UGameplayCueFunctionLibrary::RemoveGameplayCueOnActor(GetAvatarActorFromActorInfo(), CastGameplayCueTag, Parameters);
	
	if (UGameplayMessageSubsystem::HasInstance(this))
	{
		UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(this);

		FSkillInputInitializeMessage Message;
		Message.bShouldShow = false;
		MessageSubsystem.BroadcastMessage(D1GameplayTags::Message_HUD_Spell_Input, Message);
	}

	if (CastStartBeginEventTask)
	{
		CastStartBeginEventTask->EndTask();
	}

	if (SkillConfirmCancelTask)
	{
		SkillConfirmCancelTask->EndTask();
	}
}

void UD1GameplayAbility_Skill_Targeting::OnCastStartBegin(FGameplayEventData Payload)
{
	FGameplayCueParameters Parameters;
	Parameters.EffectCauser = GetFirstEquipmentActor();
	UGameplayCueFunctionLibrary::AddGameplayCueOnActor(GetAvatarActorFromActorInfo(), CastGameplayCueTag, Parameters);

	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(this);
	
	FSkillInputInitializeMessage Message;
	Message.bShouldShow = true;
	MessageSubsystem.BroadcastMessage(D1GameplayTags::Message_HUD_Spell_Input, Message);
	
	WaitTargetData();
}

void UD1GameplayAbility_Skill_Targeting::OnSpellBegin(FGameplayEventData Payload)
{
	if (HasAuthority(&CurrentActivationInfo) == false)
		return;
	
	UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo();
	if (SourceASC == nullptr)
		return;

	AD1EquipmentBase* EquipmentActor = GetFirstEquipmentActor();
	check(EquipmentActor);
	
	if (TargetDataHandle.Data.IsValidIndex(0))
	{
		if (FGameplayAbilityTargetData* TargetData = TargetDataHandle.Data[0].Get())
		{
			if (const FHitResult* HitResultPtr = TargetData->GetHitResult())
			{
				const FHitResult& HitResult = *HitResultPtr;

				for (TSubclassOf<UGameplayEffect> GameplayEffectClass : GameplayEffectClasses)
				{
					if (GameplayEffectClass)
					{
						FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(GameplayEffectClass);
						FGameplayEffectContextHandle EffectContextHandle = SourceASC->MakeEffectContext();
						EffectContextHandle.AddHitResult(HitResult);
						EffectContextHandle.AddInstigator(SourceASC->AbilityActorInfo->OwnerActor.Get(), EquipmentActor);
						EffectSpecHandle.Data->SetContext(EffectContextHandle);
						ApplyGameplayEffectSpecToTarget(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EffectSpecHandle, TargetDataHandle);
					}
				}

				UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(HitResult.GetActor());
				if (TargetASC && BurstGameplayCueTag.IsValid())
				{
					FGameplayCueParameters GameplayCueParams;

					ALyraCharacter* LyraCharacter = Cast<ALyraCharacter>(HitResult.GetActor());
					GameplayCueParams.TargetAttachComponent = LyraCharacter ? LyraCharacter->GetMesh() : HitResult.GetActor()->GetRootComponent();
					
					GameplayCueParams.Location = HitResult.ImpactPoint;
					GameplayCueParams.Normal = HitResult.ImpactNormal;
					GameplayCueParams.PhysicalMaterial = HitResult.PhysMaterial;
					TargetASC->ExecuteGameplayCue(BurstGameplayCueTag, GameplayCueParams);
				}
			}
		}
	}

	TargetDataHandle.Clear();
}

void UD1GameplayAbility_Skill_Targeting::OnMontageFinished()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UD1GameplayAbility_Skill_Targeting::OnInputConfirm()
{
	ConfirmTaskByInstanceName(TEXT("TargetDataTask"), true);
}

void UD1GameplayAbility_Skill_Targeting::OnInputCancel()
{
	CancelSkill();
}
