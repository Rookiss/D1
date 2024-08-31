#pragma once

#include "AbilitySystemBlueprintLibrary.h"
#include "D1InteractionInfo.h"
#include "D1InteractionQuery.h"
#include "AbilitySystem/Attributes/D1CombatSet.h"
#include "UObject/Interface.h"
#include "D1Interactable.generated.h"

class ID1Interactable;
struct FD1InteractionInfo;
struct FD1InteractionQuery;

class FD1InteractionInfoBuilder
{
public:
	FD1InteractionInfoBuilder(TScriptInterface<ID1Interactable> InInteractable, TArray<FD1InteractionInfo>& InInteractionInfos)
		: Interactable(InInteractable)
		, InteractionInfos(InInteractionInfos) { }

public:
	void AddInteractionInfo(const FD1InteractionInfo& InteractionInfo)
	{
		FD1InteractionInfo& Entry = InteractionInfos.Add_GetRef(InteractionInfo);
		Entry.Interactable = Interactable;
	}
	
private:
	TScriptInterface<ID1Interactable> Interactable;
	TArray<FD1InteractionInfo>& InteractionInfos;
};

UINTERFACE(MinimalAPI, BlueprintType, meta=(CannotImplementInterfaceInBlueprint))
class UD1Interactable : public UInterface
{
	GENERATED_BODY()
};

class ID1Interactable
{
	GENERATED_BODY()

public:
	virtual FD1InteractionInfo GetPreInteractionInfo(const FD1InteractionQuery& InteractionQuery) const { return FD1InteractionInfo(); }
	
	virtual void GatherPostInteractionInfos(const FD1InteractionQuery& InteractionQuery, FD1InteractionInfoBuilder& InteractionInfoBuilder) const
	{
		FD1InteractionInfo InteractionInfo = GetPreInteractionInfo(InteractionQuery);
	
		if (UAbilitySystemComponent* AbilitySystem = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InteractionQuery.RequestingAvatar.Get()))
		{
			float Resourcefulness = AbilitySystem->GetNumericAttribute(UD1CombatSet::GetResourcefulnessAttribute());
			InteractionInfo.Duration = FMath::Max<float>(0.f, InteractionInfo.Duration - Resourcefulness * 0.01f);
		}
	
		InteractionInfoBuilder.AddInteractionInfo(InteractionInfo);
	}
	
	virtual void CustomizeInteractionEventData(const FGameplayTag& InteractionEventTag, FGameplayEventData& InOutEventData) const { }
	
	UFUNCTION(BlueprintCallable)
	virtual void GetMeshComponents(TArray<UMeshComponent*>& OutMeshComponents) const { }
	
	UFUNCTION(BlueprintCallable)
	virtual bool CanInteraction(const FD1InteractionQuery& InteractionQuery) const { return true; }
};
