#include "D1GameplayAbility_Weapon_Spell_Projectile.h"

#include "D1GameplayTags.h"
#include "GameplayCueFunctionLibrary.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitConfirmCancel.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Actors/D1ProjectileBase.h"
#include "Actors/D1EquipmentBase.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "Kismet/KismetMathLibrary.h"
#include "Player/LyraPlayerController.h"
#include "UI/HUD/D1SkillInputWidget.h"
#include "UI/HUD/D1SkillProgressWidget.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1GameplayAbility_Weapon_Spell_Projectile)

UD1GameplayAbility_Weapon_Spell_Projectile::UD1GameplayAbility_Weapon_Spell_Projectile(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	Phase1_ProjectileClasses.SetNum(3);
	Phase2_ProjectileClasses.SetNum(3);
	Phase3_ProjectileClasses.SetNum(3);
	
	PhaseTimes.SetNum(2);
	PhaseColors.SetNum(3);
}

void UD1GameplayAbility_Weapon_Spell_Projectile::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	CurrentIndex = 0;

	if (UAbilityTask_PlayMontageAndWait* CastStartMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("CastStartMontage"), CastStartMontage, 1.f, NAME_None, true, 1.f, 0.f, false))
	{
		CastStartMontageTask->ReadyForActivation();
	}

	CastStartBeginEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, D1GameplayTags::GameplayEvent_Montage_Begin, nullptr, true, true);
	if (CastStartBeginEventTask)
	{
		CastStartBeginEventTask->EventReceived.AddDynamic(this, &ThisClass::OnCastStartBegin);
		CastStartBeginEventTask->ReadyForActivation();
	}

	InputCancelTask = UAbilityTask_WaitConfirmCancel::WaitConfirmCancel(this);
	if (InputCancelTask)
	{
		InputCancelTask->OnCancel.AddDynamic(this, &ThisClass::OnInputCancel);
		InputCancelTask->ReadyForActivation();
	}
}

void UD1GameplayAbility_Weapon_Spell_Projectile::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	ResetSpell();
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	FlushPressedInput(MainHandInputAction);
	FlushPressedInput(OffHandInputAction);
}

void UD1GameplayAbility_Weapon_Spell_Projectile::ConfirmSpell()
{
	ResetSpell();

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
		if (UAbilityTask_WaitGameplayEvent* SpellStartEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, D1GameplayTags::GameplayEvent_Montage_Spell, nullptr, true, true))
		{
			SpellStartEventTask->EventReceived.AddDynamic(this, &ThisClass::OnSpellStart);
			SpellStartEventTask->ReadyForActivation();
		}
	}
}

void UD1GameplayAbility_Weapon_Spell_Projectile::CancelSpell()
{
	ResetSpell();

	if (UAbilityTask_PlayMontageAndWait* CastEndMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("CastEndMontage"), CastEndMontage, 1.f, NAME_None, false))
	{
		CastEndMontageTask->ReadyForActivation();
	}

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UD1GameplayAbility_Weapon_Spell_Projectile::ResetSpell()
{
	FGameplayCueParameters Parameters;
	UGameplayCueFunctionLibrary::RemoveGameplayCueOnActor(GetAvatarActorFromActorInfo(), CastGameplayCueTag, Parameters);

	if (UGameplayMessageSubsystem::HasInstance(this))
	{
		UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(this);

		FSkillProgressInitializeMessage ProgressMessage;
		ProgressMessage.bShouldShow = false;
		MessageSubsystem.BroadcastMessage(D1GameplayTags::Message_HUD_Spell_Progress_Construct, ProgressMessage);

		FSkillInputInitializeMessage InputMessage;
		InputMessage.bShouldShow = false;
		MessageSubsystem.BroadcastMessage(D1GameplayTags::Message_HUD_Spell_Input, InputMessage);
	}

	if (CastStartBeginEventTask)
	{
		CastStartBeginEventTask->EndTask();
	}

	if (InputConfirmTask)
	{
		InputConfirmTask->EndTask();
	}

	if (InputCancelTask)
	{
		InputCancelTask->EndTask();
	}

	GetWorld()->GetTimerManager().ClearTimer(PhaseTimerHandle);
}

void UD1GameplayAbility_Weapon_Spell_Projectile::OnCastStartBegin(FGameplayEventData Payload)
{
	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(this);
	
	FSkillInputInitializeMessage InputMessage;
	InputMessage.bShouldShow = true;
	MessageSubsystem.BroadcastMessage(D1GameplayTags::Message_HUD_Spell_Input, InputMessage);

	InputConfirmTask = UAbilityTask_WaitConfirmCancel::WaitConfirmCancel(this);
	if (InputConfirmTask)
	{
		InputConfirmTask->OnConfirm.AddDynamic(this, &ThisClass::OnInputConfirm);
		InputConfirmTask->ReadyForActivation();
	}

	FGameplayCueParameters Parameters;
	Parameters.EffectCauser = GetFirstEquipmentActor();
	UGameplayCueFunctionLibrary::AddGameplayCueOnActor(GetAvatarActorFromActorInfo(), CastGameplayCueTag, Parameters);

	TotalCastTime = 0.f;
	
	for (float PhaseTime : PhaseTimes)
	{
		TotalCastTime += PhaseTime;
	}

	TotalCastTime *= UKismetMathLibrary::SafeDivide(1.f, GetSnapshottedAttackRate());

	FSkillProgressInitializeMessage ProgressMessage;
	ProgressMessage.bShouldShow = true;
	ProgressMessage.DisplayName = Name;
	ProgressMessage.PhaseColor = PhaseColors[CurrentIndex];
	ProgressMessage.TotalCastTime = TotalCastTime;
	MessageSubsystem.BroadcastMessage(D1GameplayTags::Message_HUD_Spell_Progress_Construct, ProgressMessage);

	float PhaseTime = PhaseTimes[CurrentIndex];
	PhaseTime *= UKismetMathLibrary::SafeDivide(1.f, GetSnapshottedAttackRate());
	GetWorld()->GetTimerManager().SetTimer(PhaseTimerHandle, this, &ThisClass::OnPhaseTimePassed, PhaseTime, false);
}

