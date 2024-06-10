#pragma once

#include "D1Define.h"
#include "Components/ControllerComponent.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "D1EquipmentManagerComponent.generated.h"

class ALyraCharacter;
class ALyraPlayerController;
class UD1ItemInstance;
class UD1ItemTemplate;
class UD1EquipmentManagerComponent;
class UD1InventoryManagerComponent;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnEquipmentEntryChanged, EEquipmentSlotType, UD1ItemInstance*)

USTRUCT(BlueprintType)
struct FD1EquipmentEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()
	
private:
	void Init(UD1ItemInstance* NewItemInstance);
	
public:
	UD1ItemInstance* GetItemInstance() const { return ItemInstance; }
	
private:
	friend struct FD1EquipmentList;
	friend class UD1EquipmentManagerComponent;

	UPROPERTY()
	TObjectPtr<UD1ItemInstance> ItemInstance;

	UPROPERTY()
	int32 LastValidTemplateID = 0;

private:
	UPROPERTY(NotReplicated)
	EEquipmentSlotType EquipmentSlotType = EEquipmentSlotType::Count;
	
	UPROPERTY(NotReplicated)
	TObjectPtr<UD1EquipmentManagerComponent> EquipmentManager;
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
	void AddEquipment(EEquipmentSlotType EquipmentSlotType, UD1ItemInstance* ItemInstance);
	UD1ItemInstance* RemoveEquipment(EEquipmentSlotType EquipmentSlotType);

private:
	void BroadcastChangedMessage(EEquipmentSlotType EquipmentSlotType, UD1ItemInstance* ItemInstance);
	
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
class UD1EquipmentManagerComponent : public UControllerComponent
{
	GENERATED_BODY()
	
public:
	UD1EquipmentManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void InitializeComponent() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	virtual void ReadyForReplication() override;
	
public:
	UFUNCTION(Server, Reliable)
	void Server_AddEquipment_FromInventory(UD1InventoryManagerComponent* OtherComponent, const FIntPoint& FromItemSlotPos, EEquipmentSlotType ToEquipmentSlotType);
	bool CanAddEquipment_FromInventory(UD1InventoryManagerComponent* OtherComponent, const FIntPoint& FromItemSlotPos, EEquipmentSlotType ToEquipmentSlotType) const;

	UFUNCTION(Server, Reliable)
	void Server_AddEquipment_FromEquipment(UD1EquipmentManagerComponent* OtherComponent, EEquipmentSlotType FromEquipmentSlotType, EEquipmentSlotType ToEquipmentSlotType);
	bool CanAddEquipment_FromEquipment(UD1EquipmentManagerComponent* OtherComponent, EEquipmentSlotType FromEquipmentSlotType, EEquipmentSlotType ToEquipmentSlotType) const;

	bool CanAddEquipment(UD1ItemInstance* FromItemInstance, EEquipmentSlotType ToEquipmentSlotType) const;
	
public:
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void TryAddEquipment(EEquipmentSlotType EquipmentSlotType, TSubclassOf<UD1ItemTemplate> ItemTemplateClass, EItemRarity ItemRarity);

private:
	void AddEquipment_Unsafe(EEquipmentSlotType EquipmentSlotType, UD1ItemInstance* ItemInstance);
	UD1ItemInstance* RemoveEquipment_Unsafe(EEquipmentSlotType EquipmentSlotType);
	
public:
	static bool IsWeaponSlot(EEquipmentSlotType EquipmentSlotType);
	static bool IsArmorSlot(EEquipmentSlotType EquipmentSlotType);
	
	static bool IsSameWeaponEquipState(EEquipmentSlotType EquipmentSlotType, EWeaponEquipState WeaponEquipState);
	static bool IsSameWeaponHandType(EEquipmentSlotType EquipmentSlotType, EWeaponHandType WeaponHandType);
	static bool IsSameArmorType(EEquipmentSlotType EquipmentSlotType, EArmorType ArmorType);
	
	static bool IsPrimaryWeaponSlot(EEquipmentSlotType EquipmentSlotType);
	static bool IsSecondaryWeaponSlot(EEquipmentSlotType EquipmentSlotType);
	static bool IsTertiaryWeaponSlot(EEquipmentSlotType EquipmentSlotType);
	static bool IsQuaternaryWeaponSlot(EEquipmentSlotType EquipmentSlotType);
	
	bool IsAllEmpty(EWeaponEquipState WeaponEquipState) const;

	ALyraCharacter* GetCharacter() const;
	ALyraPlayerController* GetPlayerController() const;
	
	const TArray<FD1EquipmentEntry>& GetAllEntries() const;

public:
	FOnEquipmentEntryChanged OnEquipmentEntryChanged;
	
private:
	friend class UD1InventoryManagerComponent;
	
	UPROPERTY(Replicated)
	FD1EquipmentList EquipmentList;
};
