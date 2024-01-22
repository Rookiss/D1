#pragma once

#include "Data/D1AbilitySystemData.h"
#include "Item/Fragments/D1ItemFragment_Equippable.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "D1EquipmentManagerComponent.generated.h"

class UD1ItemInstance;

USTRUCT(BlueprintType)
struct FD1EquipmentEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()

private:
	void Init(UD1ItemInstance* InItemInstance);
	
public:
	UD1ItemInstance* GetItemInstance() const { return ItemInstance; }
	FString GetDebugString() const;

private:
	friend struct FD1EquipmentList;
	friend class UD1EquipmentManagerComponent;

	UPROPERTY()
	TObjectPtr<UD1ItemInstance> ItemInstance = nullptr;

	UPROPERTY(NotReplicated)
	FD1AbilitySystemData_GrantedHandles GrantedHandles;
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
	bool TryEquipItem(UD1ItemInstance* ItemInstance);
	bool TryUnEquipItem(EEquipmentType EquipmentType);
	
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

private:
	
};
