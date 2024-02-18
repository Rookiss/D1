#pragma once

#include "D1Define.h"
#include "Data/D1AbilitySystemData.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "D1EquipmentManagerComponent.generated.h"

class UD1ItemInstance;
class UD1AbilitySystemComponent;
class UD1EquipmentManagerComponent;
class UD1InventoryManagerComponent;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnEquipmentEntryChanged, EEquipmentSlotType, UD1ItemInstance*)

USTRUCT(BlueprintType)
struct FD1EquipmentEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()
	
private:
	void Init(UD1ItemInstance* InItemInstance);
	void Reset();
	
public:
	UD1ItemInstance* GetItemInstance() const { return ItemInstance; }
	FString GetDebugString() const;
	
private:
	friend struct FD1EquipmentList;
	friend class UD1EquipmentManagerComponent;

	UPROPERTY()
	TObjectPtr<UD1ItemInstance> ItemInstance;

	UPROPERTY()
	int32 LastValidItemID = 0;

private:
	UPROPERTY(NotReplicated)
	TArray<TObjectPtr<AActor>> SpawnedActor;
	
	UPROPERTY(NotReplicated)
	FD1AbilitySystemData_GrantedHandles GrantedHandles;

	UPROPERTY(NotReplicated)
	TArray<FActiveGameplayEffectHandle> StatHandles;

private:
	UPROPERTY(NotReplicated)
	TObjectPtr<UD1EquipmentManagerComponent> OwnerComponent;
};

USTRUCT(BlueprintType)
struct FD1EquipmentList : public FFastArraySerializer
{
	GENERATED_BODY()

public:
	FD1EquipmentList() : OwnerComponent(nullptr) { }
	FD1EquipmentList(UD1EquipmentManagerComponent* InOwnerComponent) : OwnerComponent(InOwnerComponent) { }

public:
	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParams);
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);

private:
	void InitOnServer();
	
	void EquipItem_Unsafe(EEquipmentSlotType EquipmentSlotType, UD1ItemInstance* ItemInstance);
	UD1ItemInstance* UnequipItem_Unsafe(EEquipmentSlotType EquipmentSlotType); /* Return Unequipped ItemInstance */

public:
	const TArray<FD1EquipmentEntry>& GetAllEntries() const { return Entries; }
	FD1EquipmentEntry GetEntryBySlotType(EEquipmentSlotType EquipmentSlotType) const;
	
private:
	friend class UD1EquipmentManagerComponent;
	friend class UD1InventoryManagerComponent;

	UPROPERTY()
	TArray<FD1EquipmentEntry> Entries;
	
	UPROPERTY(NotReplicated)
	TObjectPtr<UD1EquipmentManagerComponent> OwnerComponent;
};

template<>
struct TStructOpsTypeTraits<FD1EquipmentList> : public TStructOpsTypeTraitsBase2<FD1EquipmentList>
{
	enum
	{
		WithNetDeltaSerializer = true
	};
};

UCLASS(BlueprintType)
class UD1EquipmentManagerComponent : public UActorComponent
{
	GENERATED_BODY()
	
public:
	UD1EquipmentManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void BeginPlay() override;
	virtual void UninitializeComponent() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;

public:
	// TODO: Auth Check
	UFUNCTION(Server, Reliable)
	void Server_RequestEquipItem_FromInventory(UD1InventoryManagerComponent* OtherComponent, const FIntPoint& FromItemSlotPos, EEquipmentSlotType ToEquipmentSlotType);
	bool CanEquipItem_FromInventory(UD1InventoryManagerComponent* OtherComponent, const FIntPoint& FromItemSlotPos, EEquipmentSlotType ToEquipmentSlotType) const;

	UFUNCTION(Server, Reliable)
	void Server_RequestEquipItem_FromEquipment(UD1EquipmentManagerComponent* OtherComponent, EEquipmentSlotType FromEquipmentSlotType, EEquipmentSlotType ToEquipmentSlotType);
	bool CanEquipItem_FromEquipment(UD1EquipmentManagerComponent* OtherComponent, EEquipmentSlotType FromEquipmentSlotType, EEquipmentSlotType ToEquipmentSlotType) const;
	
	bool CanEquipItem(UD1ItemInstance* FromItemInstance, EEquipmentSlotType ToEquipmentSlotType) const;

public:
	bool IsSameWeaponHandType(EEquipmentSlotType EquipmentSlotType, EWeaponHandType WeaponHandType) const;
	bool IsSameArmorType(EEquipmentSlotType EquipmentSlotType, EArmorType ArmorType) const;

	bool IsPrimaryWeaponSlot(EEquipmentSlotType EquipmentSlotType) const;
	bool IsSecondaryWeaponSlot(EEquipmentSlotType EquipmentSlotType) const;
	
	const TArray<FD1EquipmentEntry>& GetAllEntries() const;
	int32 GetEquipmentSlotCount() const { return EquipmentSlotCount; }
	UD1AbilitySystemComponent* GetAbilitySystemComponent() const;

public:
	FOnEquipmentEntryChanged OnEquipmentEntryChanged;
	
private:
	friend class UD1InventoryManagerComponent;
	
	UPROPERTY(Replicated)
	FD1EquipmentList EquipmentList;
};
