#pragma once

#include "Components/PlayerStateComponent.h"
#include "D1ContractManagerComponent.generated.h"

class UD1EquipmentManagerComponent;
class UD1InventoryManagerComponent;

UCLASS(BlueprintType)
class UD1ContractManagerComponent : public UPlayerStateComponent
{
	GENERATED_BODY()
	
public:
	UD1ContractManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

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
	UFUNCTION(BlueprintCallable)
	bool IsAllowedActor(AActor* Actor) const;
	
private:
	UPROPERTY()
	TSet<TWeakObjectPtr<AActor>> AllowedActors;
};
