#pragma once

#include "GameplayTagContainer.h"
#include "D1FootprintStyle.generated.h"

class AD1FootprintActor;

UCLASS()
class UD1FootprintStyle : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, Category="Footprint")
	FGameplayTagQuery MatchPattern;

public:
	UPROPERTY(EditDefaultsOnly, Category="Footprint")
	bool bOverrideFootprint = false;

	UPROPERTY(EditDefaultsOnly, Category="Footprint", meta=(EditCondition=bOverrideFootprint))
	TSubclassOf<AD1FootprintActor> FootprintClass;
};
