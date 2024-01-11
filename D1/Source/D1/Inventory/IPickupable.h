#pragma once
//
// #include "IPickupable.generated.h"
//
// class UD1InventoryItemInstance;
// class UD1ItemDefinition;

// USTRUCT(BlueprintType)
// struct FPickupTemplate
// {
// 	GENERATED_BODY()
//
// public:
// 	UPROPERTY(EditAnywhere)
// 	TSubclassOf<UD1ItemDefinition> ItemDef;
// 	
// 	UPROPERTY(EditAnywhere)
// 	int32 StackCount = 1;
// };
//
// USTRUCT(BlueprintType)
// struct FPickupInstance
// {
// 	GENERATED_BODY()
//
// public:
// 	UPROPERTY(EditAnywhere, BlueprintReadOnly)
// 	TObjectPtr<UD1InventoryItemInstance> Item = nullptr;
// };
//
// USTRUCT(BlueprintType)
// struct FInventoryPickup
// {
// 	GENERATED_BODY()
//
// public:
// 	UPROPERTY(EditAnywhere, BlueprintReadOnly)
// 	TArray<FPickupInstance> Instances;
//
// 	UPROPERTY(EditAnywhere, BlueprintReadOnly)
// 	TArray<FPickupTemplate> Templates;
// };
//
// UINTERFACE(MinimalAPI, BlueprintType, meta=(CannotImplementInterfaceInBlueprint))
// class UPickupable : public UInterface
// {
// 	GENERATED_BODY()
// };
//
// class IPickupable
// {
// 	GENERATED_BODY()
//
// public:
// 	UFUNCTION(BlueprintCallable)
// 	virtual FInventoryPickup GetPickupInventory() const abstract;
// };


