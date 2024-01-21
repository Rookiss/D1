#pragma once

#include "Net/Serialization/FastArraySerializer.h"
#include "D1InventoryManagerComponent.generated.h"

class UD1InventoryManagerComponent;
class UD1ItemInstance;

DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnInventoryEntryChanged, const FIntPoint&/*ItemSlotPos*/, UD1ItemInstance*/*ItemInstance*/, int32/*ItemCount*/);

UENUM(BlueprintType)
enum class EInventoryOpResult : uint8
{
	Fail,
	Move,
	Merge
};

USTRUCT(BlueprintType)
struct FD1InventoryEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()

public:
	FD1InventoryEntry() { }

private:
	void Init(int32 InItemID, int32 InItemCount);
	void Init(UD1ItemInstance* InItemInstance, int32 InItemCount);
	
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
	int32 LatestValidItemID = 0;
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
	void ServerInit();

	bool TryAddItem(const FIntPoint& ItemSlotPos, int32 ItemID, int32 ItemCount);
	bool TryAddItem(int32 ItemID, int32 ItemCount);
	bool TryRemoveItem(const FIntPoint& ItemSlotPos, int32 ItemCount);
	bool TryRemoveItem(int32 ItemID, int32 ItemCount);
	
	void Unsafe_MoveItem(const FIntPoint& FromSlotPos, const FIntPoint& ToSlotPos);
	void Unsafe_MergeItem(const FIntPoint& FromSlotPos, const FIntPoint& ToSlotPos, int32 MergeCount);
	
public:
	FD1InventoryEntry GetEntryByPosition(const FIntPoint& ItemSlotPos);
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
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;

public:
	UFUNCTION(Server, Reliable)
	void RequestMoveOrMergeItem(const FIntPoint& FromSlotPos, const FIntPoint& ToSlotPos);

	bool TryAddItem(const FIntPoint& ItemSlotPos, int32 ItemID, int32 ItemCount);
	bool TryAddItem(int32 ItemID, int32 ItemCount);
	bool TryRemoveItem(const FIntPoint& ItemSlotPos, int32 ItemCount);
	bool TryRemoveItem(int32 ItemID, int32 ItemCount);
	
	void MarkSlotChecks(TArray<TArray<bool>>& SlotChecks, bool bIsUsing, const FIntPoint& ItemSlotPos, const FIntPoint& ItemSlotCount);
	void MarkSlotChecks(bool bIsUsing, const FIntPoint& ItemSlotPos, const FIntPoint& ItemSlotCount);
	
public:
	bool IsEmpty(const TArray<TArray<bool>>& SlotChecks, const FIntPoint& ItemSlotPos, const FIntPoint& ItemSlotCount) const;
	bool IsEmpty(const FIntPoint& ItemSlotPos, const FIntPoint& ItemSlotCount) const;
	EInventoryOpResult CanMoveOrMergeItem(const FIntPoint& FromSlotPos, const FIntPoint& ToSlotPos, int32& OutMergeCount) const;

	const TArray<FD1InventoryEntry>& GetAllEntries() const;
	FD1InventoryEntry GetEntryByPosition(const FIntPoint& ItemSlotPos);
	FIntPoint GetInventorySlotCount() const { return InventorySlotCount; }
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
