#include "D1ItemInstance.h"

#if UE_WITH_IRIS
#include "Iris/ReplicationSystem/ReplicationFragmentUtil.h"
#endif // UE_WITH_IRIS

#include "D1ItemTemplate.h"
#include "Data/D1ItemData.h"
#include "Net/UnrealNetwork.h"

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
}

void UD1ItemInstance::Init(int32 InItemTemplateID, EItemRarity InItemRarity)
{
	if (InItemTemplateID <= 0 || InItemRarity == EItemRarity::Count)
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

void UD1ItemInstance::AddStatTagStack(FGameplayTag StatTag, int32 StackCount)
{
	StatContainer.AddStack(StatTag, StackCount);
}

void UD1ItemInstance::RemoveStatTagStack(FGameplayTag StatTag, int32 StackCount)
{
	StatContainer.RemoveStack(StatTag, StackCount);
}

EItemRarity UD1ItemInstance::DetermineItemRarity(const TArray<FD1ItemRarityProbability>& ItemProbabilities)
{
	EItemRarity ItemRarity = EItemRarity::Junk;

	float TotalPercent = 0.f;
	float RandValue = FMath::RandRange(0.f, 100.f);

	for (const FD1ItemRarityProbability& ItemProbability : ItemProbabilities)
	{
		TotalPercent += ItemProbability.Probability;
		if (RandValue <= TotalPercent)
		{
			ItemRarity = ItemProbability.Rarity;
			break;
		}
	}

	check(FMath::IsNearlyEqual(TotalPercent, 100.f));
	return ItemRarity;
}

bool UD1ItemInstance::HasStatTag(FGameplayTag StatTag) const
{
	return StatContainer.ContainsTag(StatTag);
}

int32 UD1ItemInstance::GetStatCountByTag(FGameplayTag StatTag) const
{
	return StatContainer.GetStackCount(StatTag);
}

const UD1ItemFragment* UD1ItemInstance::FindFragmentByClass(TSubclassOf<UD1ItemFragment> FragmentClass) const
{
	if (ItemTemplateID > 0 && FragmentClass)
	{
		const UD1ItemTemplate& ItemTemplate = UD1ItemData::Get().FindItemTemplateByID(ItemTemplateID);
		return ItemTemplate.FindFragmentByClass(FragmentClass);
	}
	return nullptr;
}
