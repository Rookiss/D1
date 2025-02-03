#pragma once

#include "D1TeamInfoBase.h"
#include "D1TeamPrivateInfo.generated.h"

UCLASS()
class AD1TeamPrivateInfo : public AD1TeamInfoBase
{
	GENERATED_BODY()

public:
	AD1TeamPrivateInfo(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};
