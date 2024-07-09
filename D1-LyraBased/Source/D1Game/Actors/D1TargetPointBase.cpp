#include "D1TargetPointBase.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1TargetPointBase)

AD1TargetPointBase::AD1TargetPointBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

AActor* AD1TargetPointBase::SpawnActor()
{
	if (HasAuthority() == false)
		return nullptr;

	SpawnedActor = GetWorld()->SpawnActor<AActor>(SpawnActorClass, GetActorLocation(), GetActorRotation());
	return SpawnedActor;
}

void AD1TargetPointBase::DestroyActor()
{
	if (HasAuthority() == false)
		return;
	
	if (SpawnedActor)
	{
		SpawnedActor->Destroy(true);
	}
}
