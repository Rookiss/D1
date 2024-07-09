#pragma once

#include "Components/ControllerComponent.h"
#include "D1ItemManagerComponent.generated.h"

class UD1EquipmentManagerComponent;
class UD1InventoryManagerComponent;

UCLASS()
class UD1ItemManagerComponent : public UControllerComponent
{
	GENERATED_BODY()
	
public:
	UD1ItemManagerComponent(const FObjectInitializer& ObjectInitializer);
	
public:
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_InventoryToEquipment(UD1InventoryManagerComponent* FromInventoryManager, const FIntPoint& FromItemSlotPos, UD1EquipmentManagerComponent* ToEquipmentManager, EEquipmentSlotType ToEquipmentSlotType);

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_InventoryToEquipment_Quick(UD1InventoryManagerComponent* FromInventoryManager, const FIntPoint& FromItemSlotPos, UD1EquipmentManagerComponent* ToEquipmentManager);
	
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_EquipmentToInventory(UD1EquipmentManagerComponent* FromEquipmentManager, EEquipmentSlotType FromEquipmentSlotType, UD1InventoryManagerComponent* ToInventoryManager, const FIntPoint& ToItemSlotPos);

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_EquipmentToInventory_Quick(UD1EquipmentManagerComponent* FromEquipmentManager, EEquipmentSlotType FromEquipmentSlotType, UD1InventoryManagerComponent* ToInventoryManager);
	
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_InventoryToInventory(UD1InventoryManagerComponent* FromInventoryManager, const FIntPoint& FromItemSlotPos, UD1InventoryManagerComponent* ToInventoryManager, const FIntPoint& ToItemSlotPos);

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_InventoryToInventory_Quick(UD1InventoryManagerComponent* FromInventoryManager, const FIntPoint& FromItemSlotPos, UD1InventoryManagerComponent* ToInventoryManager);
	
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_EquipmentToEquipment(UD1EquipmentManagerComponent* FromEquipmentManager, EEquipmentSlotType FromEquipmentSlotType, UD1EquipmentManagerComponent* ToEquipmentManager, EEquipmentSlotType ToEquipmentSlotType);

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_EquipmentToEquipment_Quick(UD1EquipmentManagerComponent* FromEquipmentManager, EEquipmentSlotType FromEquipmentSlotType, UD1EquipmentManagerComponent* ToEquipmentManager);
	
public:
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void AddAllowedComponent(UActorComponent* ActorComponent);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void RemoveAllowedComponent(UActorComponent* ActorComponent);

public:
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, BlueprintPure)
	bool IsAllowedComponent(UActorComponent* ActorComponent) const;
	
private:
	UPROPERTY()
	TArray<TWeakObjectPtr<UActorComponent>> AllowedComponents;
};
