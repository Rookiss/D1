#pragma once

#include "D1ItemFragment.h"
#include "D1ItemFragment_DisplayInfo.generated.h"

UCLASS(BlueprintType)
class UD1ItemFragment_DisplayInfo : public UD1ItemFragment
{
	GENERATED_BODY()
	
public:
	UD1ItemFragment_DisplayInfo(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText Description;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FSlateBrush IconBrush;
};
