#pragma once

#include "Net/Serialization/FastArraySerializer.h"
#include "D1InventoryManagerComponent.generated.h"

class UD1InventoryItemDefinition;
class UD1InventoryItemInstance;

USTRUCT(BlueprintType)
struct FD1InventoryEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()

public:
	FD1InventoryEntry() { }

public:
	FString GetDebugString() const;

private:
	UPROPERTY()
	TObjectPtr<UD1InventoryItemInstance> Instance = nullptr;

	UPROPERTY()
	int32 StackCount = 0;
	
	UPROPERTY(NotReplicated)
	int32 LastObservedCount = INDEX_NONE;
};

USTRUCT(BlueprintType)
struct FD1InventoryList : public FFastArraySerializer
{
	GENERATED_BODY()

public:
	FD1InventoryList() : OwnerComponent(nullptr) { }
	FD1InventoryList(UActorComponent* InOwnerComponent) : OwnerComponent(InOwnerComponent) { }

public:
	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParams)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FD1InventoryEntry, FD1InventoryList>(Entries, DeltaParams, *this);
	}

	UD1InventoryItemInstance* AddEntry(TSubclassOf<UD1InventoryItemDefinition> ItemClass, int32 StackCount);
	void AddEntry(UD1InventoryItemInstance* Instance);

	void RemoveEntry(UD1InventoryItemInstance* Instance);
	
public:
	TArray<UD1InventoryItemInstance*> GetAllItems() const;
	
private:
	UPROPERTY()
	TArray<FD1InventoryEntry> Entries;

	UPROPERTY(NotReplicated)
	TObjectPtr<UActorComponent> OwnerComponent;
};

template<>
struct TStructOpsTypeTraits<FD1InventoryList> : public TStructOpsTypeTraitsBase2<FD1InventoryList>
{
	enum { WithNetDeltaSerializer = true };
};

UCLASS(BlueprintType)
class UD1InventoryManagerComponent : public UActorComponent
{
	GENERATED_BODY()
	
public:
	UD1InventoryManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	bool CanAddItem(TSubclassOf<UD1InventoryItemDefinition> ItemDef, int32 StackCount = 1);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	UD1InventoryItemInstance* AddItemByDefinition(TSubclassOf<UD1InventoryItemDefinition> ItemDef, int32 StackCount = 1);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void AddItemByInstance(UD1InventoryItemInstance* ItemInstance);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void RemoveItem(UD1InventoryItemInstance* ItemInstance);

public:
	UFUNCTION(BlueprintCallable, BlueprintPure="false")	
	TArray<UD1InventoryItemInstance*> GetAllItems() const;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	UD1InventoryItemInstance* FindFirstItemStackByDefinition(TSubclassOf<UD1InventoryItemDefinition> ItemDef) const;

	int32 GetTotalItemCountByDefinition(TSubclassOf<UD1InventoryItemDefinition> ItemDef) const;
	bool ConsumeItemsByDefinition(TSubclassOf<UD1InventoryItemDefinition> ItemDef, int32 Count);

private:
	UPROPERTY(Replicated)
	FD1InventoryList InventoryList;
};
