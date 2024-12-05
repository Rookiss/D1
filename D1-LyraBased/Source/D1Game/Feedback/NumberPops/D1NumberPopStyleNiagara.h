#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "D1NumberPopStyleNiagara.generated.h"

class UNiagaraSystem;

UCLASS()
class UD1NumberPopStyleNiagara : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category="NumberPop")
	FName NiagaraArrayName;
	
	UPROPERTY(EditDefaultsOnly, Category="NumberPop")
	TObjectPtr<UNiagaraSystem> TextNiagara;
};
