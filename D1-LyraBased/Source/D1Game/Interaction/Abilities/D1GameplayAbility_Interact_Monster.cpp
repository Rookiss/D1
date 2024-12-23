#include "D1GameplayAbility_Interact_Monster.h"

#include "CommonActivatableWidget.h"
#include "D1GameplayTags.h"
#include "Actions/AsyncAction_PushContentToLayerForPlayer.h"
#include "Character/LyraCharacter.h"
#include "Character/LyraHealthComponent.h"
#include "Item/Managers/D1InventoryManagerComponent.h"
#include "Item/Managers/D1EquipManagerComponent.h"
#include "Item/Managers/D1EquipmentManagerComponent.h"
#include "Item/Managers/D1ItemManagerComponent.h"
#include "Player/LyraPlayerController.h"
#include "UI/Item/Equipment/D1EquipmentSlotsWidget.h"
#include "UI/Item/Inventory/D1InventorySlotsWidget.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1GameplayAbility_Interact_Monster)

UD1GameplayAbility_Interact_Monster::UD1GameplayAbility_Interact_Monster(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1GameplayAbility_Interact_Monster::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (TriggerEventData == nullptr || bInitialized == false)
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}

	ULyraHealthComponent* HealthComponent = InteractableActor->GetComponentByClass<ULyraHealthComponent>();
	if (HealthComponent == nullptr || HealthComponent->IsDeadOrDying() == false)
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}
	
	if (HasAuthority(&CurrentActivationInfo))
	{
		UD1ItemManagerComponent* MyItemManager = GetLyraPlayerControllerFromActorInfo()->GetComponentByClass<UD1ItemManagerComponent>();
		UD1InventoryManagerComponent* OtherInventoryManager = InteractableActor->GetComponentByClass<UD1InventoryManagerComponent>();
		MyItemManager->AddAllowedComponent(OtherInventoryManager);
	}

	if (IsLocallyControlled())
	{
		if (UAsyncAction_PushContentToLayerForPlayer* PushWidgetAction = UAsyncAction_PushContentToLayerForPlayer::PushContentToLayerForPlayer(GetLyraPlayerControllerFromActorInfo(), WidgetClass, WidgetLayerTag, true))
		{
			PushWidgetAction->AfterPush.AddDynamic(this, &ThisClass::OnAfterPushWidget);
			PushWidgetAction->Activate();
		}
	}
}

void UD1GameplayAbility_Interact_Monster::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (HasAuthority(&CurrentActivationInfo))
	{
		if (UD1ItemManagerComponent* MyItemManager = GetLyraPlayerControllerFromActorInfo()->GetComponentByClass<UD1ItemManagerComponent>())
		{
			UD1InventoryManagerComponent* OtherInventoryManager = InteractableActor->GetComponentByClass<UD1InventoryManagerComponent>();
			MyItemManager->RemoveAllowedComponent(OtherInventoryManager);
		}
	}

	if (IsLocallyControlled() && PushedWidget)
	{
		PushedWidget->DeactivateWidget();
	}
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UD1GameplayAbility_Interact_Monster::OnAfterPushWidget(UCommonActivatableWidget* InPushedWidget)
{
	PushedWidget = InPushedWidget;
	
	ALyraCharacter* LyraCharacter = GetLyraCharacterFromActorInfo();
	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(this);
	
	FInventoryInitializeMessage MyInventoryInitMessage;
	MyInventoryInitMessage.InventoryManager = LyraCharacter->GetComponentByClass<UD1InventoryManagerComponent>();
	MessageSubsystem.BroadcastMessage(D1GameplayTags::Message_Initialize_MyInventory, MyInventoryInitMessage);
	
	FEquipmentInitializeMessage MyEquipmentInitMessage;
	MyEquipmentInitMessage.EquipmentManager = LyraCharacter->GetComponentByClass<UD1EquipmentManagerComponent>();
	MyEquipmentInitMessage.EquipManager = LyraCharacter->GetComponentByClass<UD1EquipManagerComponent>();
	MessageSubsystem.BroadcastMessage(D1GameplayTags::Message_Initialize_MyEquipment, MyEquipmentInitMessage);

	FInventoryInitializeMessage OtherInventoryInitMessage;
	OtherInventoryInitMessage.InventoryManager = InteractableActor->GetComponentByClass<UD1InventoryManagerComponent>();
	MessageSubsystem.BroadcastMessage(D1GameplayTags::Message_Initialize_OtherInventory, OtherInventoryInitMessage);

	InPushedWidget->OnDeactivated().AddWeakLambda(this, [this]()
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	});
}
