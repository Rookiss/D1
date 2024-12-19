#include "D1ItemInstance.h"

#if UE_WITH_IRIS
#include "Iris/ReplicationSystem/ReplicationFragmentUtil.h"
#endif // UE_WITH_IRIS

#include "D1ItemTemplate.h"
#include "Data/D1ItemData.h"
#include "Fragments/D1ItemFragment_Equipable_Attachment.h"
#include "Net/UnrealNetwork.h"
#include "Physics/PhysicalMaterialWithTags.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1ItemInstance)

UD1ItemInstance::UD1ItemInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1ItemInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, ItemTemplateID);
	DOREPLIFETIME(ThisClass, ItemRarity);
	
	DOREPLIFETIME(ThisClass, StatContainer);
	DOREPLIFETIME(ThisClass, OwnedTagContainer);
}

void UD1ItemInstance::Init(int32 InItemTemplateID, EItemRarity InItemRarity)
{
	if (InItemTemplateID <= INDEX_NONE || InItemRarity == EItemRarity::Count)
		return;

	ItemTemplateID = InItemTemplateID;
	ItemRarity = InItemRarity;

	const UD1ItemTemplate& ItemTemplate = UD1ItemData::Get().FindItemTemplateByID(ItemTemplateID);
	for (const UD1ItemFragment* Fragment : ItemTemplate.Fragments)
	{
		if (Fragment)
		{
			Fragment->OnInstanceCreated(this);
		}
	}
}

#if UE_WITH_IRIS
void UD1ItemInstance::RegisterReplicationFragments(UE::Net::FFragmentRegistrationContext& Context, UE::Net::EFragmentRegistrationFlags RegistrationFlags)
{
	using namespace UE::Net;
	FReplicationFragmentUtil::CreateAndRegisterFragmentsForObject(this, Context, RegistrationFlags);
}
#endif // UE_WITH_IRIS

void UD1ItemInstance::AddOrRemoveStatTagStack(FGameplayTag StatTag, int32 StackCount)
{
	StatContainer.AddOrRemoveStack(StatTag, StackCount);
}

void UD1ItemInstance::RemoveStatTagStack(FGameplayTag StatTag)
{
	StatContainer.RemoveStack(StatTag);
}

void UD1ItemInstance::AddOrRemoveOwnedTagStack(FGameplayTag OwnedTag, int32 StackCount)
{
	OwnedTagContainer.AddOrRemoveStack(OwnedTag, StackCount);
}

void UD1ItemInstance::RemoveOwnedTagStack(FGameplayTag OwnedTag)
{
	OwnedTagContainer.RemoveStack(OwnedTag);
}

EItemRarity UD1ItemInstance::DetermineItemRarity(const TArray<FD1ItemRarityProbability>& ItemProbabilities)
{
	float TotalProbability = 0.f;
	for (const FD1ItemRarityProbability& ItemProbability : ItemProbabilities)
	{
		TotalProbability += ItemProbability.Probability;
	}

	if (TotalProbability > 100.f)
		return EItemRarity::Count;
	
	float SumProbability = 0.f;
	float RandomValue = FMath::RandRange(0.f, 100.f);

	for (const FD1ItemRarityProbability& ItemProbability : ItemProbabilities)
	{
		SumProbability += ItemProbability.Probability;
		if (RandomValue < SumProbability)
		{
			return ItemProbability.Rarity;
		}
	}
	
	return EItemRarity::Count;
}

bool UD1ItemInstance::HasStatTag(FGameplayTag StatTag) const
{
	return StatContainer.ContainsTag(StatTag);
}

int32 UD1ItemInstance::GetStatCountByTag(FGameplayTag StatTag) const
{
	return StatContainer.GetStackCount(StatTag);
}

bool UD1ItemInstance::HasOwnedTag(FGameplayTag OwnedTag) const
{
	return OwnedTagContainer.ContainsTag(OwnedTag);
}

int32 UD1ItemInstance::GetOwnedCountByTag(FGameplayTag OwnedTag) const
{
	return OwnedTagContainer.GetStackCount(OwnedTag);
}

const UD1ItemFragment* UD1ItemInstance::FindFragmentByClass(TSubclassOf<UD1ItemFragment> FragmentClass) const
{
	if (ItemTemplateID > INDEX_NONE && FragmentClass)
	{
		const UD1ItemTemplate& ItemTemplate = UD1ItemData::Get().FindItemTemplateByID(ItemTemplateID);
		return ItemTemplate.FindFragmentByClass(FragmentClass);
	}
	return nullptr;
}
