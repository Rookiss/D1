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
	
	AActor* SpawningActor = GetWorld()->SpawnActorDeferred<AActor>(SpawnActorClass, GetActorTransform(), this, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	InitializeSpawningActor(SpawningActor);
	SpawningActor->FinishSpawning(GetActorTransform());
	SpawnedActor = SpawningActor;
	
	return SpawningActor;
}

void AD1TargetPointBase::InitializeSpawningActor(AActor* InSpawningActor)
{
	
}

void AD1TargetPointBase::DestroyActor()
{
	if (HasAuthority() == false)
		return;
	
	if (SpawnedActor.Get())
	{
		SpawnedActor->Destroy(true);
	}
}
