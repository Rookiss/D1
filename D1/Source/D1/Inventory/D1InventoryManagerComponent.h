#pragma once

#include "Net/Serialization/FastArraySerializer.h"
#include "D1InventoryManagerComponent.generated.h"

class UD1ItemInstance;

DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnInventoryEntryChanged, const FIntPoint&/*ItemSlotPos*/, UD1ItemInstance*/*ItemInstance*/, int32/*ItemCount*/);

USTRUCT(BlueprintType)
struct FD1InventoryEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()

public:
	FD1InventoryEntry() { }

public:
	UD1ItemInstance* GetItemInstance() const { return ItemInstance; }
	int32 GetItemCount() const { return ItemCount; }
	FString GetDebugString() const;

private:
	friend struct FD1InventoryList;
	friend class UD1InventoryManagerComponent;
	
	UPROPERTY()
	TObjectPtr<UD1ItemInstance> ItemInstance = nullptr;

	UPROPERTY()
	int32 ItemCount = 0;

	UPROPERTY()
	int32 CachedItemID = 0;
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
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);

private:
	bool TryAddItem(const FIntPoint& ToItemSlotPos, UD1ItemInstance* FromItemInstance, int32 FromItemCount = 1);
	bool TryAddItem(int32 ItemID, int32 ItemCount = 1);
	bool TryAddItem(UD1ItemInstance* ItemInstance, int32 ItemCount = 1);

	bool TryRemoveItem(const FIntPoint& ItemSlotPos, int32 ItemCount = 1);

public:
	FD1InventoryEntry GetItemByPosition(const FIntPoint& ItemSlotPos);
	int32 GetTotalCountByID(int32 ItemID);
	
	const TArray<FD1InventoryEntry>& GetAllEntries() const { return Entries; }
	
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
	virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;

public:
	void Server_Init();
	
	bool TryAddItemByPosition(const FIntPoint& ItemSlotPos, UD1ItemInstance* ItemInstance, int32 ItemCount = 1);
	bool TryAddItemByID(int32 ItemID, int32 ItemCount = 1);
	bool TryAddItemByInstance(UD1ItemInstance* ItemInstance, int32 ItemCount = 1);
	
	bool TryRemoveItemByPosition(const FIntPoint& ItemSlotPos, int32 ItemCount = 1);
	bool TryRemoveItemByID(int32 ItemID, int32 ItemCount = 1);

	UFUNCTION(Server, Reliable)
	void RequestMoveOrMergeItem(const FIntPoint& FromSlotPos, const FIntPoint& ToSlotPos);
	
	void MarkSlotChecks(bool bIsUsing, const FIntPoint& ItemSlotPos, const FIntPoint& ItemSlotCount);
	
public:
	bool IsEmpty(const FIntPoint& ItemSlotPos, const FIntPoint& ItemSlotCount) const;
	bool CanMoveItem(const FIntPoint& FromSlotPos, const FIntPoint& ToSlotPos) const;
	bool CanMergeItem(const FIntPoint& FromSlotPos, const FIntPoint& ToSlotPos, int32& MergeCount) const;

	FIntPoint GetInventorySlotCount() const { return InventorySlotCount; }
	const TArray<FD1InventoryEntry>& GetAllEntries() const;
	FD1InventoryEntry GetItemByPosition(const FIntPoint& ItemSlotPos);
	TArray<TArray<bool>>& GetSlotChecks() { return InventorySlotChecks; }

public:
	FOnInventoryEntryChanged OnInventoryEntryChanged;
	
private:
	UPROPERTY(Replicated)
	FD1InventoryList InventoryList;

	UPROPERTY(EditDefaultsOnly)
	FIntPoint InventorySlotCount = FIntPoint(10, 5);
	
	TArray<TArray<bool>> InventorySlotChecks;
};
