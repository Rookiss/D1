#pragma once

#include "D1ElectricField.generated.h"

UENUM(BlueprintType)
enum class ED1ElectricFieldState : uint8
{
	Break,
	Notice,
	Shrink,
	End,
};

UCLASS()
class AD1ElectricField : public AActor
{
	GENERATED_BODY()
	
public:
	AD1ElectricField(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
protected:
	virtual void Tick(float DeltaSeconds) override;

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> MeshComponent;

public:
	UPROPERTY(EditDefaultsOnly)
	float Damage = 5.f;
};
