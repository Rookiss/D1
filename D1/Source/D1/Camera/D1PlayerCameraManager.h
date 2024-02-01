#pragma once

#include "D1PlayerCameraManager.generated.h"

#define CAMERA_DEFAULT_PITCH_MIN (-80.0f)
#define CAMERA_DEFAULT_PITCH_MAX (+80.0f)

UCLASS()
class AD1PlayerCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()
	
public:
	AD1PlayerCameraManager(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};
