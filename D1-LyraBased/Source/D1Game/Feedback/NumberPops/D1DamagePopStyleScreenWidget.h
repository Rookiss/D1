#pragma once

#include "GameplayTagContainer.h"
#include "D1DamagePopStyleScreenWidget.generated.h"

class UD1NumberPopWidgetBase;

UCLASS()
class UD1DamagePopStyleScreenWidget : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category="DamagePop")
	FGameplayTagQuery MatchPattern;

public:
	UPROPERTY(EditDefaultsOnly, Category="DamagePop")
	bool bOverrideColor = false;
	
	UPROPERTY(EditDefaultsOnly, Category="DamagePop", meta=(EditCondition=bOverrideColor))
	FLinearColor Color = FLinearColor::White;

	UPROPERTY(EditDefaultsOnly, Category="DamagePop", meta=(EditCondition=bOverrideColor))
	FLinearColor CriticalColor = FLinearColor::Yellow;

public:
	UPROPERTY(EditDefaultsOnly, Category="DamagePop")
	bool bOverrideWidget = false;

	UPROPERTY(EditDefaultsOnly, Category="DamagePop", meta=(EditCondition=bOverrideWidget))
	TSubclassOf<UD1NumberPopWidgetBase> WidgetClass;
};
