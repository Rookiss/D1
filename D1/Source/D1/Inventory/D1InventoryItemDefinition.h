#pragma once

#include "D1InventoryItemDefinition.generated.h"

class UD1InventoryItemInstance;

UCLASS(DefaultToInstanced, EditInlineNew, Abstract)
class UD1InventoryItemFragment : public UObject
{
	GENERATED_BODY()

public:
	virtual void OnInstanceCreated(UD1InventoryItemInstance* Instance) const { }
};

UCLASS(Blueprintable, Const, Abstract)
class UD1InventoryItemDefinition : public UObject
{
	GENERATED_BODY()
	
public:
	UD1InventoryItemDefinition(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	const UD1InventoryItemFragment* FindFragmentByClass(TSubclassOf<UD1InventoryItemFragment> FragmentClass) const;
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText DisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Instanced)
	TArray<TObjectPtr<UD1InventoryItemFragment>> Fragments;
};
