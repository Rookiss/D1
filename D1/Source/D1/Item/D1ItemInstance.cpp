#include "D1ItemInstance.h"

#include "Fragments/D1ItemFragment_Stackable.h"
#include "Net/UnrealNetwork.h"
#if UE_WITH_IRIS
#include "Iris/ReplicationSystem/ReplicationFragmentUtil.h"
#endif // UE_WITH_IRIS

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1ItemInstance)

UD1ItemInstance::UD1ItemInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1ItemInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, TemplateID);
	DOREPLIFETIME(ThisClass, ItemRarity);
	DOREPLIFETIME(ThisClass, StatContainer);
}

#if UE_WITH_IRIS
void UD1ItemInstance::RegisterReplicationFragments(UE::Net::FFragmentRegistrationContext& Context, UE::Net::EFragmentRegistrationFlags RegistrationFlags)
{
	using namespace UE::Net;
	FReplicationFragmentUtil::CreateAndRegisterFragmentsForObject(this, Context, RegistrationFlags);
}
#endif // UE_WITH_IRIS

void UD1ItemInstance::Init(int32 InTemplateID)
{
	if (InTemplateID <= 0)
		return;

	TemplateID = InTemplateID;
	ItemRarity = DetermineItemRarity();
	
	const FD1ItemTemplate& ItemTemplate = UD1AssetManager::GetItemTemplate(TemplateID);
	for (const UD1ItemFragment* Fragment : ItemTemplate.Fragments)
	{
		if (Fragment)
		{
			Fragment->OnInstanceCreated(this);
		}
	}
}

void UD1ItemInstance::AddStatTagStack(const FGameplayTag& StatTag, int32 StackCount)
{
	StatContainer.AddStack(StatTag, StackCount);
}

void UD1ItemInstance::RemoveStatTagStack(const FGameplayTag& StatTag, int32 StackCount)
{
	StatContainer.RemoveStack(StatTag, StackCount);
}

int32 UD1ItemInstance::GetStackCountByTag(const FGameplayTag& StatTag) const
{
	return StatContainer.GetStackCountByTag(StatTag);
}

bool UD1ItemInstance::HasStatTag(const FGameplayTag& StatTag) const
{
	return StatContainer.ContainsTag(StatTag);
}

EItemRarity UD1ItemInstance::DetermineItemRarity()
{
	const UD1ItemData* ItemData = UD1AssetManager::GetItemData();
	const TArray<FD1ItemRarityProbability>& ItemProbabilities = ItemData->GetRarityProbabilities();
	
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
