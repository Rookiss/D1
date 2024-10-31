#include "D1TargetPointBase.h"

#include "GameModes/LyraExperienceManagerComponent.h"
#include "Kismet/GameplayStatics.h"
#include "System/D1ActorSpawningManagerComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1TargetPointBase)

AD1TargetPointBase::AD1TargetPointBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
	
	PreviewMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PreviewMeshComponent"));
	PreviewMeshComponent->SetupAttachment(RootComponent);
	PreviewMeshComponent->SetCollisionProfileName(TEXT("NoCollision"));
	PreviewMeshComponent->SetHiddenInGame(true);
	PreviewMeshComponent->bIsEditorOnly = true;
}

void AD1TargetPointBase::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority() == false)
		return;

	AGameStateBase* GameState = UGameplayStatics::GetGameState(GetWorld());
	ULyraExperienceManagerComponent* ExperienceComponent = GameState->FindComponentByClass<ULyraExperienceManagerComponent>();
	check(ExperienceComponent);
	ExperienceComponent->CallOrRegister_OnExperienceLoaded_LowPriority(FOnLyraExperienceLoaded::FDelegate::CreateUObject(this, &ThisClass::OnExperienceLoaded));
}

void AD1TargetPointBase::OnExperienceLoaded(const ULyraExperienceDefinition* ExperienceDefinition)
{
	if (HasAuthority() == false)
		return;
	
	AGameStateBase* GameState = UGameplayStatics::GetGameState(GetWorld());
	UD1ActorSpawningManagerComponent* ActorSpawningManager = GameState->FindComponentByClass<UD1ActorSpawningManagerComponent>();
	check(ActorSpawningManager);
	ActorSpawningManager->RegisterTargetPoint(this);
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
