#pragma once

#include "Net/Serialization/FastArraySerializer.h"
#include "D1InventoryManagerComponent.generated.h"

class UD1ItemDefinition;
class UD1ItemInstance;

USTRUCT(BlueprintType)
struct FD1InventoryEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()

public:
	FD1InventoryEntry() { }

public:
	FString GetDebugString() const;

private:
	friend struct FD1InventoryList;
	friend class UD1InventoryManagerComponent;
	
	UPROPERTY()
	TObjectPtr<UD1ItemInstance> Instance = nullptr;

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
	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParams);
	
	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);
	
	UD1ItemInstance* TryAddItem(int32 ItemID, int32 StackCount = 1);
	UD1ItemInstance* TryAddItem(UD1ItemInstance* Instance, int32 StackCount = 1);
	bool TryRemoveItem(UD1ItemInstance* Instance, int32 StackCount = 1);
	
private:
	friend class UD1InventoryManagerComponent;
	
	UPROPERTY()
	TArray<FD1InventoryEntry> Entries;

	UPROPERTY()
	int32 InventorySize = 30;
	
	UPROPERTY(NotReplicated)
	TObjectPtr<UActorComponent> OwnerComponent;
};

template<>
struct TStructOpsTypeTraits<FD1InventoryList> : public TStructOpsTypeTraitsBase2<FD1InventoryList>
{
	enum
	{
		WithNetDeltaSerializer = true
	};
};

UCLASS(BlueprintType)
class UD1InventoryManagerComponent : public UActorComponent
{
	GENERATED_BODY()
	
public:
	UD1InventoryManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void ReadyForReplication() override;
	virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	
public:
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void SetInventorySize(int32 NewSize);
	
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	bool CanAddItem(int32 ItemID, int32 StackCount = 1);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	UD1ItemInstance* AddItemByDefinition(TSubclassOf<UD1ItemDefinition> ItemDef, int32 StackCount = 1);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void AddItemByInstance(UD1ItemInstance* ItemInstance);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void RemoveItemByInstance(UD1ItemInstance* ItemInstance);

public:
	UFUNCTION(BlueprintCallable)
	int32 GetInventorySize() { return InventorySize; }
	
// 	UFUNCTION(BlueprintCallable, BlueprintPure)
// 	UD1InventoryItemInstance* FindFirstItemByDefinition(TSubclassOf<UD1ItemDefinition> ItemDef) const;
//
// 	int32 GetTotalItemCountByDefinition(TSubclassOf<UD1ItemDefinition> ItemDef) const;
// 	bool ConsumeItemsByDefinition(TSubclassOf<UD1ItemDefinition> ItemDef, int32 Count);

private:
	UPROPERTY(Replicated)
	FD1InventoryList InventoryList;
};
