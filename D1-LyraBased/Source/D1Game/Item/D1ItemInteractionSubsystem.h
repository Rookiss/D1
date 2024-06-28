#pragma once

#include "D1ItemInteractionSubsystem.generated.h"

class UD1EquipmentManagerComponent;
class UD1InventoryManagerComponent;

UCLASS()
class UD1ItemInteractionSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
public:
	UD1ItemInteractionSubsystem();

protected:
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	
public:
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	bool InvnetoryToEquipment(UD1InventoryManagerComponent* FromInventoryManager, const FIntPoint& FromItemSlotPos, UD1EquipmentManagerComponent* ToEquipmentManager, EEquipmentSlotType ToEquipmentSlotType);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	bool EquipmentToInventory(UD1EquipmentManagerComponent* FromEquipmentManager, EEquipmentSlotType FromEquipmentSlotType, UD1InventoryManagerComponent* ToInventoryManager, const FIntPoint& ToItemSlotPos);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	bool InventoryToInventory(UD1InventoryManagerComponent* FromInventoryManager, const FIntPoint& FromItemSlotPos, UD1InventoryManagerComponent* ToInventoryManager, const FIntPoint& ToItemSlotPos, int32 ItemCount);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	bool EquipmentToEquipment(UD1EquipmentManagerComponent* FromEquipmentManager, EEquipmentSlotType FromEquipmentSlotType, UD1EquipmentManagerComponent* ToEquipmentManager, EEquipmentSlotType ToEquipmentSlotType);

public:
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void AddAllowedActor(AActor* Actor);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void RemoveAllowedActor(AActor* Actor);

public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsAllowedActor(AActor* Actor) const;
	
private:
	UPROPERTY()
	TSet<TWeakObjectPtr<AActor>> AllowedActors;
};
