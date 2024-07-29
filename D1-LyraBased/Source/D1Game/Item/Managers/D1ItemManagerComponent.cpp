#include "D1ItemManagerComponent.h"

#include "D1EquipmentManagerComponent.h"
#include "D1InventoryManagerComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1ItemManagerComponent)

UD1ItemManagerComponent::UD1ItemManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetIsReplicatedByDefault(true);
}

void UD1ItemManagerComponent::Server_InventoryToEquipment_Implementation(UD1InventoryManagerComponent* FromInventoryManager, const FIntPoint& FromItemSlotPos, UD1EquipmentManagerComponent* ToEquipmentManager, EEquipmentSlotType ToEquipmentSlotType)
{
	if (HasAuthority() == false)
		return;
	
	if (FromInventoryManager == nullptr || ToEquipmentManager == nullptr)
		return;

	if (IsAllowedComponent(FromInventoryManager) == false ||  IsAllowedComponent(ToEquipmentManager) == false)
		return;
	
	if (ToEquipmentManager->CanAddEquipment(FromInventoryManager, FromItemSlotPos, ToEquipmentSlotType))
	{
		UD1ItemInstance* RemovedItemInstance = FromInventoryManager->RemoveItem(FromItemSlotPos, 1);
		ToEquipmentManager->AddEquipment(ToEquipmentSlotType, RemovedItemInstance);
	}
}

void UD1ItemManagerComponent::Server_InventoryToEquipment_Quick_Implementation(UD1InventoryManagerComponent* FromInventoryManager, const FIntPoint& FromItemSlotPos, UD1EquipmentManagerComponent* ToEquipmentManager)
{
	if (HasAuthority() == false)
		return;
	
	if (FromInventoryManager == nullptr || ToEquipmentManager == nullptr)
		return;

	if (IsAllowedComponent(FromInventoryManager) == false ||  IsAllowedComponent(ToEquipmentManager) == false)
		return;

	EEquipmentSlotType ToEquipmentSlotType;

	// TODO: Try to Inven Add -> Equip Quick -> Equip Swap
	if (ToEquipmentManager->CanAddEquipment_Swap(FromInventoryManager, FromItemSlotPos, ToEquipmentSlotType))
	{
		UD1ItemInstance* RemovedItemInstanceTo = ToEquipmentManager->RemoveEquipment(ToEquipmentSlotType);
		UD1ItemInstance* RemovedItemInstanceFrom = FromInventoryManager->RemoveItem(FromItemSlotPos, 1);
		ToEquipmentManager->AddEquipment(ToEquipmentSlotType, RemovedItemInstanceFrom);
		FromInventoryManager->AddItem(FromItemSlotPos, RemovedItemInstanceTo, 1);
	}
	else if (ToEquipmentManager->CanAddEquipment_Quick(FromInventoryManager, FromItemSlotPos, ToEquipmentSlotType))
	{
		UD1ItemInstance* RemovedItemInstance = FromInventoryManager->RemoveItem(FromItemSlotPos, 1);
		ToEquipmentManager->AddEquipment(ToEquipmentSlotType, RemovedItemInstance);
	}
	else if (FromInventoryManager->GetOwner() != ToEquipmentManager->GetOwner())
	{
		if (UD1InventoryManagerComponent* ToInventoryManager = ToEquipmentManager->GetOwner()->GetComponentByClass<UD1InventoryManagerComponent>())
		{
			Server_InventoryToInventory_Quick(FromInventoryManager, FromItemSlotPos, ToInventoryManager);
		}
	}
}

void UD1ItemManagerComponent::Server_EquipmentToInventory_Implementation(UD1EquipmentManagerComponent* FromEquipmentManager, EEquipmentSlotType FromEquipmentSlotType, UD1InventoryManagerComponent* ToInventoryManager, const FIntPoint& ToItemSlotPos)
{
	if (HasAuthority() == false)
		return;
	
	if (FromEquipmentManager == nullptr || ToInventoryManager == nullptr)
		return;

	if (IsAllowedComponent(FromEquipmentManager) == false ||  IsAllowedComponent(ToInventoryManager) == false)
		return;

	if (ToInventoryManager->CanMoveOrMergeItem(FromEquipmentManager, FromEquipmentSlotType, ToItemSlotPos))
	{
		UD1ItemInstance* RemovedItemInstance = FromEquipmentManager->RemoveEquipment(FromEquipmentSlotType);
		ToInventoryManager->AddItem(ToItemSlotPos, RemovedItemInstance, 1);
	}
}

void UD1ItemManagerComponent::Server_EquipmentToInventory_Quick_Implementation(UD1EquipmentManagerComponent* FromEquipmentManager, EEquipmentSlotType FromEquipmentSlotType, UD1InventoryManagerComponent* ToInventoryManager)
{
	if (HasAuthority() == false)
		return;
	
	if (FromEquipmentManager == nullptr || ToInventoryManager == nullptr)
		return;

	if (IsAllowedComponent(FromEquipmentManager) == false ||  IsAllowedComponent(ToInventoryManager) == false)
		return;

	FIntPoint ToItemSlotPos;
	
	if (ToInventoryManager->CanMoveOrMergeItem_Quick(FromEquipmentManager, FromEquipmentSlotType, ToItemSlotPos))
	{
		UD1ItemInstance* RemovedItemInstance = FromEquipmentManager->RemoveEquipment(FromEquipmentSlotType);
		ToInventoryManager->AddItem(ToItemSlotPos, RemovedItemInstance, 1);
	}
}

