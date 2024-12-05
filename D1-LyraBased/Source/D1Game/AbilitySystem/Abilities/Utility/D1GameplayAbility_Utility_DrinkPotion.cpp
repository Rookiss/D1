#include "D1GameplayAbility_Utility_DrinkPotion.h"

#include "D1GameplayTags.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Actors/D1EquipmentBase.h"
#include "Character/LyraCharacter.h"
#include "Data/D1ItemData.h"
#include "Item/D1ItemInstance.h"
#include "Item/D1ItemTemplate.h"
#include "Item/Fragments/D1ItemFragment_Equipable_Utility.h"
#include "Item/Managers/D1EquipManagerComponent.h"
#include "Item/Managers/D1EquipmentManagerComponent.h"
#include "Player/LyraPlayerController.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1GameplayAbility_Utility_DrinkPotion)

UD1GameplayAbility_Utility_DrinkPotion::UD1GameplayAbility_Utility_DrinkPotion(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    AbilityTags.AddTag(D1GameplayTags::Ability_Drink);
	ActivationOwnedTags.AddTag(D1GameplayTags::Status_Drink);
}

void UD1GameplayAbility_Utility_DrinkPotion::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (K2_CheckAbilityCost() == false)
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}
	
	if (UAbilityTask_PlayMontageAndWait* DrinkMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("DrinkMontage"), DrinkMontage, 1.f, NAME_None, true))
	{
		DrinkMontageTask->OnCompleted.AddDynamic(this, &ThisClass::OnMontageFinished);
		DrinkMontageTask->OnBlendOut.AddDynamic(this, &ThisClass::OnMontageFinished);
		DrinkMontageTask->OnInterrupted.AddDynamic(this, &ThisClass::OnMontageFinished);
		DrinkMontageTask->OnCancelled.AddDynamic(this, &ThisClass::OnMontageFinished);
		DrinkMontageTask->ReadyForActivation();
	}

	if (UAbilityTask_WaitGameplayEvent* DrinkBeginEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, D1GameplayTags::GameplayEvent_Montage_Begin, nullptr, true, true))
	{
		DrinkBeginEventTask->EventReceived.AddDynamic(this, &ThisClass::OnDrinkBegin);
		DrinkBeginEventTask->ReadyForActivation();
	}
}

void UD1GameplayAbility_Utility_DrinkPotion::OnDrinkBegin(FGameplayEventData Payload)
{
	bItemUsed = true;
	ApplyUtilityGameplayEffect();
}

void UD1GameplayAbility_Utility_DrinkPotion::OnMontageFinished()
{
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		FGameplayEventData Payload;
		ASC->HandleGameplayEvent(D1GameplayTags::GameplayEvent_Potion_Fill, &Payload);
	}

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UD1GameplayAbility_Utility_DrinkPotion::ApplyUtilityGameplayEffect()
{
	AD1EquipmentBase* EquipmentActor = GetFirstEquipmentActor();
	if (EquipmentActor == nullptr)
		return;
	
	const int32 ItemTemplateID = EquipmentActor->GetTemplateID();
	const UD1ItemTemplate& ItemTemplate = UD1ItemData::Get().FindItemTemplateByID(ItemTemplateID);

	const UD1ItemFragment_Equipable_Utility* UtilityFragment = ItemTemplate.FindFragmentByClass<UD1ItemFragment_Equipable_Utility>();
	if (UtilityFragment == nullptr)
		return;

	TSubclassOf<UGameplayEffect> UtilityEffectClass = UtilityFragment->UtilityEffectClass;
	if (UtilityEffectClass == nullptr)
		return;

	ALyraCharacter* LyraCharacter = GetLyraCharacterFromActorInfo();
	if (LyraCharacter == nullptr)
		return;

	UD1EquipManagerComponent* EquipManager = LyraCharacter->GetComponentByClass<UD1EquipManagerComponent>();
	UD1EquipmentManagerComponent* EquipmentManager = LyraCharacter->GetComponentByClass<UD1EquipmentManagerComponent>();
	if (EquipManager == nullptr || EquipmentManager == nullptr)
		return;
	
	UD1ItemInstance* ItemInstance = GetEquipmentItemInstance(EquipmentActor);
	if (ItemInstance == nullptr)
		return;

	FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(UtilityEffectClass, (float)ItemInstance->GetItemRarity());
	ApplyGameplayEffectSpecToOwner(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), SpecHandle);
}
