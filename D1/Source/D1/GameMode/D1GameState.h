#pragma once

#include "GameFramework/GameStateBase.h"
#include "D1GameState.generated.h"

UCLASS()
class AD1GameState : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	AD1GameState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void Tick(float DeltaSeconds) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	UPROPERTY(Replicated)
	float AvgPing;
};
