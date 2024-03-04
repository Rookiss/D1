#pragma once

#include "D1Define.h"
#include "Data/D1AbilitySystemData.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "D1EquipManagerComponent.generated.h"

class AD1Player;
class AD1WeaponBase;
class UD1ItemInstance;
class AD1PlayerController;
class UD1EquipManagerComponent;
class UD1EquipmentManagerComponent;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnEquipEntryChanged, EEquipmentSlotType, UD1ItemInstance*);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnWeaponEquipStateChanged, EWeaponEquipState);

USTRUCT(BlueprintType)
struct FD1EquipEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()

private:
	void Init(UD1ItemInstance* NewItemInstance);
	
	void Equip();
	void Unequip();

public:
	UD1ItemInstance* GetItemInstance() const { return ItemInstance; }
	AD1WeaponBase* GetSpawnedWeaponActor() const { return SpawnedWeaponActor; }
	
private:
	friend struct FD1EquipList;
	friend class UD1EquipManagerComponent;

	UPROPERTY()
	TObjectPtr<UD1ItemInstance> ItemInstance;

private:
	UPROPERTY(NotReplicated)
	TObjectPtr<AD1WeaponBase> SpawnedWeaponActor;
	
	UPROPERTY(NotReplicated)
	FD1AbilitySystemData_GrantedHandles GrantedHandles;

	UPROPERTY(NotReplicated)
	TArray<FActiveGameplayEffectHandle> StatHandles;

private:
	UPROPERTY(NotReplicated)
	TObjectPtr<UD1EquipManagerComponent> EquipManager;
	
	UPROPERTY(NotReplicated)
	EEquipmentSlotType EquipmentSlotType = EEquipmentSlotType::Count;
};

USTRUCT(BlueprintType)
struct FD1EquipList : public FFastArraySerializer
{
	GENERATED_BODY()

public:
	FD1EquipList() : EquipManager(nullptr) { }
	FD1EquipList(UD1EquipManagerComponent* InOwnerComponent) : EquipManager(InOwnerComponent) { }

public:
	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParams);
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);

private:
	void Equip(EEquipmentSlotType EquipmentSlotType, UD1ItemInstance* ItemInstance);
	void Unequip(EEquipmentSlotType EquipmentSlotType);
	
public:
	const TArray<FD1EquipEntry>& GetAllEntries() const { return Entries; }
	
private:
	friend class UD1EquipManagerComponent;

	UPROPERTY()
	TArray<FD1EquipEntry> Entries;
	
	UPROPERTY(NotReplicated)
	TObjectPtr<UD1EquipManagerComponent> EquipManager;
};

template<>
struct TStructOpsTypeTraits<FD1EquipList> : public TStructOpsTypeTraitsBase2<FD1EquipList>
{
	enum
	{
		WithNetDeltaSerializer = true
	};
};

UCLASS(BlueprintType)
class UD1EquipManagerComponent : public UActorComponent
{
	GENERATED_BODY()
	
public:
	UD1EquipManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	
public:
	void Equip(EEquipmentSlotType EquipmentSlotType, UD1ItemInstance* ItemInstance);
	void Unequip(EEquipmentSlotType EquipmentSlotType);

public:
	void EquipWeaponInSlot();
	void UnequipWeaponInSlot();

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void ChangeWeaponEquipState(EWeaponEquipState NewWeaponEquipState);
	bool CanChangeWeaponEquipState(EWeaponEquipState NewWeaponEquipState) const;
	
	EWeaponEquipState GetCurrentWeaponEquipState() const { return CurrentWeaponEquipState; }
	EWeaponEquipState GetBackwardWeaponEquipState() const;
	EWeaponEquipState GetForwardWeaponEquipState() const;

private:
	UFUNCTION()
	void OnRep_CurrentWeaponEquipState();
	
public:
	AD1Player* GetPlayerCharacter() const;
	AD1PlayerController* GetPlayerController() const;
	const TArray<FD1EquipEntry>& GetAllEntries() const;
	UD1AbilitySystemComponent* GetAbilitySystemComponent() const;
	UD1EquipmentManagerComponent* GetEquipmentManagerComponent() const;
	
public:
	FOnEquipEntryChanged OnEquipEntryChanged;
	FOnWeaponEquipStateChanged OnWeaponEquipStateChanged;
	
private:
	UPROPERTY(Replicated)
	FD1EquipList EquipList;

	UPROPERTY(ReplicatedUsing=OnRep_CurrentWeaponEquipState)
	EWeaponEquipState CurrentWeaponEquipState = EWeaponEquipState::Unarmed;
};
