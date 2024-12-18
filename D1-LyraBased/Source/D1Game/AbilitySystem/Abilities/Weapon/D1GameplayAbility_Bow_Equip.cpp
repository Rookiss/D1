#include "D1GameplayAbility_Bow_Equip.h"

#include "D1GameplayTags.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Data/D1ItemData.h"
#include "Item/D1ItemInstance.h"
#include "Item/Managers/D1InventoryManagerComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1GameplayAbility_Bow_Equip)

UD1GameplayAbility_Bow_Equip::UD1GameplayAbility_Bow_Equip(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1GameplayAbility_Bow_Equip::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ALyraCharacter* LyraCharacter = GetLyraCharacterFromActorInfo();
	if (LyraCharacter == nullptr)
		return;

	UD1InventoryManagerComponent* InventoryManager = LyraCharacter->GetComponentByClass<UD1InventoryManagerComponent>();
	if (InventoryManager == nullptr)
		return;
	
	AD1EquipmentBase* WeaponActor = GetFirstEquipmentActor();
	UD1ItemInstance* ItemInstance = GetEquipmentItemInstance(WeaponActor);

	FGameplayTagContainer TagContainer = ItemInstance->GetOwnedTagContainer().GetTags();
	UAnimMontage* SelectedMontage;
	
	if (TagContainer.HasTag(D1GameplayTags::Ammo_Arrow))
	{
		SelectedMontage = EquipMontage_Arrow;
	}
	else
	{
		TArray<FIntPoint> OutToItemSlotPoses;
		TArray<int32> OutToItemCounts;
		const int32 ItemTemplateID = UD1ItemData::Get().FindItemTemplateIDByClass(ArrowItemTemplateClass);
		if (InventoryManager->CanRemoveItem(ItemTemplateID, 1, OutToItemSlotPoses, OutToItemCounts))
		{
			InventoryManager->TryRemoveItemByID(ItemTemplateID, 1);
			ItemInstance->AddOrRemoveOwnedTagStack(D1GameplayTags::Ammo_Arrow, 1);
			SelectedMontage = EquipMontage_Arrow;
		}
		else
		{
			SelectedMontage = EquipMontage_Empty;
		}
	}
	
	if (UAbilityTask_PlayMontageAndWait* EquipMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("EquipMontage"), SelectedMontage, 1.f, NAME_None, false))
	{
		EquipMontageTask->OnCompleted.AddDynamic(this, &ThisClass::OnMontageFinished);
		EquipMontageTask->OnBlendOut.AddDynamic(this, &ThisClass::OnMontageFinished);
		EquipMontageTask->OnInterrupted.AddDynamic(this, &ThisClass::OnMontageFinished);
		EquipMontageTask->OnCancelled.AddDynamic(this, &ThisClass::OnMontageFinished);
		EquipMontageTask->ReadyForActivation();
	}
}

void UD1GameplayAbility_Bow_Equip::OnMontageFinished()
{
	K2_EndAbility();
}
