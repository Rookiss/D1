#pragma once

#include "D1Define.h"
#include "Components/PawnComponent.h"
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
	friend class UD1ItemManagerComponent;

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
	friend class UD1ItemManagerComponent;

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
class UD1EquipmentManagerComponent : public UPawnComponent
{
	GENERATED_BODY()
	
public:
	UD1EquipmentManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void InitializeComponent() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	virtual void ReadyForReplication() override;
	
public:
	bool CanAddEquipment(UD1InventoryManagerComponent* OtherComponent, const FIntPoint& FromItemSlotPos, EEquipmentSlotType ToEquipmentSlotType) const;
	bool CanAddEquipment(UD1EquipmentManagerComponent* OtherComponent, EEquipmentSlotType FromEquipmentSlotType, EEquipmentSlotType ToEquipmentSlotType) const;
	bool CanAddEquipment(UD1ItemInstance* FromItemInstance, EEquipmentSlotType ToEquipmentSlotType) const;
	bool CanAddEquipment_Quick(UD1InventoryManagerComponent* OtherComponent, const FIntPoint& FromItemSlotPos, EEquipmentSlotType& OutToEquipmentSlotType) const;
	bool CanAddEquipment_Quick(UD1EquipmentManagerComponent* OtherComponent, EEquipmentSlotType FromEquipmentSlotType, EEquipmentSlotType& OutToEquipmentSlotType) const;
	bool CanAddEquipment_Quick(UD1ItemInstance* FromItemInstance, EEquipmentSlotType& OutToEquipmentSlotType) const;
	bool CanAddEquipment_Swap(UD1InventoryManagerComponent* OtherComponent, const FIntPoint& FromItemSlotPos, EEquipmentSlotType& OutToEquipmentSlotType) const;
	bool CanAddEquipment_Swap(UD1EquipmentManagerComponent* OtherComponent, EEquipmentSlotType FromEquipmentSlotType, EEquipmentSlotType& OutToEquipmentSlotType) const;
	bool CanAddEquipment_Swap(UD1ItemInstance* FromItemInstance, EEquipmentSlotType& OutToEquipmentSlotType) const;
	
public:
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void TryAddEquipment(EEquipmentSlotType EquipmentSlotType, TSubclassOf<UD1ItemTemplate> ItemTemplateClass, EItemRarity ItemRarity);

private:
	void AddEquipment(EEquipmentSlotType EquipmentSlotType, UD1ItemInstance* ItemInstance);
	UD1ItemInstance* RemoveEquipment(EEquipmentSlotType EquipmentSlotType);
	
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

	UD1ItemInstance* GetItemInstance(EEquipmentSlotType EquipmentSlotType) const;
	const TArray<FD1EquipmentEntry>& GetAllEntries() const;

public:
	FOnEquipmentEntryChanged OnEquipmentEntryChanged;
	
private:
	friend class UD1ItemManagerComponent;
	friend class ULyraCheatManager;
	
	UPROPERTY(Replicated)
	FD1EquipmentList EquipmentList;
};