void UD1GameplayAbility_Weapon_Spell_Projectile::OnSpellStart(FGameplayEventData Payload)
{
	SpawnProjectile();
}

void UD1GameplayAbility_Weapon_Spell_Projectile::OnMontageFinished()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UD1GameplayAbility_Weapon_Spell_Projectile::OnInputConfirm()
{
	if (InputCancelTask)
	{
		InputCancelTask->EndTask();
	}

	bool bCommitted = CommitAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo);
	bCommitted ? ConfirmSpell() : CancelSpell();
}

void UD1GameplayAbility_Weapon_Spell_Projectile::OnInputCancel()
{
	CancelSpell();
}

void UD1GameplayAbility_Weapon_Spell_Projectile::OnPhaseTimePassed()
{
	CurrentIndex++;

	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(this);
	
	if (CurrentIndex < PhaseTimes.Num())
	{
		FSkillProgressRefreshMessage Message;
		Message.PhaseColor = PhaseColors[CurrentIndex];
		MessageSubsystem.BroadcastMessage(D1GameplayTags::Message_HUD_Spell_Progress_Refresh, Message);
		
		float PhaseTime = PhaseTimes[CurrentIndex];
		PhaseTime *= UKismetMathLibrary::SafeDivide(1.f, GetSnapshottedAttackRate());
		GetWorld()->GetTimerManager().SetTimer(PhaseTimerHandle, this, &ThisClass::OnPhaseTimePassed, PhaseTime, false);
	}
	else
	{
		FSkillProgressRefreshMessage Message;
		Message.PhaseColor = PhaseColors.Last();
		MessageSubsystem.BroadcastMessage(D1GameplayTags::Message_HUD_Spell_Progress_Refresh, Message);
	}
}

void UD1GameplayAbility_Weapon_Spell_Projectile::SpawnProjectile()
{
	if (HasAuthority(&CurrentActivationInfo) == false)
		return;
	
	AD1EquipmentBase* WeaponActor = GetFirstEquipmentActor();
	if (WeaponActor == nullptr)
		return;
	
	USkeletalMeshComponent* WeaponMeshComponent = WeaponActor->MeshComponent;
	if (WeaponMeshComponent == nullptr)
		return;

	ALyraPlayerController* LyraPlayerController = GetLyraPlayerControllerFromActorInfo();
	if (LyraPlayerController == nullptr)
		return;

#if UE_EDITOR
	ensure(WeaponMeshComponent->DoesSocketExist(ProjectileSocketName));
#endif
	
	if (WeaponMeshComponent->DoesSocketExist(ProjectileSocketName) == false)
		return;

	FVector CameraLocation;
	FRotator CameraRotation;
	LyraPlayerController->GetPlayerViewPoint(CameraLocation, CameraRotation);
	
	FVector CameraForward = CameraRotation.Vector();
	FVector WeaponSocketLocation = WeaponMeshComponent->GetSocketLocation(ProjectileSocketName);
	float Distance = CameraForward.Dot(WeaponSocketLocation - CameraLocation);
	
	FTransform SpawnTransform_Left;
	SpawnTransform_Left.SetLocation(CameraLocation + CameraForward * Distance);
	SpawnTransform_Left.SetRotation(CameraRotation.Quaternion() * FRotator(0.f, -DeltaDegree, 0.f).Quaternion());

	FTransform SpawnTransform_Center;
	SpawnTransform_Center.SetLocation(CameraLocation + CameraForward * Distance);
	SpawnTransform_Center.SetRotation(CameraRotation.Quaternion());
	
	FTransform SpawnTransform_Right;
	SpawnTransform_Right.SetLocation(CameraLocation + CameraForward * Distance);
	SpawnTransform_Right.SetRotation(CameraRotation.Quaternion() * FRotator(0.f, DeltaDegree, 0.f).Quaternion());

	TArray<FTransform> SpawnTransforms = { SpawnTransform_Left, SpawnTransform_Center, SpawnTransform_Right };
	
	TArray<TSubclassOf<AD1ProjectileBase>>* SelectedProjectileClasses = nullptr;
	switch (CurrentIndex)
	{
	case 0: SelectedProjectileClasses = &Phase1_ProjectileClasses; break;
	case 1: SelectedProjectileClasses = &Phase2_ProjectileClasses; break;
	case 2: SelectedProjectileClasses = &Phase3_ProjectileClasses; break;
	}

	if (SelectedProjectileClasses == nullptr)
		return;
	
	for (int i = 0; i < SelectedProjectileClasses->Num(); i++)
	{
		const TSubclassOf<AD1ProjectileBase>& ProjectileClass = (*SelectedProjectileClasses)[i];
		if (ProjectileClass == nullptr)
			continue;

		const FTransform& SpawnTransform = SpawnTransforms[i];
		
		AD1ProjectileBase* Projectile = GetWorld()->SpawnActorDeferred<AD1ProjectileBase>(
			ProjectileClass,
			SpawnTransform,
			GetAvatarActorFromActorInfo(),
			Cast<APawn>(GetAvatarActorFromActorInfo()),
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn
		);
	
		Projectile->FinishSpawning(SpawnTransform);
	}
}
