#pragma once

#include "Actors/D1ElectricField.h"
#include "ActiveGameplayEffectHandle.h"
#include "Data/D1ElectricFieldPhaseData.h"
#include "Components/GameStateComponent.h"
#include "D1ElectricFieldManagerComponent.generated.h"

class ALyraCharacter;

UCLASS()
class UD1ElectricFieldManagerComponent : public UGameStateComponent
{
	GENERATED_BODY()
	
public:
	UD1ElectricFieldManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	UFUNCTION(BlueprintCallable)
	void Initialize();
	
protected:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void UninitializeComponent() override;
	
private:
	bool SetupNextElectricFieldPhase();
	void OnDamageTimer();

protected:
	UPROPERTY(EditDefaultsOnly)
	float Damage = 1.5f;

	UPROPERTY(EditDefaultsOnly)
	float DamageInterval = 1.5f;
	
private:
	int32 CurrentPhaseIndex = 0;
	ED1ElectricFieldState CurrentPhaseState = ED1ElectricFieldState::Break;
	FD1ElectricFieldPhaseEntry CurrentPhaseEntry;
	FD1ElectricFieldPhaseEntry CachedPhaseEntry;

	float StartPhaseRadius = 0.f;
	FVector StartPhasePosition = FVector::ZeroVector;

	FTimerHandle DamageTimerHandle;
	
private:
	UPROPERTY()
	TObjectPtr<AD1ElectricField> ElectricFieldActor;

	UPROPERTY()
	TMap<TWeakObjectPtr<ALyraCharacter>, FActiveGameplayEffectHandle> OutSideCharacters;
};
