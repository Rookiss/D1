#include "D1ItemInstance.h"

#if UE_WITH_IRIS
#include "Iris/ReplicationSystem/ReplicationFragmentUtil.h"
#endif // UE_WITH_IRIS

#include "Fragments/D1ItemFragment_Stackable.h"
#include "Net/UnrealNetwork.h"
#include "Data/D1ItemData.h"

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

void UD1ItemInstance::Init(int32 InTemplateID, EItemRarity InItemRarity)
{
	check(InTemplateID > 0 && InItemRarity != EItemRarity::Count);

	TemplateID = InTemplateID;
	ItemRarity = InItemRarity;
	
	const FD1ItemTemplate& ItemTemplate = UD1ItemData::Get().FindItemTemplateByID(TemplateID);
	for (const UD1ItemFragment* Fragment : ItemTemplate.Fragments)
	{
		if (Fragment)
		{
			Fragment->OnInstanceCreated(this);
		}
	}
}

void UD1ItemInstance::Init(int32 InTemplateID, const TArray<FD1ItemRarityProbability>& InItemProbabilities)
{
	Init(InTemplateID, DetermineItemRarity(InItemProbabilities));
}

void UD1ItemInstance::AddStatTagStack(const FGameplayTag& StatTag, int32 StackCount)
{
	StatContainer.AddStack(StatTag, StackCount);
}

void UD1ItemInstance::RemoveStatTagStack(const FGameplayTag& StatTag, int32 StackCount)
{
	StatContainer.RemoveStack(StatTag, StackCount);
}

int32 UD1ItemInstance::GetStatCountByTag(const FGameplayTag& StatTag) const
{
	return StatContainer.GetStackCount(StatTag);
}

bool UD1ItemInstance::HasStatTag(const FGameplayTag& StatTag) const
{
	return StatContainer.ContainsTag(StatTag);
}

EItemRarity UD1ItemInstance::DetermineItemRarity(const TArray<FD1ItemRarityProbability>& ItemProbabilities)
{
	EItemRarity ItemRarity = EItemRarity::Junk;
	
	float TotalPercent = 0.f;
	float RandValue = FMath::RandRange(0.f, 100.f);
		
	for (int32 i = 0; i < ItemProbabilities.Num(); i++)
	{
		TotalPercent += ItemProbabilities[i].Probability;
		if (RandValue <= TotalPercent)
		{
			ItemRarity = ItemProbabilities[i].Rarity;
			break;
		}
	}
	return ItemRarity;
}
