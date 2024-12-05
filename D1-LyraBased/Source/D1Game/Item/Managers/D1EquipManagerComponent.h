#pragma once

#include "D1Define.h"
#include "AbilitySystem/LyraAbilitySet.h"
#include "Components/PawnComponent.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "D1EquipManagerComponent.generated.h"

class AD1PocketWorldAttachment;
class ALyraCharacter;
class ALyraPlayerController;
class AD1EquipmentBase;
class UD1ItemInstance;
class UD1EquipManagerComponent;
class UD1EquipmentManagerComponent;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnEquipStateChanged, EEquipState, EEquipState);

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

	void SetEquipmentActor(AD1EquipmentBase* InEquipmentActor) { SpawnedEquipmentActor = InEquipmentActor; }
	AD1EquipmentBase* GetEquipmentActor() const { return SpawnedEquipmentActor; }
	
private:
	friend struct FD1EquipList;
	friend class UD1EquipManagerComponent;
	
	UPROPERTY()
	TObjectPtr<UD1ItemInstance> ItemInstance;

private:
	UPROPERTY(NotReplicated)
	EEquipmentSlotType EquipmentSlotType = EEquipmentSlotType::Count;

private:
	UPROPERTY(NotReplicated)
	TObjectPtr<AD1EquipmentBase> SpawnedEquipmentActor;

	UPROPERTY(NotReplicated)
	TObjectPtr<AD1EquipmentBase> SpawnedPocketWorldActor;

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

	void EquipCurrentSlots();
	void UnequipCurrentSlots();
	
public:
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void ChangeEquipState(EEquipState NewEquipState);

	UFUNCTION(BlueprintCallable)
	bool CanChangeEquipState(EEquipState NewEquipState) const;

private:
	UFUNCTION()
	void OnRep_CurrentEquipState(EEquipState PrevEquipState);

	void BroadcastChangedMessage(EEquipState PrevEquipState, EEquipState NewEquipState);

public:
	ALyraCharacter* GetCharacter() const;
	ALyraPlayerController* GetPlayerController() const;
	
	TArray<FD1EquipEntry>& GetAllEntries();
	UAbilitySystemComponent* GetAbilitySystemComponent() const;
	UD1EquipmentManagerComponent* GetEquipmentManager() const;

	static EEquipmentSlotType ConvertToEquipmentSlotType(EWeaponHandType WeaponHandType, EEquipState EquipState);
	static EEquipmentSlotType ConvertToEquipmentSlotType(EWeaponHandType WeaponHandType, EWeaponSlotType WeaponSlotType);
	static EEquipmentSlotType ConvertToEquipmentSlotType(EArmorType ArmorType);
	static EEquipmentSlotType ConvertToEquipmentSlotType(EUtilitySlotType UtilitySlotType);
	
	static EWeaponSlotType ConvertToWeaponSlotType(EEquipmentSlotType EquipmentSlotType);
	static EWeaponHandType ConvertToWeaponHandType(EEquipmentSlotType EquipmentSlotType);
	static EArmorType ConvertToArmorType(EEquipmentSlotType EquipmentSlotType);
	static EUtilitySlotType ConvertToUtilitySlotType(EEquipmentSlotType EquipmentSlotType);
	static EEquipState ConvertToEquipState(EWeaponSlotType WeaponSlotType);
	static EEquipState ConvertToEquipState(EUtilitySlotType UtilitySlotType);

	static bool IsWeaponEquipState(EEquipState EquipState);
	static bool IsUtilityEquipState(EEquipState EquipState);

	static const TArray<EEquipmentSlotType>& GetEquipmentSlotsByEquipState(EEquipState EquipState);
	
	UFUNCTION(BlueprintCallable)
	void ChangeShouldHiddenEquipments(bool bNewShouldHiddenEquipments);
	
	bool ShouldHiddenEquipments() const { return bShouldHiddenEquipments; }
	EEquipState GetCurrentEquipState() const { return CurrentEquipState; }
	
	AD1EquipmentBase* GetFirstEquippedActor() const;
	AD1EquipmentBase* GetEquippedActor(EWeaponHandType WeaponHandType) const;
	void GetAllEquippedActors(TArray<AD1EquipmentBase*>& OutActors) const;

	UD1ItemInstance* GetFirstEquippedItemInstance(bool bIgnoreArmor = true) const;
	UD1ItemInstance* GetEquippedItemInstance(EArmorType ArmorType) const;
	UD1ItemInstance* GetEquippedItemInstance(EWeaponHandType WeaponHandType) const;
	UD1ItemInstance* GetEquippedItemInstance(EEquipmentSlotType EquipmentSlotType) const;

public:
	FOnEquipStateChanged OnEquipStateChanged;
	
private:
	UPROPERTY(Replicated)
	FD1EquipList EquipList;

	UPROPERTY(ReplicatedUsing=OnRep_CurrentEquipState)
	EEquipState CurrentEquipState = EEquipState::Count;

	UPROPERTY(Replicated)
	bool bShouldHiddenEquipments = false;
};
