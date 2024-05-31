#include "D1ItemInstance.h"

#if UE_WITH_IRIS
#include "Iris/ReplicationSystem/ReplicationFragmentUtil.h"
#endif // UE_WITH_IRIS

#include "D1ItemDefinition.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1ItemInstance)

UD1ItemInstance::UD1ItemInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1ItemInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, ItemDefinition);
	DOREPLIFETIME(ThisClass, ItemRarity);
	DOREPLIFETIME(ThisClass, StatContainer);
}

void UD1ItemInstance::SetItemDefinition(TSubclassOf<UD1ItemDefinition> InItemDefinition, EItemRarity InItemRarity)
{
	if (InItemDefinition == nullptr || InItemRarity == EItemRarity::Count)
		return;

	ItemDefinition = InItemDefinition;
	ItemRarity = InItemRarity;
	
	for (const UD1ItemFragment* Fragment : GetDefault<UD1ItemDefinition>(ItemDefinition)->Fragments)
	{
		if (Fragment)
		{
			Fragment->OnInstanceCreated(this);
		}
	}
}

void UD1ItemInstance::SetItemDefinition(TSubclassOf<UD1ItemDefinition> InItemDefinition, const TArray<FD1ItemRarityProbability>& InItemProbabilities)
{
	SetItemDefinition(InItemDefinition, DetermineItemRarity(InItemProbabilities));
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
	if (ItemDefinition && FragmentClass)
	{
		return GetDefault<UD1ItemDefinition>(ItemDefinition)->FindFragmentByClass(FragmentClass);
	}
	return nullptr;
}
