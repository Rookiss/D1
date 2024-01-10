#pragma once

#include "D1InventoryItemDefinition.generated.h"

class UInventoryItemFragment;

UCLASS(Blueprintable, Const, Abstract)
class UD1InventoryItemDefinition : public UObject
{
	GENERATED_BODY()
	
public:
	UD1InventoryItemDefinition(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	const UInventoryItemFragment* FindFragmentByClass(TSubclassOf<UInventoryItemFragment> FragmentClass) const;
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText DisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Instanced)
	TArray<TObjectPtr<UInventoryItemFragment>> Fragments;
};
