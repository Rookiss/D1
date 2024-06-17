#include "D1PocketStage.h"

#include "D1PocketWorldSubsystem.h"
#include "PocketCapture.h"
#include "PocketCaptureSubsystem.h"
#include "Actors/D1ArmorBase.h"
#include "Camera/CameraComponent.h"
#include "Components/LightComponent.h"
#include "GameFramework/Character.h"

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

		TArray<AActor*> AttachedActors;
		GetAttachedActors(AttachedActors, true, true);

		PocketCapture->SetAlphaMaskedActors(AttachedActors);
		
		for (AActor* AttachedActor : AttachedActors)
		{
			if (AttachedActor)
			{
				if (AttachedActor->IsA(AD1ArmorBase::StaticClass()))
				{
					AlphaMaskMaterialActors.Add(AttachedActor);
				}
				
				TArray<UPrimitiveComponent*> PrimitiveComponents;
				AttachedActor->GetComponents<UPrimitiveComponent>(PrimitiveComponents);
				
				for (UPrimitiveComponent* PrimitiveComponent : PrimitiveComponents)
				{
					if (PrimitiveComponent)
					{
						PrimitiveComponent->SetLightingChannels(false, true, false);
					}
				}

				TArray<ULightComponent*> LightComponents;
				AttachedActor->GetComponents<ULightComponent>(LightComponents);

				for (ULightComponent* LightComponent : LightComponents)
				{
					if (LightComponent)
					{
						LightComponent->SetLightingChannels(false, true, false);
					}
				}
			}
		}
		AlphaMaskMaterialActors.Add(SpawnedCharacter);
		
		if (UD1PocketWorldSubsystem* PocketWorldSubsystem = GetWorld()->GetSubsystem<UD1PocketWorldSubsystem>())
		{
			PocketWorldSubsystem->SetPocketStage(this);
		}
	}
}

void AD1PocketStage::PreCaptureDiffuse_Implementation()
{
	
}

void AD1PocketStage::PostCaptureDiffuse_Implementation()
{
	
}

void AD1PocketStage::PreCaptureAlphaMask_Implementation()
{
	CachedMaterials.Reset();
	
	for (AActor* AlphaMaskMaterialActor : AlphaMaskMaterialActors)
	{
		if (AlphaMaskMaterialActor)
		{
			TArray<UPrimitiveComponent*> PrimitiveComponents;
			AlphaMaskMaterialActor->GetComponents<UPrimitiveComponent>(PrimitiveComponents);

			for (UPrimitiveComponent* PrimitiveComponent : PrimitiveComponents)
			{
				if (PrimitiveComponent)
				{
					for (int32 i = 0; i < PrimitiveComponent->GetNumMaterials(); i++)
					{
						CachedMaterials.Add(PrimitiveComponent->GetMaterial(i));
						PrimitiveComponent->SetMaterial(i, OverrideMaterial);
					}
				}
			}
		}
	}
}

void AD1PocketStage::PostCaptureAlphaMask_Implementation()
{
	int32 CachedMaterialsIndex = 0;
	
	for (AActor* AlphaMaskMaterialActor : AlphaMaskMaterialActors)
	{
		if (AlphaMaskMaterialActor)
		{
			TArray<UPrimitiveComponent*> PrimitiveComponents;
			AlphaMaskMaterialActor->GetComponents<UPrimitiveComponent>(PrimitiveComponents);

			for (UPrimitiveComponent* PrimitiveComponent : PrimitiveComponents)
			{
				if (PrimitiveComponent)
				{
					for (int32 i = 0; i < PrimitiveComponent->GetNumMaterials(); i++)
					{
						PrimitiveComponent->SetMaterial(i, CachedMaterials[CachedMaterialsIndex]);
						CachedMaterialsIndex++;
					}
				}
			}
		}
	}
}
