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
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
private:
	bool SetupNextElectricFieldPhase();
	void RemoveAllDamageEffects();

public:
	UPROPERTY(Replicated)
	bool bShouldShow = false;
	
	UPROPERTY(Replicated)
	float TargetPhaseRadius = 0.f;
	
	UPROPERTY(Replicated)
	FVector TargetPhasePosition = FVector::ZeroVector;

	UPROPERTY(Replicated, BlueprintReadOnly)
	int32 RemainTimeSeconds = 0;

	UPROPERTY()
	TObjectPtr<AD1ElectricField> ElectricFieldActor;
	
private:
	int32 CurrentPhaseIndex = 0;
	ED1ElectricFieldState CurrentPhaseState = ED1ElectricFieldState::Break;
	FD1ElectricFieldPhaseEntry CurrentPhaseEntry;

	float StartPhaseRadius = 0.f;
	FVector StartPhasePosition = FVector::ZeroVector;
	
	float RemainTime = 0.f;

private:
	UPROPERTY()
	TMap<TWeakObjectPtr<ALyraCharacter>, FActiveGameplayEffectHandle> OutsideCharacters;
};
