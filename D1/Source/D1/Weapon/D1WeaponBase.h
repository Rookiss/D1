#pragma once

#include "D1WeaponBase.generated.h"

UCLASS()
class AD1WeaponBase : public AActor
{
	GENERATED_BODY()
	
public:
	AD1WeaponBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> WeaponMesh;
};
