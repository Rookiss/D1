#include "D1AnimNotify_PlayCameraShake.h"

#include "Character/LyraCharacter.h"
#include "Player/LyraPlayerController.h"
#include "Shakes/LegacyCameraShake.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1AnimNotify_PlayCameraShake)

UD1AnimNotify_PlayCameraShake::UD1AnimNotify_PlayCameraShake(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
#if WITH_EDITORONLY_DATA
	bShouldFireInEditor = false;
#endif
	bIsNativeBranchingPoint = true;
}

void UD1AnimNotify_PlayCameraShake::Notify(USkeletalMeshComponent* MeshComponent, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComponent, Animation, EventReference);
	
	ALyraCharacter* LyraCharacter = Cast<ALyraCharacter>(MeshComponent->GetOwner());
	if (LyraCharacter == nullptr)
		return;
	
	ALyraPlayerController* LyraPlayerController = LyraCharacter->GetLyraPlayerController();
	if (LyraPlayerController == nullptr)
		return;
	
	APlayerCameraManager* PlayerCameraManager = LyraPlayerController->PlayerCameraManager;
	if (PlayerCameraManager == nullptr)
		return;
	
	ULegacyCameraShake::StartLegacyCameraShake(PlayerCameraManager, CameraShakeClass, PlayScale);
}
