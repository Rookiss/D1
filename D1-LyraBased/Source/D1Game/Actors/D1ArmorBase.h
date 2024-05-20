#pragma once

#include "D1ArmorBase.generated.h"

UCLASS(BlueprintType, Blueprintable)
class AD1ArmorBase : public AActor
{
	GENERATED_BODY()
	
public:
	AD1ArmorBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	void SetArmorMesh(USkeletalMesh* ArmorMesh);
	
private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USkeletalMeshComponent> ArmorMeshComponent;
};
