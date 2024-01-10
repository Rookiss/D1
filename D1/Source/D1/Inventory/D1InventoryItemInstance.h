#pragma once
#include "System/GameplayTagStack.h"

#include "D1InventoryItemInstance.generated.h"

struct FGameplayTag;
class UInventoryItemFragment;
class UD1InventoryItemDefinition;

UCLASS(BlueprintType)
class UD1InventoryItemInstance : public UObject
{
	GENERATED_BODY()
	
public:
	UD1InventoryItemInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void AddStatTagStack(const FGameplayTag& StatTag, int32 StackCount);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void RemoveStatTagStack(const FGameplayTag& StatTag, int32 StackCount);
	
public:
	virtual bool IsSupportedForNetworking() const override { return true; }
	
	UFUNCTION(BlueprintCallable)
	int32 GetStackCountByTag(const FGameplayTag& StatTag) const;

	UFUNCTION(BlueprintCallable)
	bool HasStatTag(const FGameplayTag& StatTag) const;

	UFUNCTION(BlueprintCallable, BlueprintPure="false", meta=(DeterminesOutputType=FragmentClass))
	const UInventoryItemFragment* FindFragmentByClass(TSubclassOf<UInventoryItemFragment> FragmentClass) const;
	
	template <typename FragmentClass>
	const FragmentClass* FindFragmentByClass() const { return (FragmentClass*)FindFragmentByClass(FragmentClass::StaticClass()); }
	
	TSubclassOf<UD1InventoryItemDefinition> GetItemDef() const { return ItemDef; }
	void SetItemDef(TSubclassOf<UD1InventoryItemDefinition> InDef) { ItemDef = InDef; }

private:
	UPROPERTY(Replicated)
	FGameplayTagStackContainer StatTags;
	
	UPROPERTY(Replicated)
	TSubclassOf<UD1InventoryItemDefinition> ItemDef;
};
