#pragma once

#include "D1InteractionInfo.h"
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
	virtual void GatherPostInteractionInfos(const FD1InteractionQuery& InteractionQuery, FD1InteractionInfoBuilder& InteractionInfoBuilder) const abstract;
	virtual void CustomizeInteractionEventData(const FGameplayTag& InteractionEventTag, FGameplayEventData& InOutEventData) const { }

protected:
	UFUNCTION(BlueprintCallable)
	virtual FD1InteractionInfo GetPreInteractionInfo(const FD1InteractionQuery& InteractionQuery) const abstract;
};
