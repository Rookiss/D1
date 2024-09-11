#pragma once

#include "Abilities/GameplayAbilityTargetActor_Trace.h"
#include "D1GameplayAbilityTargetActor_LineTraceHighlight.generated.h"

UCLASS()
class AD1GameplayAbilityTargetActor_LineTraceHighlight : public AGameplayAbilityTargetActor_Trace
{
	GENERATED_BODY()
	
public:
	AD1GameplayAbilityTargetActor_LineTraceHighlight(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual FHitResult PerformTrace(AActor* InSourceActor) override;
	virtual void Tick(float DeltaSeconds) override;

private:
	void HighlightActor(bool bShouldHighlight, AActor* ActorToHighlight);
	
protected:
	UPROPERTY()
	TWeakObjectPtr<AActor> CachedTracedActor;
};
