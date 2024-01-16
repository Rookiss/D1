#pragma once

#include "Net/Serialization/FastArraySerializer.h"
#include "D1InventoryManagerComponent.generated.h"

class UD1ItemInstance;

DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnInventoryEntryChanged, const FIntPoint&, UD1ItemInstance*, int32);

USTRUCT(BlueprintType)
struct FD1InventoryEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()

public:
	FD1InventoryEntry() { }

public:
	UD1ItemInstance* GetInstance() const { return Instance; }
	int32 GetCount() const { return Count; }
	FIntPoint GetPosition() const { return Position; }
	FString GetDebugString() const;

private:
	friend struct FD1InventoryList;
	friend class UD1InventoryManagerComponent;
	
	UPROPERTY()
	TObjectPtr<UD1ItemInstance> Instance = nullptr;

	UPROPERTY()
	int32 Count = 0;
	
	UPROPERTY()
	FIntPoint Position = FIntPoint::ZeroValue;
};

USTRUCT(BlueprintType)
struct FD1InventoryList : public FFastArraySerializer
{
	GENERATED_BODY()

public:
	FD1InventoryList() : OwnerComponent(nullptr) { }
	FD1InventoryList(UD1InventoryManagerComponent* InOwnerComponent) : OwnerComponent(InOwnerComponent) { }

public:
	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParams);
	
	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);

private:
	bool TryAddItem(int32 ItemID, int32 Count = 1);
	bool TryAddItem(UD1ItemInstance* Instance, int32 Count = 1);
	bool TryRemoveItem(int32 ItemID, int32 Count = 1);
	bool TryRemoveItem(UD1ItemInstance* Instance, int32 Count = 1);

public:
	int32 GetTotalCountByID(int32 ItemID);
	int32 GetTotalCountByInstance(UD1ItemInstance* Instance);
	const TArray<FD1InventoryEntry>& GetAllItems() const { return Entries; }
	
private:
	friend class UD1InventoryManagerComponent;
	
	UPROPERTY()
	TArray<FD1InventoryEntry> Entries;
	
	UPROPERTY(NotReplicated)
	TObjectPtr<UD1InventoryManagerComponent> OwnerComponent;
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

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void ReadyForReplication() override;
	virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	
public:
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	bool TryExpandInventorySize(const FIntPoint& NewSize);
	
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	bool TryAddItemByID(int32 ItemID, int32 Count = 1);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	bool TryAddItemByInstance(UD1ItemInstance* Instance, int32 Count = 1);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	bool TryRemoveItemByID(int32 ItemID, int32 Count = 1);
	
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	bool TryRemoveItemByInstance(UD1ItemInstance* Instance, int32 Count = 1);

public:
	void AddReplicatedSubObject(UD1ItemInstance* Instance);
	void RemoveReplicatedSubObject(UD1ItemInstance* Instance);
	
	UFUNCTION(BlueprintCallable)
	FIntPoint GetInventorySize() { return InventorySize; }
	
	UFUNCTION(BlueprintCallable, BlueprintPure="false")
	const TArray<FD1InventoryEntry>& GetAllItems() const;
	
	TArray<TArray<bool>>& GetSlotChecks() { return SlotChecks; }
	bool CanAddItemByPosition(const FIntPoint& Position, const FIntPoint& SlotSize);
	void MarkSlotChecks(bool bIsUsing, const FIntPoint& Position, const FIntPoint& SlotSize);

public:
	FOnInventoryEntryChanged OnInventoryEntryChanged;
	
private:
	UPROPERTY(Replicated)
	FD1InventoryList InventoryList;

	UPROPERTY(Replicated)
	FIntPoint InventorySize;
	
	TArray<TArray<bool>> SlotChecks;
};
