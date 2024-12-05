#include "D1GameplayAbility_Utility_ThrowTorch.h"

#include "D1GameplayTags.h"
#include "Abilities/Tasks/AbilityTask_NetworkSyncPoint.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitConfirmCancel.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitInputRelease.h"
#include "Actors/D1PickupableItemBase.h"
#include "Actors/D1EquipmentBase.h"
#include "Character/LyraCharacter.h"
#include "Data/D1ItemData.h"
#include "Interaction/D1Pickupable.h"
#include "Item/D1ItemTemplate.h"
#include "Item/Fragments/D1ItemFragment_Equipable_Utility.h"
#include "Item/Managers/D1EquipManagerComponent.h"
#include "Item/Managers/D1EquipmentManagerComponent.h"
#include "Kismet/KismetMathLibrary.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1GameplayAbility_Utility_ThrowTorch)

UD1GameplayAbility_Utility_ThrowTorch::UD1GameplayAbility_Utility_ThrowTorch(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    AbilityTags.AddTag(D1GameplayTags::Ability_ADS_Throw);
    AbilityTags.AddTag(D1GameplayTags::Ability_Throw);
	ActivationOwnedTags.AddTag(D1GameplayTags::Status_ADS_Throw);
	ActivationOwnedTags.AddTag(D1GameplayTags::Status_Throw);
}

void UD1GameplayAbility_Utility_ThrowTorch::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (K2_CheckAbilityCost() == false)
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}
	
	SetCameraMode(CameraModeClass);

	if (UAbilityTask_PlayMontageAndWait* ThrowStartMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("ThrowStartMontage"), ThrowStartMontage, 1.f, NAME_None, true))
	{
		ThrowStartMontageTask->ReadyForActivation();
	}

	InputReleaseTask = UAbilityTask_WaitInputRelease::WaitInputRelease(this, true);
	if (InputReleaseTask)
	{
		InputReleaseTask->OnRelease.AddDynamic(this, &ThisClass::OnInputRelease);
		InputReleaseTask->ReadyForActivation();
	}

	if (UAbilityTask_WaitConfirmCancel* ConfirmCancelTask = UAbilityTask_WaitConfirmCancel::WaitConfirmCancel(this))
	{
		ConfirmCancelTask->OnConfirm.AddDynamic(this, &ThisClass::OnInputConfirm);
		ConfirmCancelTask->ReadyForActivation();
	}
}

void UD1GameplayAbility_Utility_ThrowTorch::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	ClearCameraMode();
	
	if (UD1EquipManagerComponent* EquipManager = GetLyraCharacterFromActorInfo()->GetComponentByClass<UD1EquipManagerComponent>())
	{
		EquipManager->ChangeShouldHiddenEquipments(false);
	}
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UD1GameplayAbility_Utility_ThrowTorch::OnInputRelease(float TimeHeld)
{
	if (UAbilityTask_PlayMontageAndWait* ThrowEndMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("ThrowEndMontage"), ThrowEndMontage, 1.f, NAME_None, false))
	{
		ThrowEndMontageTask->OnCompleted.AddDynamic(this, &ThisClass::OnThrowEndFinished);
		ThrowEndMontageTask->OnBlendOut.AddDynamic(this, &ThisClass::OnThrowEndFinished);
		ThrowEndMontageTask->OnInterrupted.AddDynamic(this, &ThisClass::OnThrowEndFinished);
		ThrowEndMontageTask->OnCancelled.AddDynamic(this, &ThisClass::OnThrowEndFinished);
		ThrowEndMontageTask->ReadyForActivation();
	}
}

void UD1GameplayAbility_Utility_ThrowTorch::OnInputConfirm()
{
	if (InputReleaseTask)
	{
		InputReleaseTask->EndTask();
	}

	if (UAbilityTask_PlayMontageAndWait* ThrowMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("ThrowMontage"), ThrowMontage, 1.f, NAME_None, false))
	{
		ThrowMontageTask->OnCompleted.AddDynamic(this, &ThisClass::OnThrowFinished);
		ThrowMontageTask->OnBlendOut.AddDynamic(this, &ThisClass::OnThrowFinished);
		ThrowMontageTask->OnInterrupted.AddDynamic(this, &ThisClass::OnThrowFinished);
		ThrowMontageTask->OnCancelled.AddDynamic(this, &ThisClass::OnThrowFinished);
		ThrowMontageTask->ReadyForActivation();
	}

	if (UAbilityTask_WaitGameplayEvent* ThrowEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, D1GameplayTags::GameplayEvent_Montage_Begin, nullptr, true, true))
	{
		ThrowEventTask->EventReceived.AddDynamic(this, &ThisClass::OnThrowEvent);
		ThrowEventTask->ReadyForActivation();
	}
}

