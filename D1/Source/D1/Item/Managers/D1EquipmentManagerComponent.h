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
	void Init(UD1ItemInstance* NewItemInstance);
	
	void Equip();
	void Unequip();
	
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
	TObjectPtr<AActor> SpawnedWeaponActor;
	
	UPROPERTY(NotReplicated)
	FD1AbilitySystemData_GrantedHandles GrantedHandles;

	UPROPERTY(NotReplicated)
	TArray<FActiveGameplayEffectHandle> StatHandles;

private:
	UPROPERTY(NotReplicated)
	TObjectPtr<UD1EquipmentManagerComponent> EquipmentManager;

	UPROPERTY(NotReplicated)
	EEquipmentSlotType EquipmentSlotType = EEquipmentSlotType::Count;
};

USTRUCT(BlueprintType)
struct FD1EquipmentList : public FFastArraySerializer
{
	GENERATED_BODY()

public:
	FD1EquipmentList() : EquipmentManager(nullptr) { }
	FD1EquipmentList(UD1EquipmentManagerComponent* InOwnerComponent) : EquipmentManager(InOwnerComponent) { }
	
public:
	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParams);
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);

private:
	void InitOnServer();

	void SetEntry(EEquipmentSlotType EquipmentSlotType, UD1ItemInstance* ItemInstance);
	UD1ItemInstance* ResetEntry(EEquipmentSlotType EquipmentSlotType); /* Return Prev ItemInstance */
	
public:
	const TArray<FD1EquipmentEntry>& GetAllEntries() const { return Entries; }
	
private:
	friend class UD1EquipmentManagerComponent;
	friend class UD1InventoryManagerComponent;

	UPROPERTY()
	TArray<FD1EquipmentEntry> Entries;
	
	UPROPERTY(NotReplicated)
	TObjectPtr<UD1EquipmentManagerComponent> EquipmentManager;
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
	// TODO: With Validation
	UFUNCTION(Server, Reliable)
	void Server_RequestSetEntry_FromInventory(UD1InventoryManagerComponent* OtherComponent, const FIntPoint& FromItemSlotPos, EEquipmentSlotType ToEquipmentSlotType);
	bool CanSetEntry_FromInventory(UD1InventoryManagerComponent* OtherComponent, const FIntPoint& FromItemSlotPos, EEquipmentSlotType ToEquipmentSlotType) const;

	UFUNCTION(Server, Reliable)
	void Server_RequestSetEntry_FromEquipment(UD1EquipmentManagerComponent* OtherComponent, EEquipmentSlotType FromEquipmentSlotType, EEquipmentSlotType ToEquipmentSlotType);
	bool CanSetEntry_FromEquipment(UD1EquipmentManagerComponent* OtherComponent, EEquipmentSlotType FromEquipmentSlotType, EEquipmentSlotType ToEquipmentSlotType) const;
	
	bool CanSetEntry(UD1ItemInstance* FromItemInstance, EEquipmentSlotType ToEquipmentSlotType) const;

public:
	void EquipWeaponInSlot();
	void UnequipWeaponInSlot();

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void ChangeWeaponEquipState(EWeaponEquipState NewWeaponEquipState);
	bool CanChangeWeaponEquipState(EWeaponEquipState NewWeaponEquipState) const;

	UFUNCTION()
	void OnRep_CurrentWeaponEquipState();
	
public:
	bool IsSameWeaponEquipState(EEquipmentSlotType EquipmentSlotType, EWeaponEquipState WeaponEquipState) const;
	bool IsSameWeaponHandType(EEquipmentSlotType EquipmentSlotType, EWeaponHandType WeaponHandType) const;
	bool IsSameArmorType(EEquipmentSlotType EquipmentSlotType, EArmorType ArmorType) const;
	
	bool IsPrimaryWeaponSlot(EEquipmentSlotType EquipmentSlotType) const;
	bool IsSecondaryWeaponSlot(EEquipmentSlotType EquipmentSlotType) const;
	bool IsAllEmpty(EWeaponEquipState WeaponEquipState) const;
	
	EWeaponEquipState GetCurrentWeaponEquipState() const { return CurrentWeaponEquipState; }
	EWeaponEquipState GetBackwardWeaponEquipState() const;
	EWeaponEquipState GetForwardWeaponEquipState() const;
	
	const TArray<FD1EquipmentEntry>& GetAllEntries() const;
	UD1AbilitySystemComponent* GetAbilitySystemComponent() const;

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayMontage(FSoftObjectPath MontagePath);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SetAnimInstance(TSubclassOf<UAnimInstance> AnimInstanceClass);

	void Refresh();
	
public:
	FOnEquipmentEntryChanged OnEquipmentEntryChanged;
	
private:
	friend class UD1InventoryManagerComponent;
	
	UPROPERTY(Replicated)
	FD1EquipmentList EquipmentList;
	
	UPROPERTY(ReplicatedUsing=OnRep_CurrentWeaponEquipState)
	EWeaponEquipState CurrentWeaponEquipState = EWeaponEquipState::Unarmed;
};
