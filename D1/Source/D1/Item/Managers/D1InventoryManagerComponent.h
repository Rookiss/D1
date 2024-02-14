#pragma once

#include "D1Define.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "D1InventoryManagerComponent.generated.h"

class UD1ItemInstance;
class UD1EquipmentManagerComponent;
class UD1InventoryManagerComponent;

DECLARE_MULTICAST_DELEGATE_FiveParams(FOnInventoryEntryChanged, const FIntPoint&/*ItemSlotPos*/, UD1ItemInstance*/*ItemInstance*/, int32/*ItemID*/, int32/*OldItemCount*/, int32/*NewItemCount*/);

USTRUCT(BlueprintType)
struct FD1InventoryEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()

private:
	void Init(int32 InItemID, int32 InItemCount);
	void Init(UD1ItemInstance* InItemInstance, int32 InItemCount);
	void Reset();
	
public:
	UD1ItemInstance* GetItemInstance() const { return ItemInstance; }
	int32 GetItemCount() const { return ItemCount; }
	int32 GetLastObservedCount() const { return LastObservedCount; }
	FString GetDebugString() const;

private:
	friend struct FD1InventoryList;
	friend class UD1InventoryManagerComponent;
	
	UPROPERTY()
	TObjectPtr<UD1ItemInstance> ItemInstance;

	UPROPERTY()
	int32 ItemCount = 0;

	UPROPERTY()
	int32 LatestValidItemID = 0;

	UPROPERTY(NotReplicated)
	int32 LastObservedCount = INDEX_NONE;
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
	void InitOnServer();
	
	void AddItem_Unsafe(const FIntPoint& ItemSlotPos, UD1ItemInstance* ItemInstance, int32 ItemCount);
	UD1ItemInstance* RemoveItem_Unsafe(const FIntPoint& ItemSlotPos, int32 ItemCount); /* Return Removed ItemInstance */
	
	bool TryAddItem(int32 ItemID, int32 ItemCount);
	bool TryRemoveItem(int32 ItemID, int32 ItemCount);

public:
	const TArray<FD1InventoryEntry>& GetAllEntries() const { return Entries; }
	FD1InventoryEntry GetEntryByPosition(const FIntPoint& ItemSlotPos) const;
	int32 GetTotalCountByID(int32 ItemID) const;
	
private:
	friend class UD1InventoryManagerComponent;
	friend class UD1EquipmentManagerComponent;
	
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
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;

public:
	void Init(FIntPoint InInventorySlotCount);
	
public:
	// TODO: Auth Check
	UFUNCTION(Server, Reliable)
	void Server_RequestMoveOrMergeItem_FromInternalInventory(const FIntPoint& FromItemSlotPos, const FIntPoint& ToItemSlotPos);
	int32 CanMoveOrMergeItem_FromInternalInventory(const FIntPoint& FromItemSlotPos, const FIntPoint& ToItemSlotPos) const; /* Return Movable Item Count */

	UFUNCTION(Server, Reliable)
	void Server_RequestMoveOrMergeItem_FromExternalInventory(UD1InventoryManagerComponent* OtherComponent, const FIntPoint& FromItemSlotPos, const FIntPoint& ToItemSlotPos);
	int32 CanMoveOrMergeItem_FromExternalInventory(UD1InventoryManagerComponent* OtherComponent, const FIntPoint& FromItemSlotPos, const FIntPoint& ToItemSlotPos) const; /* Return Movable Item Count */

	UFUNCTION(Server, Reliable)
	void Server_RequestMoveOrMergeItem_FromExternalEquipment(UD1EquipmentManagerComponent* OtherComponent, EEquipmentSlotType FromEquipmentSlotType, const FIntPoint& ToItemSlotPos);
	bool CanMoveOrMergeItem_FromExternalEquipment(UD1EquipmentManagerComponent* OtherComponent, EEquipmentSlotType FromEquipmentSlotType, const FIntPoint& ToItemSlotPos) const;

public:
	bool TryAddItem(int32 ItemID, int32 ItemCount);
	bool TryRemoveItem(int32 ItemID, int32 ItemCount);

public:
	void MarkSlotChecks(TArray<TArray<bool>>& InSlotChecks, bool bIsUsing, const FIntPoint& ItemSlotPos, const FIntPoint& ItemSlotCount);
	void MarkSlotChecks(bool bIsUsing, const FIntPoint& ItemSlotPos, const FIntPoint& ItemSlotCount);
	
public:
	bool IsEmpty(const TArray<TArray<bool>>& InSlotChecks, const FIntPoint& ItemSlotPos, const FIntPoint& ItemSlotCount) const;
	bool IsEmpty(const FIntPoint& ItemSlotPos, const FIntPoint& ItemSlotCount) const;

public:
	const TArray<FD1InventoryEntry>& GetAllEntries() const;
	FIntPoint GetInventorySlotCount() const { return InventorySlotCount; }
	TArray<TArray<bool>>& GetSlotChecks() { return SlotChecks; }
	int32 GetTotalCountByID(int32 ItemID) const;

public:
	FOnInventoryEntryChanged OnInventoryEntryChanged;
	
private:
	friend class UD1EquipmentManagerComponent;
	
	UPROPERTY(Replicated)
	FD1InventoryList InventoryList;
	
	FIntPoint InventorySlotCount;
	TArray<TArray<bool>> SlotChecks;
};
