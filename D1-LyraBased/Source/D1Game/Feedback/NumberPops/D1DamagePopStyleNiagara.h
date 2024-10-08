#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "D1DamagePopStyleNiagara.generated.h"

class UNiagaraSystem;

UCLASS()
class UD1DamagePopStyleNiagara : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category="DamagePop")
	FName NiagaraArrayName;
	
	UPROPERTY(EditDefaultsOnly, Category="DamagePop")
	TObjectPtr<UNiagaraSystem> TextNiagara;
};
