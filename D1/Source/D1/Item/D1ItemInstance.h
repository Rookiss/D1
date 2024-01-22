#pragma once

#include "Data/D1ItemData.h"
#include "System/D1AssetManager.h"
#include "System/D1GameplayTagStack.h"
#include "D1ItemInstance.generated.h"

struct FGameplayTag;

UCLASS(BlueprintType)
class UD1ItemInstance : public UObject
{
	GENERATED_BODY()
	
public:
	UD1ItemInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	void Init(int32 InItemID);
	
	void AddStatTagStack(const FGameplayTag& StatTag, int32 StackCount);
	void RemoveStatTagStack(const FGameplayTag& StatTag, int32 StackCount);
	
public:
	virtual bool IsSupportedForNetworking() const override { return true; }

	int32 GetItemID() const { return ItemID; }
	int32 GetStackCountByTag(const FGameplayTag& StatTag) const;
	bool HasStatTag(const FGameplayTag& StatTag) const;
	FString GetDebugString() const;

	template <typename FragmentClass>
	const FragmentClass* FindFragmentByClass() const;

private:
	EItemRarity DetermineItemRarity();
	
private:
	friend struct FD1InventoryList;
	friend struct FD1InventoryEntry;
	
	UPROPERTY(Replicated)
	int32 ItemID = 0;

	UPROPERTY(Replicated)
	EItemRarity ItemRarity = EItemRarity::Junk;
	
	UPROPERTY(Replicated)
	FD1GameplayTagStackContainer StatTags;
};

template <typename FragmentClass>
const FragmentClass* UD1ItemInstance::FindFragmentByClass() const
{
	const UD1ItemData* ItemData = UD1AssetManager::GetItemData();
	const FD1ItemDefinition& ItemDef = ItemData->GetItemDefByID(ItemID);
	return ItemDef.FindFragmentByClass<FragmentClass>();
}
