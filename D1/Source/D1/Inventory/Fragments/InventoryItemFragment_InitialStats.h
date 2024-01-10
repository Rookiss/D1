#pragma once

#include "GameplayTagContainer.h"
#include "InventoryItemFragment.h"
#include "InventoryItemFragment_InitialStats.generated.h"

UCLASS(BlueprintType)
class UInventoryItemFragment_InitialStats : public UInventoryItemFragment
{
	GENERATED_BODY()
	
public:
	UInventoryItemFragment_InitialStats(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	virtual void OnInstanceCreated(UD1InventoryItemInstance* Instance) const override;

public:
	int32 GetItemStatByTag(const FGameplayTag& StatTag) const;
	
protected:
	UPROPERTY(EditDefaultsOnly)
	TMap<FGameplayTag, int32> InitialItemStats;
};
