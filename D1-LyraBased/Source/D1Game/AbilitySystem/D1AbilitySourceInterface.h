#pragma once

#include "UObject/Interface.h"
#include "D1AbilitySourceInterface.generated.h"

class UObject;
class UPhysicalMaterial;
struct FGameplayTagContainer;

UINTERFACE()
class UD1AbilitySourceInterface : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

class ID1AbilitySourceInterface
{
	GENERATED_IINTERFACE_BODY()

public:
	virtual float GetDistanceAttenuation(float Distance, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr) const abstract;
	virtual float GetPhysicalMaterialAttenuation(const UPhysicalMaterial* PhysicalMaterial, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr) const abstract;
};
