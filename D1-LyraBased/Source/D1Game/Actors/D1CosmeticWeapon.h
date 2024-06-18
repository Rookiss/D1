#pragma once

#include "D1CosmeticWeapon.generated.h"

class UArrowComponent;

UCLASS(BlueprintType, Blueprintable)
class AD1CosmeticWeapon : public AActor
{
	GENERATED_BODY()
	
public:
	AD1CosmeticWeapon(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UArrowComponent> ArrowComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USkeletalMeshComponent> WeaponMeshComponent;
};
