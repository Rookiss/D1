#include "D1SecondarySet.h"

#include "D1GameplayTags.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1SecondarySet)

UD1SecondarySet::UD1SecondarySet()
{
	TagToAttributeFunc.Add(D1GameplayTags::Attribute_Secondary_Strength, GetStrengthAttribute);
	TagToAttributeFunc.Add(D1GameplayTags::Attribute_Secondary_Vigor, GetVigorAttribute);
	TagToAttributeFunc.Add(D1GameplayTags::Attribute_Secondary_Agility, GetAgilityAttribute);
	TagToAttributeFunc.Add(D1GameplayTags::Attribute_Secondary_Dexterity, GetDexterityAttribute);
	TagToAttributeFunc.Add(D1GameplayTags::Attribute_Secondary_Will, GetWillAttribute);
	TagToAttributeFunc.Add(D1GameplayTags::Attribute_Secondary_Knowledge, GetKnowledgeAttribute);
	TagToAttributeFunc.Add(D1GameplayTags::Attribute_Secondary_Resourcefulness, GetResourcefulnessAttribute);
}

void UD1SecondarySet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, Strength, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, Vigor, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, Agility, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, Dexterity, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, Will, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, Knowledge, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, Resourcefulness, COND_None, REPNOTIFY_Always);
}

void UD1SecondarySet::OnRep_Strength(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, Strength, OldValue);
}

void UD1SecondarySet::OnRep_Vigor(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, Vigor, OldValue);
}

void UD1SecondarySet::OnRep_Agility(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, Agility, OldValue);
}

void UD1SecondarySet::OnRep_Dexterity(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, Dexterity, OldValue);
}

void UD1SecondarySet::OnRep_Will(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, Will, OldValue);
}

void UD1SecondarySet::OnRep_Knowledge(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, Knowledge, OldValue);
}

void UD1SecondarySet::OnRep_Resourcefulness(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, Resourcefulness, OldValue);
}
