#pragma once

#include "D1InteractionQuery.generated.h"

USTRUCT(BlueprintType)
struct FD1InteractionQuery
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
	TWeakObjectPtr<AActor> RequestingAvatar;
	
	UPROPERTY(BlueprintReadWrite)
	TWeakObjectPtr<AController> RequestingController;
	
	UPROPERTY(BlueprintReadWrite)
	TWeakObjectPtr<UObject> OptionalObjectData;
};
