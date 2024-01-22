#include "D1EquipmentManagerComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1EquipmentManagerComponent)

void FD1EquipmentEntry::Init(UD1ItemInstance* InItemInstance)
{
}

FString FD1EquipmentEntry::GetDebugString() const
{
	return {};
}

bool FD1EquipmentList::NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParams)
{
	return false;
}

void FD1EquipmentList::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
}

void FD1EquipmentList::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
{
}

bool FD1EquipmentList::TryEquipItem(UD1ItemInstance* ItemInstance)
{
	return false;
}

bool FD1EquipmentList::TryUnEquipItem(EEquipmentType EquipmentType)
{
	return false;
}

UD1EquipmentManagerComponent::UD1EquipmentManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}
