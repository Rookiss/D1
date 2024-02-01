#include "D1PlayerCameraManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1PlayerCameraManager)

AD1PlayerCameraManager::AD1PlayerCameraManager(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ViewPitchMin = CAMERA_DEFAULT_PITCH_MIN;
	ViewPitchMax = CAMERA_DEFAULT_PITCH_MAX;
}
