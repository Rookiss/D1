#pragma once

#include "UObject/Interface.h"
#include "D1Pickupable.generated.h"

class UD1ItemInstance;

USTRUCT()
struct FPickupTemplate
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	int32 ItemID = 0;

	UPROPERTY(EditAnywhere)
	int32 ItemCount = 1;
};

USTRUCT()
struct FPickupInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	TObjectPtr<UD1ItemInstance> ItemInstance;

	UPROPERTY(EditAnywhere)
	int32 ItemCount = 1;
};

USTRUCT()
struct FPickupInfo
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	TArray<FPickupTemplate> Templates;

	UPROPERTY(EditAnywhere)
	TArray<FPickupInstance> Instances;
};

UINTERFACE(MinimalAPI)
class UD1Pickupable : public UInterface
{
	GENERATED_BODY()
};

class ID1Pickupable
{
	GENERATED_BODY()

public:
	virtual const FPickupInfo& GetPickupInfo() const abstract;
};
