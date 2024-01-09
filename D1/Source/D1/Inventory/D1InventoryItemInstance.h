#pragma once

#include "D1InventoryItemInstance.generated.h"

class UD1InventoryItemFragment;
class UD1InventoryItemDefinition;

UCLASS(BlueprintType)
class UD1InventoryItemInstance : public UObject
{
	GENERATED_BODY()
	
public:
	UD1InventoryItemInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	virtual bool IsSupportedForNetworking() const override { return true; }
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	TSubclassOf<UD1InventoryItemDefinition> GetItemDef() const { return ItemDef; }
	void SetItemDef(TSubclassOf<UD1InventoryItemDefinition> InDef) { ItemDef = InDef; }
	
	UFUNCTION(BlueprintCallable, BlueprintPure="false", meta=(DeterminesOutputType=FragmentClass))
	const UD1InventoryItemFragment* FindFragmentByClass(TSubclassOf<UD1InventoryItemFragment> FragmentClass) const;

	template <typename FragmentClass>
	const FragmentClass* FindFragmentByClass() const { return (FragmentClass*)FindFragmentByClass(FragmentClass::StaticClass()); }

private:
	UPROPERTY(Replicated)
	TSubclassOf<UD1InventoryItemDefinition> ItemDef;
};
