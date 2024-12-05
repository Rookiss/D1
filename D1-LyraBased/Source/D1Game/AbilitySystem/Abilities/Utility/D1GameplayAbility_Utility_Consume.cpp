#include "D1GameplayAbility_Utility_Consume.h"

#include "Item/Managers/D1EquipmentManagerComponent.h"
#include "Item/Managers/D1InventoryManagerComponent.h"
#include "Item/Managers/D1ItemManagerComponent.h"
#include "Player/LyraPlayerController.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1GameplayAbility_Utility_Consume)

UD1GameplayAbility_Utility_Consume::UD1GameplayAbility_Utility_Consume(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1GameplayAbility_Utility_Consume::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	bItemUsed = false;
	
	ALyraPlayerController* LyraPlayerController = GetLyraPlayerControllerFromActorInfo();
	ALyraCharacter* LyraCharacter = GetLyraCharacterFromActorInfo();
	if (LyraPlayerController && LyraCharacter)
	{
		if (UD1ItemManagerComponent* ItemManager = LyraPlayerController->FindComponentByClass<UD1ItemManagerComponent>())
		{
			UD1InventoryManagerComponent* InventoryManager = LyraCharacter->FindComponentByClass<UD1InventoryManagerComponent>();
			ItemManager->RemoveAllowedComponent(InventoryManager);
			
			UD1EquipmentManagerComponent* EquipmentManager = LyraCharacter->FindComponentByClass<UD1EquipmentManagerComponent>();
			ItemManager->RemoveAllowedComponent(EquipmentManager);
		}
	}
}

void UD1GameplayAbility_Utility_Consume::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (bItemUsed)
	{
		CommitAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo);
	}
	
	ALyraPlayerController* LyraPlayerController = GetLyraPlayerControllerFromActorInfo();
	ALyraCharacter* LyraCharacter = GetLyraCharacterFromActorInfo();
	if (LyraPlayerController && LyraCharacter)
	{
		if (UD1ItemManagerComponent* ItemManager = LyraPlayerController->FindComponentByClass<UD1ItemManagerComponent>())
		{
			UD1InventoryManagerComponent* InventoryManager = LyraCharacter->FindComponentByClass<UD1InventoryManagerComponent>();
			ItemManager->AddAllowedComponent(InventoryManager);
			
			UD1EquipmentManagerComponent* EquipmentManager = LyraCharacter->FindComponentByClass<UD1EquipmentManagerComponent>();
			ItemManager->AddAllowedComponent(EquipmentManager);
		}
	}
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
