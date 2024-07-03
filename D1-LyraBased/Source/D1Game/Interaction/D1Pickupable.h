#pragma once

#include "UObject/Interface.h"
#include "D1Pickupable.generated.h"

class UD1ItemInstance;

USTRUCT(BlueprintType)
struct FD1PickupTemplate
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	int32 ItemTemplateID = 0;

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
	TArray<FD1PickupTemplate> ItemTemplates;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FD1PickupInstance> ItemInstances;
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
	virtual FD1PickupInfo GetPickupInfo() const abstract;
};
