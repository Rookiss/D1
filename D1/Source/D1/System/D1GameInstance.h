#pragma once

#include "D1GameInstance.generated.h"

UCLASS()
class UD1GameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	UD1GameInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	virtual void Init() override;
	virtual void Shutdown() override;
};
