#pragma once

#include "D1Define.h"
#include "AbilitySystem/LyraAbilitySet.h"
#include "Components/PawnComponent.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "D1EquipManagerComponent.generated.h"

class AD1CosmeticWeapon;
class ALyraCharacter;
class ALyraPlayerController;
class AD1WeaponBase;
class UD1ItemInstance;
class UD1EquipManagerComponent;
class UD1EquipmentManagerComponent;

USTRUCT(BlueprintType)
struct FD1WeaponEquipStateChangedMessage
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	EWeaponEquipState WeaponEquipState = EWeaponEquipState::Count;
};

USTRUCT(BlueprintType)
struct FD1EquipEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()
	
private:
	void Init(UD1ItemInstance* InItemInstance);
	
	void Equip();
	void Unequip();

public:
	UD1ItemInstance* GetItemInstance() const { return ItemInstance; }
	
private:
	friend struct FD1EquipList;
	friend class UD1EquipManagerComponent;

	UPROPERTY()
	TObjectPtr<UD1ItemInstance> ItemInstance;

public:
	UPROPERTY(NotReplicated)
	EEquipmentSlotType EquipmentSlotType = EEquipmentSlotType::Count;
	
	UPROPERTY(NotReplicated)
	TObjectPtr<AD1WeaponBase> SpawnedWeaponActor;

	UPROPERTY(NotReplicated)
	TObjectPtr<AD1CosmeticWeapon> SpawnedCosmeticWeapon;

private:
	UPROPERTY(NotReplicated)
	FLyraAbilitySet_GrantedHandles BaseAbilitySetHandles;

	UPROPERTY(NotReplicated)
	FActiveGameplayEffectHandle BaseStatHandle;

public:
	UPROPERTY(NotReplicated)
	TObjectPtr<UD1EquipManagerComponent> EquipManager;
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
	TArray<FD1EquipEntry>& GetAllEntries() { return Entries; }
	
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
class UD1EquipManagerComponent : public UPawnComponent
{
	GENERATED_BODY()
	
public:
	UD1EquipManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void InitializeComponent() override;
	virtual void UninitializeComponent() override;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	virtual void ReadyForReplication() override;
	
public:
	void Equip(EEquipmentSlotType EquipmentSlotType, UD1ItemInstance* ItemInstance);
	void Unequip(EEquipmentSlotType EquipmentSlotType);

public:
	void EquipWeaponInSlot();
	void UnequipWeaponInSlot();

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void ChangeWeaponEquipState(EWeaponEquipState NewWeaponEquipState);

	UFUNCTION(BlueprintCallable)
	bool CanChangeWeaponEquipState(EWeaponEquipState NewWeaponEquipState) const;

private:
	UFUNCTION()
	void OnRep_CurrentWeaponEquipState();

	void BroadcastChangedMessage(EWeaponEquipState NewWeaponEquipState);
	
public:
	ALyraCharacter* GetCharacter() const;
	ALyraPlayerController* GetPlayerController() const;
	
	TArray<FD1EquipEntry>& GetAllEntries();
	UAbilitySystemComponent* GetAbilitySystemComponent() const;
	UD1EquipmentManagerComponent* GetEquipmentManagerComponent() const;

	static EEquipmentSlotType ConvertToEquipmentSlotType(EWeaponHandType WeaponHandType, EWeaponEquipState WeaponEquipState);
	static EEquipmentSlotType ConvertToEquipmentSlotType(EWeaponHandType WeaponHandType, EWeaponSlotType WeaponSlotType);
	static EEquipmentSlotType ConvertToEquipmentSlotType(EArmorType ArmorType);
	static EWeaponHandType ConvertToWeaponHandType(EEquipmentSlotType EquipmentSlotType);
	static EArmorType ConvertToArmorType(EEquipmentSlotType EquipmentSlotType);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	EWeaponEquipState GetCurrentWeaponEquipState() const { return CurrentWeaponEquipState; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	AD1WeaponBase* GetEquippedWeaponActor(EWeaponHandType WeaponHandType) const;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	void GetAllEquippedWeaponActors(TArray<AD1WeaponBase*>& OutEquippedWeaponActors) const;
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	UD1ItemInstance* GetEquippedWeaponItemInstance(EWeaponHandType WeaponHandType) const;
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	AD1WeaponBase* GetFirstEquippedWeapon() const;
	
private:
	UPROPERTY(Replicated)
	FD1EquipList EquipList;

	UPROPERTY(ReplicatedUsing=OnRep_CurrentWeaponEquipState)
	EWeaponEquipState CurrentWeaponEquipState = EWeaponEquipState::Count;
};
