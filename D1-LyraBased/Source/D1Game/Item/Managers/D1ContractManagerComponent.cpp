#include "D1ContractManagerComponent.h"

#include "D1EquipmentManagerComponent.h"
#include "D1InventoryManagerComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1ContractManagerComponent)

UD1ContractManagerComponent::UD1ContractManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

bool UD1ContractManagerComponent::InvnetoryToEquipment(UD1InventoryManagerComponent* FromInventoryManager, const FIntPoint& FromItemSlotPos, UD1EquipmentManagerComponent* ToEquipmentManager, EEquipmentSlotType ToEquipmentSlotType)
{
	if (HasAuthority() == false)
		return false;
	
	if (FromInventoryManager == nullptr || ToEquipmentManager == nullptr)
		return false;

	if (IsAllowedActor(FromInventoryManager->GetOwner()) == false || IsAllowedActor(ToEquipmentManager->GetOwner()) == false)
		return false;

	

	return true;
}

bool UD1ContractManagerComponent::EquipmentToInventory(UD1EquipmentManagerComponent* FromEquipmentManager, EEquipmentSlotType FromEquipmentSlotType, UD1InventoryManagerComponent* ToInventoryManager, const FIntPoint& ToItemSlotPos)
{
	if (HasAuthority() == false)
		return false;

	if (FromEquipmentManager == nullptr || ToInventoryManager == nullptr)
		return false;

	if (IsAllowedActor(FromEquipmentManager->GetOwner()) == false || IsAllowedActor(ToInventoryManager->GetOwner()) == false)
		return false;

	
	
	return true;
}

bool UD1ContractManagerComponent::InventoryToInventory(UD1InventoryManagerComponent* FromInventoryManager, const FIntPoint& FromItemSlotPos, UD1InventoryManagerComponent* ToInventoryManager, const FIntPoint& ToItemSlotPos, int32 ItemCount)
{
	if (HasAuthority() == false)
		return false;

	if (FromInventoryManager == nullptr || ToInventoryManager == nullptr)
		return false;

	if (IsAllowedActor(FromInventoryManager->GetOwner()) == false || IsAllowedActor(ToInventoryManager->GetOwner()) == false)
		return false;

	// TODO: Woring on this
	

	return true;
}

bool UD1ContractManagerComponent::EquipmentToEquipment(UD1EquipmentManagerComponent* FromEquipmentManager, EEquipmentSlotType FromEquipmentSlotType, UD1EquipmentManagerComponent* ToEquipmentManager, EEquipmentSlotType ToEquipmentSlotType)
{
	if (HasAuthority() == false)
		return false;

	if (FromEquipmentManager == nullptr || ToEquipmentManager == nullptr)
		return false;

	if (IsAllowedActor(FromEquipmentManager->GetOwner()) == false || IsAllowedActor(ToEquipmentManager->GetOwner()) == false)
		return false;

	

	return true;
}

void UD1ContractManagerComponent::AddAllowedActor(AActor* Actor)
{
	AllowedActors.Add(Actor);
}

void UD1ContractManagerComponent::RemoveAllowedActor(AActor* Actor)
{
	AllowedActors.Remove(Actor);
}

bool UD1ContractManagerComponent::IsAllowedActor(AActor* Actor) const
{
	return AllowedActors.Contains(TWeakObjectPtr<AActor>(Actor));
}
