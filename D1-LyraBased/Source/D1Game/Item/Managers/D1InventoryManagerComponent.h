#pragma once

#include "D1Define.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "D1InventoryManagerComponent.generated.h"

class UD1ItemTemplate;
class UD1ItemInstance;
struct FD1ItemRarityProbability;
class UD1EquipmentManagerComponent;
class UD1InventoryManagerComponent;

DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnInventoryEntryChanged, const FIntPoint&/*ItemSlotPos*/, UD1ItemInstance*/*ItemInstance*/, int32/*ItemCount*/);

USTRUCT(BlueprintType)
struct FD1InventoryEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()

private:
	UD1ItemInstance* Init(int32 InItemTemplateID, int32 InItemCount, EItemRarity InItemRarity);
	void Init(UD1ItemInstance* InItemInstance, int32 InItemCount);
	UD1ItemInstance* Reset();
	
public:
	UD1ItemInstance* GetItemInstance() const { return ItemInstance; }
	int32 GetItemCount() const { return ItemCount; }

private:
	friend struct FD1InventoryList;
	friend class UD1InventoryManagerComponent;
	
	UPROPERTY()
	TObjectPtr<UD1ItemInstance> ItemInstance;

	UPROPERTY()
	int32 ItemCount = 0;

	UPROPERTY()
	int32 LastValidTemplateID = 0;
};

USTRUCT(BlueprintType)
struct FD1InventoryList : public FFastArraySerializer
{
	GENERATED_BODY()

public:
	FD1InventoryList() : InventoryManager(nullptr) { }
	FD1InventoryList(UD1InventoryManagerComponent* InOwnerComponent) : InventoryManager(InOwnerComponent) { }

public:
	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParams);
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);
	
private:
	TArray<UD1ItemInstance*> TryAddItem(int32 ItemTemplateID, int32 ItemCount, EItemRarity ItemRarity);
	TArray<UD1ItemInstance*> TryRemoveItem(int32 ItemTemplateID, int32 ItemCount);

private:
	void BroadcastChangedMessage(const FIntPoint& ItemSlotPos, UD1ItemInstance* ItemInstance, int32 ItemCount);
	
public:
	const TArray<FD1InventoryEntry>& GetAllEntries() const { return Entries; }
	int32 GetTotalCountByID(int32 ItemTemplateID) const;
	
private:
	friend class UD1InventoryManagerComponent;
	friend class UD1EquipmentManagerComponent;
	
	UPROPERTY()
	TArray<FD1InventoryEntry> Entries;
	
	UPROPERTY(NotReplicated)
	TObjectPtr<UD1InventoryManagerComponent> InventoryManager;
};

template<>
struct TStructOpsTypeTraits<FD1InventoryList> : public TStructOpsTypeTraitsBase2<FD1InventoryList>
{
	enum
	{
		WithNetDeltaSerializer = true
	};
};

UCLASS(BlueprintType, Blueprintable)
class UD1InventoryManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UD1InventoryManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void InitializeComponent() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	virtual void ReadyForReplication() override;
	
public:
	// TODO: With Validation - Check Valid Contract
	UFUNCTION(Server, Reliable)
	void Server_RequestMoveOrMergeItem_FromInternalInventory(const FIntPoint& FromItemSlotPos, const FIntPoint& ToItemSlotPos);
	int32 CanMoveOrMergeItem_FromInternalInventory(const FIntPoint& FromItemSlotPos, const FIntPoint& ToItemSlotPos) const;

	UFUNCTION(Server, Reliable)
	void Server_RequestMoveOrMergeItem_FromExternalInventory(UD1InventoryManagerComponent* OtherComponent, const FIntPoint& FromItemSlotPos, const FIntPoint& ToItemSlotPos);
	int32 CanMoveOrMergeItem_FromExternalInventory(UD1InventoryManagerComponent* OtherComponent, const FIntPoint& FromItemSlotPos, const FIntPoint& ToItemSlotPos) const;
	
	UFUNCTION(Server, Reliable)
	void Server_RequestMoveOrMergeItem_FromExternalEquipment(UD1EquipmentManagerComponent* OtherComponent, EEquipmentSlotType FromEquipmentSlotType, const FIntPoint& ToItemSlotPos);
	bool CanMoveOrMergeItem_FromExternalEquipment(UD1EquipmentManagerComponent* OtherComponent, EEquipmentSlotType FromEquipmentSlotType, const FIntPoint& ToItemSlotPos) const;

public:
	void TryAddItem(int32 ItemTemplateID, int32 ItemCount, EItemRarity ItemRarity);
	void TryAddItem(int32 ItemTemplateID, int32 ItemCount, const TArray<FD1ItemRarityProbability>& ItemProbabilities);
	void TryRemoveItem(int32 ItemTemplateID, int32 ItemCount);

private:
	void AddItem_Unsafe(const FIntPoint& ItemSlotPos, UD1ItemInstance* ItemInstance, int32 ItemCount);
	UD1ItemInstance* RemoveItem_Unsafe(const FIntPoint& ItemSlotPos, int32 ItemCount);
	
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
	int32 GetTotalCountByID(int32 ItemTemplateID) const;

public:
	FOnInventoryEntryChanged OnInventoryEntryChanged;
	
private:
	friend class UD1EquipmentManagerComponent;
	
	UPROPERTY(Replicated)
	FD1InventoryList InventoryList;
	
	FIntPoint InventorySlotCount = FIntPoint(10, 5);
	TArray<TArray<bool>> SlotChecks;
};
