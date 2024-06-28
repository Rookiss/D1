#include "D1ItemInteractionSubsystem.h"

#include "Managers/D1EquipmentManagerComponent.h"
#include "Managers/D1InventoryManagerComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1ItemInteractionSubsystem)

UD1ItemInteractionSubsystem::UD1ItemInteractionSubsystem()
{
	
}

bool UD1ItemInteractionSubsystem::InvnetoryToEquipment(UD1InventoryManagerComponent* FromInventoryManager, const FIntPoint& FromItemSlotPos, UD1EquipmentManagerComponent* ToEquipmentManager, EEquipmentSlotType ToEquipmentSlotType)
{
	if (FromInventoryManager == nullptr || ToEquipmentManager == nullptr)
		return false;

	AActor* FromOwner = FromInventoryManager->GetOwner();
	AActor* ToOwner = ToEquipmentManager->GetOwner();

	if (FromOwner == nullptr || ToOwner == nullptr)
		return false;
	
	if (FromOwner->HasAuthority() == false)
		return false;
	
	
	if (IsAllowedActor(FromOwner) == false || IsAllowedActor(ToOwner) == false)
		return false;

	

	return true;
}

bool UD1ItemInteractionSubsystem::EquipmentToInventory(UD1EquipmentManagerComponent* FromEquipmentManager, EEquipmentSlotType FromEquipmentSlotType, UD1InventoryManagerComponent* ToInventoryManager, const FIntPoint& ToItemSlotPos)
{
	// if (HasAuthority() == false)
	// 	return false;

	if (FromEquipmentManager == nullptr || ToInventoryManager == nullptr)
		return false;

	if (IsAllowedActor(FromEquipmentManager->GetOwner()) == false || IsAllowedActor(ToInventoryManager->GetOwner()) == false)
		return false;

	
	
	return true;
}

bool UD1ItemInteractionSubsystem::InventoryToInventory(UD1InventoryManagerComponent* FromInventoryManager, const FIntPoint& FromItemSlotPos, UD1InventoryManagerComponent* ToInventoryManager, const FIntPoint& ToItemSlotPos, int32 ItemCount)
{
	// if (HasAuthority() == false)
	// 	return false;

	if (FromInventoryManager == nullptr || ToInventoryManager == nullptr)
		return false;

	if (IsAllowedActor(FromInventoryManager->GetOwner()) == false || IsAllowedActor(ToInventoryManager->GetOwner()) == false)
		return false;

	// TODO: Woring on this
	

	return true;
}

bool UD1ItemInteractionSubsystem::EquipmentToEquipment(UD1EquipmentManagerComponent* FromEquipmentManager, EEquipmentSlotType FromEquipmentSlotType, UD1EquipmentManagerComponent* ToEquipmentManager, EEquipmentSlotType ToEquipmentSlotType)
{
	// if (HasAuthority() == false)
	// 	return false;

	if (FromEquipmentManager == nullptr || ToEquipmentManager == nullptr)
		return false;

	if (IsAllowedActor(FromEquipmentManager->GetOwner()) == false || IsAllowedActor(ToEquipmentManager->GetOwner()) == false)
		return false;

	

	return true;
}

bool UD1ItemInteractionSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	if (Super::ShouldCreateSubsystem(Outer) == false)
		return false;
    
	UWorld* World = Cast<UWorld>(Outer);
	TEnumAsByte<EWorldType::Type> WorldType = World->WorldType;

	bool IsValidType = WorldType != EWorldType::Editor && WorldType != EWorldType::EditorPreview && WorldType != EWorldType::None;
	return IsValidType ? World->GetNetMode() < NM_Client : false;
}

void UD1ItemInteractionSubsystem::AddAllowedActor(AActor* Actor)
{
	AllowedActors.Add(Actor);
}

void UD1ItemInteractionSubsystem::RemoveAllowedActor(AActor* Actor)
{
	AllowedActors.Remove(Actor);
}

bool UD1ItemInteractionSubsystem::IsAllowedActor(AActor* Actor) const
{
	return AllowedActors.Contains(TWeakObjectPtr<AActor>(Actor));
}
