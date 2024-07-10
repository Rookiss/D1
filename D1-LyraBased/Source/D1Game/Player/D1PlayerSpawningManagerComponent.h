#pragma once

#include "Components/GameStateComponent.h"
#include "D1PlayerSpawningManagerComponent.generated.h"

class AController;
class APlayerController;
class APlayerState;
class APlayerStart;
class AD1PlayerStart;

UCLASS()
class UD1PlayerSpawningManagerComponent : public UGameStateComponent
{
	GENERATED_BODY()
	
public:
	UD1PlayerSpawningManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void InitializeComponent() override;
	
	virtual AActor* OnChoosePlayerStart(AController* Player, TArray<AD1PlayerStart*>& PlayerStarts);
	virtual void OnFinishRestartPlayer(AController* Player, const FRotator& StartRotation);
	APlayerStart* GetFirstRandomUnoccupiedPlayerStart(AController* Controller, const TArray<AD1PlayerStart*>& StartPoints) const;

	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName=OnFinishRestartPlayer))
	void K2_OnFinishRestartPlayer(AController* Player, const FRotator& StartRotation);
	
private:
	void OnLevelAdded(ULevel* InLevel, UWorld* InWorld);
	void HandleOnActorSpawned(AActor* SpawnedActor);

	AActor* ChoosePlayerStart(AController* Player);
	bool ControllerCanRestart(AController* Player);
	void FinishRestartPlayer(AController* NewPlayer, const FRotator& StartRotation);
	friend class ALyraGameMode;
	
private:
	UPROPERTY(Transient)
	TArray<TWeakObjectPtr<AD1PlayerStart>> CachedPlayerStarts;

#if WITH_EDITOR
	APlayerStart* FindPlayFromHereStart(AController* Player);
#endif
};
