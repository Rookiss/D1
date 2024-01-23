#pragma once

#include "Data/D1AbilitySystemData.h"
#include "Item/Fragments/D1ItemFragment_Equippable.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "D1EquipmentManagerComponent.generated.h"

class UD1ItemInstance;
class UD1AbilitySystemComponent;
class UD1EquipmentManagerComponent;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnEquipmentEntryChanged, EEquipmentType, UD1ItemInstance*)

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
	TObjectPtr<UD1ItemInstance> ItemInstance = nullptr;

	UPROPERTY()
	int32 LatestValidItemID = 0;

	UPROPERTY()
	TArray<TObjectPtr<AActor>> SpawnedActor;
	
	UPROPERTY(NotReplicated)
	FD1AbilitySystemData_GrantedHandles GrantedHandles;
	
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
	
	void Unsafe_EquipItem(UD1ItemInstance* ItemInstance);
	void Unsafe_UnEquipItem(EEquipmentType EquipmentType);

private:
	bool CanEquipItem(UD1ItemInstance* ItemInstance) const;
	bool CanUnEquipItem(EEquipmentType EquipmentType) const;

public:
	FD1EquipmentEntry GetEntryByType(EEquipmentType EquipmentType) const;
	const TArray<FD1EquipmentEntry>& GetAllEntries() const { return Entries; }
	
private:
	friend class UD1EquipmentManagerComponent;

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
	UFUNCTION(Server, Reliable)
	void Server_RequestEquipItem(UD1ItemInstance* ItemInstance);

	UFUNCTION(Server, Reliable)
	void Server_RequestUnEquipItem(EEquipmentType EquipmentType);

public:
	bool CanEquipItem(UD1ItemInstance* ItemInstance) const;
	bool CanUnEquipItem(EEquipmentType EquipmentType) const;
	
	const TArray<FD1EquipmentEntry>& GetAllEntries() const;
	FD1EquipmentEntry GetEntryByType(EEquipmentType EquipmentType);
	int32 GetEquipmentSlotCount() const { return EquipmentSlotCount; }
	UD1AbilitySystemComponent* GetAbilitySystemComponent() const;
	
public:
	FOnEquipmentEntryChanged OnEquipmentEntryChanged;
	
private:
	UPROPERTY(Replicated)
	FD1EquipmentList EquipmentList;

	UPROPERTY()
	int32 EquipmentSlotCount = static_cast<int32>(EEquipmentType::Count);
};
