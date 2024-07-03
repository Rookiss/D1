#include "D1AnimNotify_SpawnActor.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1AnimNotify_SpawnActor)

UD1AnimNotify_SpawnActor::UD1AnimNotify_SpawnActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
#if WITH_EDITORONLY_DATA
	bShouldFireInEditor = false;
#endif
}

void UD1AnimNotify_SpawnActor::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (ActorClass && MeshComp->DoesSocketExist(SocketName))
	{
		FTransform SocketTransform = MeshComp->GetSocketTransform(SocketName);
		
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		
		MeshComp->GetWorld()->SpawnActor(ActorClass, &SocketTransform, SpawnParameters);
	}
}
