#include "D1PocketStage.h"

#include "D1PocketWorldSubsystem.h"
#include "PocketCapture.h"
#include "PocketCaptureSubsystem.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"
#include "Item/Managers/D1CosmeticManagerComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1PocketStage)

AD1PocketStage::AD1PocketStage(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));
	SetRootComponent(RootSceneComponent);
	
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(GetRootComponent());
	CameraComponent->SetRelativeLocationAndRotation(FVector(425.27f, 0.f, 65.f), FRotator(-10.f, -180.f, 0.f));
	CameraComponent->FieldOfView = 15.f;
	CameraComponent->AspectRatio = 0.5f;
	CameraComponent->SetConstraintAspectRatio(true);
	
	CharacterSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("CharacterSpawnPoint"));
	CharacterSpawnPoint->SetupAttachment(GetRootComponent());
}

void AD1PocketStage::BeginPlay()
{
	Super::BeginPlay();

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParameters.Owner = this;
	
	SpawnedCharacter = GetWorld()->SpawnActor<ACharacter>(CharacterClass, CharacterSpawnPoint->GetComponentTransform(), SpawnParameters);
	SpawnedCharacter->AttachToComponent(CharacterSpawnPoint, FAttachmentTransformRules::KeepWorldTransform);

	if (UPocketCaptureSubsystem* PocketCaptureSubsystem = GetWorld()->GetSubsystem<UPocketCaptureSubsystem>())
	{
		UPocketCapture* PocketCapture = PocketCaptureSubsystem->CreateThumbnailRenderer(PocketCaptureClass);
		CachedPocketCapture = PocketCapture;
		
		PocketCapture->SetRenderTargetSize(1024, 2048);
		PocketCapture->SetCaptureTarget(this);

		RefreshAlphaMaskActors();
		
		if (UD1PocketWorldSubsystem* PocketWorldSubsystem = GetWorld()->GetSubsystem<UD1PocketWorldSubsystem>())
		{
			PocketWorldSubsystem->SetPocketStage(this);
		}
	}
}

void AD1PocketStage::RefreshAlphaMaskActors()
{
	TArray<AActor*> AttachedActors;
	GetAttachedActors(AttachedActors, true, true);

	CachedPocketCapture->SetAlphaMaskedActors(AttachedActors);
}

UD1CosmeticManagerComponent* AD1PocketStage::GetCosmeticManager() const
{
	UD1CosmeticManagerComponent* Result = nullptr;
	if (SpawnedCharacter)
	{
		Result = SpawnedCharacter->GetComponentByClass<UD1CosmeticManagerComponent>();
	}
	return Result;
}
