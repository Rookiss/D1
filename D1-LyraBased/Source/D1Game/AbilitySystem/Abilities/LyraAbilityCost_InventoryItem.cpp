#include "LyraAbilityCost_InventoryItem.h"

#include "LyraGameplayAbility.h"
#include "Character/LyraCharacter.h"
#include "Data/D1ItemData.h"
#include "Item/D1ItemTemplate.h"
#include "Item/Managers/D1InventoryManagerComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(LyraAbilityCost_InventoryItem)

ULyraAbilityCost_InventoryItem::ULyraAbilityCost_InventoryItem()
{
	Quantity.SetValue(1.f);
}

bool ULyraAbilityCost_InventoryItem::CheckCost(const ULyraGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (ItemTemplateClass == nullptr)
		return false;

	const int32 AbilityLevel = Ability->GetAbilityLevel(Handle, ActorInfo);
	const float NumItemsToConsumeReal = Quantity.GetValueAtLevel(AbilityLevel);
	int32 NumItemsToConsume = FMath::TruncToInt(NumItemsToConsumeReal);
	if (NumItemsToConsume < 0)
		return false;

	if (NumItemsToConsume == 0)
		return true;
	
	ALyraCharacter* LyraCharacter = Ability->GetLyraCharacterFromActorInfo();
	if (LyraCharacter == nullptr)
		return false;

	UD1InventoryManagerComponent* InventoryManager = LyraCharacter->GetComponentByClass<UD1InventoryManagerComponent>();
	if (InventoryManager == nullptr)
		return false;

	TArray<FIntPoint> OutToItemSlotPoses;
	TArray<int32> OutToItemCounts;
	const int32 ItemTemplateID = UD1ItemData::Get().FindItemTemplateIDByClass(ItemTemplateClass);
	
	return InventoryManager->CanRemoveItem(ItemTemplateID, NumItemsToConsume, OutToItemSlotPoses, OutToItemCounts);
}

void ULyraAbilityCost_InventoryItem::ApplyCost(const ULyraGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	if (ItemTemplateClass == nullptr)
		return;

	const int32 AbilityLevel = Ability->GetAbilityLevel(Handle, ActorInfo);
	const float NumItemsToConsumeReal = Quantity.GetValueAtLevel(AbilityLevel);
	int32 NumItemsToConsume = FMath::TruncToInt(NumItemsToConsumeReal);
	if (NumItemsToConsume < 0)
		return;

	if (NumItemsToConsume == 0)
		return;
	
	ALyraCharacter* LyraCharacter = Ability->GetLyraCharacterFromActorInfo();
	if (LyraCharacter == nullptr)
		return;

	UD1InventoryManagerComponent* InventoryManager = LyraCharacter->GetComponentByClass<UD1InventoryManagerComponent>();
	if (InventoryManager == nullptr)
		return;

	const int32 ItemTemplateID = UD1ItemData::Get().FindItemTemplateIDByClass(ItemTemplateClass);
 	InventoryManager->TryRemoveItemByID(ItemTemplateID, NumItemsToConsume);
}