void UD1GameplayAbility_Utility_ThrowTorch::OnThrowFinished()
{
	if (UAbilityTask_NetworkSyncPoint* NetworkSyncTask = UAbilityTask_NetworkSyncPoint::WaitNetSync(this, EAbilityTaskNetSyncType::OnlyClientWait))
	{
		NetworkSyncTask->OnSync.AddDynamic(this, &ThisClass::OnNetSync);
		NetworkSyncTask->ReadyForActivation();
	}
}

void UD1GameplayAbility_Utility_ThrowTorch::OnThrowEndFinished()
{
	CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
}

void UD1GameplayAbility_Utility_ThrowTorch::OnThrowEvent(FGameplayEventData Payload)
{
	if (UD1EquipManagerComponent* EquipManager = GetLyraCharacterFromActorInfo()->GetComponentByClass<UD1EquipManagerComponent>())
	{
		EquipManager->ChangeShouldHiddenEquipments(true);
	}

	if (HasAuthority(&CurrentActivationInfo))
	{
		SpawnThrowableItem();
	}

	bItemUsed = true;
}

void UD1GameplayAbility_Utility_ThrowTorch::OnNetSync()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

FTransform UD1GameplayAbility_Utility_ThrowTorch::GetSpawnTransform()
{
	AD1EquipmentBase* WeaponActor = GetFirstEquipmentActor();
	if (WeaponActor == nullptr)
		return FTransform::Identity;
	
	FTransform SpawnTransform = FTransform::Identity;
	
	if (ALyraCharacter* LyraCharacter = GetLyraCharacterFromActorInfo())
	{
		int32 ItemTemplateID = WeaponActor->GetTemplateID();
		const UD1ItemTemplate& ItemTemplate = UD1ItemData::Get().FindItemTemplateByID(ItemTemplateID);

		if (const UD1ItemFragment_Equipable_Utility* UtilityFragment = ItemTemplate.FindFragmentByClass<UD1ItemFragment_Equipable_Utility>())
		{
			SpawnTransform = UtilityFragment->WeaponAttachInfo.AttachTransform;
			const FTransform& SocketTransform = LyraCharacter->GetMesh()->GetSocketTransform(UtilityFragment->WeaponAttachInfo.AttachSocket);
			SpawnTransform *= SocketTransform;
			
			FRotator LocalRotation = FRotator(0.f, 180.f, 90.f);
			SpawnTransform.SetRotation(UKismetMathLibrary::TransformRotation(LyraCharacter->GetTransform(), LocalRotation).Quaternion());
		}
	}

	return SpawnTransform;
}

void UD1GameplayAbility_Utility_ThrowTorch::SpawnThrowableItem()
{
	ALyraCharacter* LyraCharacter = Cast<ALyraCharacter>(GetLyraCharacterFromActorInfo());
	if (LyraCharacter == nullptr)
		return;

	UD1EquipManagerComponent* EquipManager = LyraCharacter->GetComponentByClass<UD1EquipManagerComponent>();
	if (EquipManager == nullptr)
		return;

	AD1EquipmentBase* WeaponActor = GetFirstEquipmentActor();
	if (WeaponActor == nullptr)
		return;
	
	EEquipmentSlotType EquipmentSlotType = WeaponActor->GetEquipmentSlotType();
	if (EquipmentSlotType == EEquipmentSlotType::Count)
		return;

	UD1EquipmentManagerComponent* EquipmentManager = LyraCharacter->GetComponentByClass<UD1EquipmentManagerComponent>();
	if (EquipmentManager == nullptr)
		return;

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = GetLyraCharacterFromActorInfo();
	
	AD1PickupableItemBase* PickupableItemActor = GetWorld()->SpawnActor<AD1PickupableItemBase>(ThrowableItemClass, GetSpawnTransform(), SpawnParameters);
	
	FD1PickupInfo PickupInfo;
	PickupInfo.PickupInstance.ItemInstance = EquipmentManager->GetItemInstance(EquipmentSlotType);
	PickupInfo.PickupInstance.ItemCount = 1;
	PickupableItemActor->SetPickupInfo(PickupInfo);
}
