#pragma once

#include "D1Define.h"
#include "UObject/Interface.h"
#include "D1Pickupable.generated.h"

class UD1ItemTemplate;
class UD1ItemInstance;

USTRUCT(BlueprintType)
struct FD1PickupTemplate
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	TSubclassOf<UD1ItemTemplate> ItemTemplateClass;

	UPROPERTY(EditAnywhere)
	EItemRarity ItemRarity = EItemRarity::Poor;
	
	UPROPERTY(EditAnywhere)
	int32 ItemCount = 1;
};

USTRUCT(BlueprintType)
struct FD1PickupInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	TObjectPtr<UD1ItemInstance> ItemInstance;

	UPROPERTY(EditAnywhere)
	int32 ItemCount = 1;
};

USTRUCT(BlueprintType)
struct FD1PickupInfo
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FD1PickupTemplate PickupTemplate;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FD1PickupInstance PickupInstance;
};

UINTERFACE(MinimalAPI, BlueprintType, meta=(CannotImplementInterfaceInBlueprint))
class UD1Pickupable : public UInterface
{
	GENERATED_BODY()
};

class ID1Pickupable
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	virtual FD1PickupInfo GetPickupInfo() const = 0;
};
