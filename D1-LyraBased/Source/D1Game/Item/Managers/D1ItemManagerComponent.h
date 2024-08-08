#pragma once

#include "Components/ControllerComponent.h"
#include "D1ItemManagerComponent.generated.h"

class UD1ItemInstance;
class AD1PickupableItemBase;
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
	void Server_EquipmentToInventory(UD1EquipmentManagerComponent* FromEquipmentManager, EEquipmentSlotType FromEquipmentSlotType, UD1InventoryManagerComponent* ToInventoryManager, const FIntPoint& ToItemSlotPos);
	
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_InventoryToInventory(UD1InventoryManagerComponent* FromInventoryManager, const FIntPoint& FromItemSlotPos, UD1InventoryManagerComponent* ToInventoryManager, const FIntPoint& ToItemSlotPos);
	
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_EquipmentToEquipment(UD1EquipmentManagerComponent* FromEquipmentManager, EEquipmentSlotType FromEquipmentSlotType, UD1EquipmentManagerComponent* ToEquipmentManager, EEquipmentSlotType ToEquipmentSlotType);
	
public:
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_QuickFromInventory(UD1InventoryManagerComponent* FromInventoryManager, const FIntPoint& FromItemSlotPos);

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_QuickFromEquipment(UD1EquipmentManagerComponent* FromEquipmentManager, EEquipmentSlotType FromEquipmentSlotType);
	
public:
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_DropItemFromInventory(UD1InventoryManagerComponent* FromInventoryManager, const FIntPoint& FromItemSlotPos);

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_DropItemFromEquipment(UD1EquipmentManagerComponent* FromEquipmentManager, EEquipmentSlotType FromEquipmentSlotType);

public:
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	bool TryPickItem(AD1PickupableItemBase* PickupableItemActor);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	bool TryDropItem(UD1ItemInstance* FromItemInstance, int32 FromItemCount);
	
public:
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void AddAllowedComponent(UActorComponent* ActorComponent);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void RemoveAllowedComponent(UActorComponent* ActorComponent);
	
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, BlueprintPure)
	bool IsAllowedComponent(UActorComponent* ActorComponent) const;

protected:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	UD1InventoryManagerComponent* GetMyInventoryManager() const;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	UD1EquipmentManagerComponent* GetMyEquipmentManager() const;
	
private:
	UPROPERTY()
	TArray<TWeakObjectPtr<UActorComponent>> AllowedComponents;
};
