#include "D1TargetPointBase.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1TargetPointBase)

AD1TargetPointBase::AD1TargetPointBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PreviewMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PreviewMeshComponent"));
	PreviewMeshComponent->SetupAttachment(RootComponent);
	PreviewMeshComponent->SetHiddenInGame(true);
	PreviewMeshComponent->bIsEditorOnly = true;
}

AActor* AD1TargetPointBase::SpawnActor()
{
	if (HasAuthority() == false)
		return nullptr;

	FTransform SpawnTransform = GetActorTransform();
	SpawnTransform.AddToTranslation(SpawnLocationOffset);
	
	AActor* SpawningActor = GetWorld()->SpawnActorDeferred<AActor>(SpawnActorClass, SpawnTransform, this, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	InitializeSpawningActor(SpawningActor);
	SpawningActor->FinishSpawning(SpawnTransform);
	SpawnedActor = SpawningActor;
	
	return SpawningActor;
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
