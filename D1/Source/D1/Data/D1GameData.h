#pragma once

#include "D1GameData.generated.h"

class UGameplayEffect;

UCLASS(BlueprintType, Const)
class UD1GameData : public UDataAsset
{
	GENERATED_BODY()

public:
	static const FName GameDataName;
	
public:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> DamageGameplayEffect;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> DynamicTagGameplayEffect;
};