void UD1ItemManagerComponent::Server_InventoryToInventory_Implementation(UD1InventoryManagerComponent* FromInventoryManager, const FIntPoint& FromItemSlotPos, UD1InventoryManagerComponent* ToInventoryManager, const FIntPoint& ToItemSlotPos)
{
	if (HasAuthority() == false)
		return;
	
	if (FromInventoryManager == nullptr || ToInventoryManager == nullptr)
		return;

	if (IsAllowedComponent(FromInventoryManager) == false ||  IsAllowedComponent(ToInventoryManager) == false)
		return;

	int32 MovableItemCount = ToInventoryManager->CanMoveOrMergeItem(FromInventoryManager, FromItemSlotPos, ToItemSlotPos);
	if (MovableItemCount > 0)
	{
		UD1ItemInstance* RemovedItemInstance = FromInventoryManager->RemoveItem(FromItemSlotPos, MovableItemCount);
		ToInventoryManager->AddItem(ToItemSlotPos, RemovedItemInstance, MovableItemCount);
	}
}

void UD1ItemManagerComponent::Server_InventoryToInventory_Quick_Implementation(UD1InventoryManagerComponent* FromInventoryManager, const FIntPoint& FromItemSlotPos, UD1InventoryManagerComponent* ToInventoryManager)
{
	if (HasAuthority() == false)
		return;
	
	if (FromInventoryManager == nullptr || ToInventoryManager == nullptr)
		return;

	if (IsAllowedComponent(FromInventoryManager) == false ||  IsAllowedComponent(ToInventoryManager) == false)
		return;

	TArray<FIntPoint> OutToItemSlotPoses;
	TArray<int32> OutToItemCounts;
	int32 MovableItemCount = ToInventoryManager->CanMoveOrMergeItem_Quick(FromInventoryManager, FromItemSlotPos, OutToItemSlotPoses, OutToItemCounts);
	if (MovableItemCount > 0)
	{
		UD1ItemInstance* RemovedItemInstance = FromInventoryManager->RemoveItem(FromItemSlotPos, MovableItemCount);
		for (int32 i = 0; i < OutToItemSlotPoses.Num(); i++)
		{
			ToInventoryManager->AddItem(OutToItemSlotPoses[i], RemovedItemInstance, OutToItemCounts[i]);
		}
	}
}

void UD1ItemManagerComponent::Server_EquipmentToEquipment_Implementation(UD1EquipmentManagerComponent* FromEquipmentManager, EEquipmentSlotType FromEquipmentSlotType, UD1EquipmentManagerComponent* ToEquipmentManager, EEquipmentSlotType ToEquipmentSlotType)
{
	if (HasAuthority() == false)
		return;
	
	if (FromEquipmentManager == nullptr || ToEquipmentManager == nullptr)
		return;

	if (IsAllowedComponent(FromEquipmentManager) == false ||  IsAllowedComponent(ToEquipmentManager) == false)
		return;
	
	if (ToEquipmentManager->CanAddEquipment(FromEquipmentManager, FromEquipmentSlotType, ToEquipmentSlotType))
	{
		UD1ItemInstance* RemovedItemInstance = FromEquipmentManager->RemoveEquipment(FromEquipmentSlotType);
		ToEquipmentManager->AddEquipment(ToEquipmentSlotType, RemovedItemInstance);
	}
}

void UD1ItemManagerComponent::Server_EquipmentToEquipment_Quick_Implementation(UD1EquipmentManagerComponent* FromEquipmentManager, EEquipmentSlotType FromEquipmentSlotType, UD1EquipmentManagerComponent* ToEquipmentManager)
{
	if (HasAuthority() == false)
		return;
	
	if (FromEquipmentManager == nullptr || ToEquipmentManager == nullptr)
		return;

	if (IsAllowedComponent(FromEquipmentManager) == false ||  IsAllowedComponent(ToEquipmentManager) == false)
		return;
	
	EEquipmentSlotType ToEquipmentSlotType;
	if (ToEquipmentManager->CanAddEquipment_Quick(FromEquipmentManager, FromEquipmentSlotType, ToEquipmentSlotType))
	{
		UD1ItemInstance* RemovedItemInstance = FromEquipmentManager->RemoveEquipment(FromEquipmentSlotType);
		ToEquipmentManager->AddEquipment(ToEquipmentSlotType, RemovedItemInstance);
	}
	else
	{
		if (UD1InventoryManagerComponent* ToInventoryManager = ToEquipmentManager->GetOwner()->GetComponentByClass<UD1InventoryManagerComponent>())
		{
			Server_EquipmentToInventory_Quick(FromEquipmentManager, FromEquipmentSlotType, ToInventoryManager);
		}
	}
}

void UD1ItemManagerComponent::AddAllowedComponent(UActorComponent* ActorComponent)
{
	AllowedComponents.Add(ActorComponent);
}

void UD1ItemManagerComponent::RemoveAllowedComponent(UActorComponent* ActorComponent)
{
	AllowedComponents.Remove(ActorComponent);
}

bool UD1ItemManagerComponent::IsAllowedComponent(UActorComponent* ActorComponent) const
{
	return AllowedComponents.Contains(ActorComponent);
}
