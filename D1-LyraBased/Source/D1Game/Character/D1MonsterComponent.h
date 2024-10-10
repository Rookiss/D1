#pragma once

#include "Components/PawnComponent.h"
#include "Components/GameFrameworkInitStateInterface.h"
#include "D1MonsterComponent.generated.h"

UCLASS(Blueprintable, meta=(BlueprintSpawnableComponent))
class UD1MonsterComponent : public UPawnComponent, public IGameFrameworkInitStateInterface
{
	GENERATED_BODY()
	
public:
	UD1MonsterComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void OnRegister() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	virtual void HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) override;
	virtual void OnActorInitStateChanged(const FActorInitStateChangedParams& Params) override;
	virtual void CheckDefaultInitialization() override;
	virtual bool CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const override;

public:
	static const FName NAME_ActorFeatureName;
	virtual FName GetFeatureName() const override { return NAME_ActorFeatureName; }
	
	UFUNCTION(BlueprintPure)
	static UD1MonsterComponent* FindMonsterComponent(const AActor* Actor) { return (Actor ? Actor->FindComponentByClass<UD1MonsterComponent>() : nullptr); }
};
