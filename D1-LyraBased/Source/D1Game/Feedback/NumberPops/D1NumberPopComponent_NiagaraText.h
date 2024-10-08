#pragma once

#include "D1NumberPopComponent.h"
#include "D1NumberPopComponent_NiagaraText.generated.h"

class UD1DamagePopStyleNiagara;
class UNiagaraComponent;

UCLASS(Blueprintable)
class UD1NumberPopComponent_NiagaraText : public UD1NumberPopComponent
{
	GENERATED_BODY()

public:
	UD1NumberPopComponent_NiagaraText(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void AddNumberPop(const FD1NumberPopRequest& NewRequest) override;

protected:
	TArray<int32> DamageNumberArray;

	UPROPERTY(EditDefaultsOnly, Category = "Number Pop|Style")
	TObjectPtr<UD1DamagePopStyleNiagara> Style;

	UPROPERTY(EditDefaultsOnly, Category = "Number Pop|Style")
	TObjectPtr<UNiagaraComponent> NiagaraComp;
};
