#pragma once

#include "Actors/D1ElectricField.h"
#include "Data/D1ElectricFieldPhaseData.h"
#include "Components/GameStateComponent.h"
#include "D1ElectricFieldManagerComponent.generated.h"

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

private:
	void NextElectricFieldPhase();
	
private:
	int32 CurrentPhaseIndex = 0;
	ED1ElectricFieldState CurrentPhaseState = ED1ElectricFieldState::Break;
	FD1ElectricFieldPhaseEntry CurrentPhaseEntry;
	
	FVector DeltaPositionPerSecond = FVector::ZeroVector;
	float DeltaRadiusPerSecond = 0.f;

	float CurrentPhaseRadius = 0.f;
	float TargetPhaseRadius = 0.f;

	FVector CurrentPhasePosition = FVector::ZeroVector;
	FVector TargetPhasePosition = FVector::ZeroVector;
	
	float RemainTime = 0.f;

private:
	UPROPERTY()
	TObjectPtr<AD1ElectricField> ElectricFieldActor;
};
