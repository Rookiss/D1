#include "D1ItemInstance.h"

#include "Fragments/D1ItemFragment_Stackable.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1ItemInstance)

UD1ItemInstance::UD1ItemInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1ItemInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, ItemID);
	DOREPLIFETIME(ThisClass, StatTags);
}

void UD1ItemInstance::Init(int32 InItemID)
{
	if (InItemID <= 0)
		return;

	ItemID = InItemID;
	ItemRarity = DetermineItemRarity();

	const UD1ItemData* ItemData = UD1AssetManager::GetItemData();
	const FD1ItemDefinition& ItemDef = ItemData->GetItemDefByID(ItemID);
	
	for (const UD1ItemFragment* Fragment : ItemDef.Fragments)
	{
		if (Fragment)
		{
			Fragment->OnInstanceCreated(this);
		}
	}
}

void UD1ItemInstance::AddStatTagStack(const FGameplayTag& StatTag, int32 StackCount)
{
	StatTags.AddStack(StatTag, StackCount);
}

void UD1ItemInstance::RemoveStatTagStack(const FGameplayTag& StatTag, int32 StackCount)
{
	StatTags.RemoveStack(StatTag, StackCount);
}

int32 UD1ItemInstance::GetStackCountByTag(const FGameplayTag& StatTag) const
{
	return StatTags.GetStackCountByTag(StatTag);
}

bool UD1ItemInstance::HasStatTag(const FGameplayTag& StatTag) const
{
	return StatTags.ContainsTag(StatTag);
}

FString UD1ItemInstance::GetDebugString() const
{
	return FString::Printf(TEXT("[ID : %d] : [%s]"), ItemID, *StatTags.GetDebugString());
}

EItemRarity UD1ItemInstance::DetermineItemRarity()
{
	const UD1ItemData* ItemData = UD1AssetManager::GetItemData();
	const TArray<FD1ItemProbability>& ItemProbabilities = ItemData->GetItemProbabilities();
	
	int32 TotalPercent = 0.f;
	int32 RandValue = FMath::RandRange(1, 100);

	for (int32 i = 0; i < ItemProbabilities.Num(); i++)
	{
		TotalPercent += ItemProbabilities[i].Probability;
		if (RandValue <= TotalPercent)
		{
			return ItemProbabilities[i].Rarity;
		}
	}
	return EItemRarity::Junk;
}
