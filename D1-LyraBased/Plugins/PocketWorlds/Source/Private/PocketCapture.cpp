#include "PocketCapture.h"

#include "Camera/CameraComponent.h"
#include "Camera/CameraTypes.h"
#include "Components/PrimitiveComponent.h"
#include "Components/SceneCaptureComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "PocketCaptureSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PocketCapture)

UPocketCapture::UPocketCapture()
{
	
}

void UPocketCapture::Initialize(UWorld* InWorld, int32 InRendererIndex)
{
	PrivateWorld = InWorld;
	RendererIndex = InRendererIndex;

	CaptureComponent = NewObject<USceneCaptureComponent2D>(this, "Thumbnail_Capture_Component");
	CaptureComponent->RegisterComponentWithWorld(InWorld);
	CaptureComponent->bConsiderUnrenderedOpaquePixelAsFullyTranslucent = true;
	CaptureComponent->PrimitiveRenderMode = ESceneCapturePrimitiveRenderMode::PRM_UseShowOnlyList;
	CaptureComponent->bCaptureEveryFrame = false;
	CaptureComponent->bCaptureOnMovement = false;
	CaptureComponent->bAlwaysPersistRenderingState = true;
}

void UPocketCapture::Deinitialize()
{
	CaptureComponent->UnregisterComponent();
}

void UPocketCapture::BeginDestroy()
{
	Super::BeginDestroy();

	if (CaptureComponent)
	{
		CaptureComponent->UnregisterComponent();
		CaptureComponent = nullptr;
	}
}

void UPocketCapture::SetRenderTargetSize(int32 Width, int32 Height)
{
	if (SurfaceWidth != Width || SurfaceHeight != Height)
	{
		SurfaceWidth = Width;
		SurfaceHeight = Height;

		if (DiffuseRT)
		{
			DiffuseRT->ResizeTarget(SurfaceWidth, SurfaceHeight);
		}

		if (AlphaMaskRT)
		{
			AlphaMaskRT->ResizeTarget(SurfaceWidth, SurfaceHeight);
		}

		if (EffectsRT)
		{
			EffectsRT->ResizeTarget(SurfaceWidth, SurfaceHeight);
		}
	}
}

UTextureRenderTarget2D* UPocketCapture::GetOrCreateDiffuseRenderTarget()
{
	if (DiffuseRT == nullptr)
	{
		DiffuseRT = NewObject<UTextureRenderTarget2D>(this, TEXT("ThumbnailRenderer_Diffuse"));
		DiffuseRT->RenderTargetFormat = RTF_RGBA8;
		DiffuseRT->InitAutoFormat(SurfaceWidth, SurfaceHeight);
		DiffuseRT->UpdateResourceImmediate(true);
	}

	return DiffuseRT;
}

UTextureRenderTarget2D* UPocketCapture::GetOrCreateAlphaMaskRenderTarget()
{
	if (AlphaMaskRT == nullptr)
	{
		AlphaMaskRT = NewObject<UTextureRenderTarget2D>(this, TEXT("ThumbnailRenderer_AlphaMask"));
		AlphaMaskRT->RenderTargetFormat = RTF_R8;
		AlphaMaskRT->InitAutoFormat(SurfaceWidth, SurfaceHeight);
		AlphaMaskRT->UpdateResourceImmediate(true);
	}

	return AlphaMaskRT;
}

UTextureRenderTarget2D* UPocketCapture::GetOrCreateEffectsRenderTarget()
{
	if (EffectsRT == nullptr)
	{
		EffectsRT = NewObject<UTextureRenderTarget2D>(this, TEXT("ThumbnailRenderer_Fx"));
		EffectsRT->RenderTargetFormat = RTF_R8;
		EffectsRT->InitAutoFormat(SurfaceWidth, SurfaceHeight);
		EffectsRT->UpdateResourceImmediate(true);
	}

	return EffectsRT;
}

void UPocketCapture::SetCaptureTarget(AActor* InCaptureTarget)
{
	CaptureTargetPtr = InCaptureTarget;

	OnCaptureTargetChanged(InCaptureTarget);
}

void UPocketCapture::SetAlphaMaskedActors(const TArray<AActor*>& InCaptureTargets)
{
	AlphaMaskActorPtrs.Reset();

	for (AActor* CaptureTarget : InCaptureTargets)
	{
		AlphaMaskActorPtrs.Add(CaptureTarget);
	}
}

UPocketCaptureSubsystem* UPocketCapture::GetThumbnailSystem() const
{
	return CastChecked<UPocketCaptureSubsystem>(GetOuter());
}

TArray<UPrimitiveComponent*> UPocketCapture::GatherPrimitivesForCapture(const TArray<AActor*>& InCaptureActors) const
{
	const bool bIncludeFromChildActors = true;
	TArray<UPrimitiveComponent*> PrimitiveComponents;

	for (AActor* CaptureActor : InCaptureActors)
	{
		TArray<UPrimitiveComponent*> ChildPrimitiveComponents;
		CaptureActor->GetComponents(ChildPrimitiveComponents, bIncludeFromChildActors);

		for (UPrimitiveComponent* ChildPrimitiveComponent : ChildPrimitiveComponents)
		{
			if (!ChildPrimitiveComponent->bHiddenInGame)
			{
				PrimitiveComponents.Add(ChildPrimitiveComponent);
			}
		}
	}

	return PrimitiveComponents;
}

bool UPocketCapture::CaptureScene(UTextureRenderTarget2D* InRenderTarget, const TArray<AActor*>& InCaptureActors, ESceneCaptureSource InCaptureSource, UMaterialInterface* OverrideMaterial)
{
	if (InRenderTarget == nullptr)
		return false;

	if (AActor* CaptureTarget = CaptureTargetPtr.Get())
	{
		if (InCaptureActors.Num() > 0)
		{
			TArray<UPrimitiveComponent*> PrimitiveComponents = GatherPrimitivesForCapture(InCaptureActors);
			
			GetThumbnailSystem()->StreamThisFrame(PrimitiveComponents);

			TArray<UMaterialInterface*> OriginalMaterials;
			if (OverrideMaterial)
			{
				for (UPrimitiveComponent* PrimitiveComponent : PrimitiveComponents)
				{
					const int32 MaterialCount = PrimitiveComponent->GetNumMaterials();
					for (int32 MaterialIndex = 0; MaterialIndex < MaterialCount; MaterialIndex++)
					{
						OriginalMaterials.Add(PrimitiveComponent->GetMaterial(MaterialIndex));

						PrimitiveComponent->SetMaterial(MaterialIndex, OverrideMaterial);
					}
				}
			}

			UCameraComponent* Camera = CaptureTarget->FindComponentByClass<UCameraComponent>();
			if (ensure(Camera))
			{
				CaptureComponent->ShowOnlyActors = InCaptureActors;

				FMinimalViewInfo CaptureView;
				Camera->GetCameraView(0, CaptureView);

				CaptureComponent->TextureTarget = InRenderTarget;
				CaptureComponent->PostProcessSettings = Camera->PostProcessSettings;
				CaptureComponent->SetCameraView(CaptureView);

				CaptureComponent->ShowFlags.SetDepthOfField(false);
				CaptureComponent->ShowFlags.SetMotionBlur(true); // Check
				CaptureComponent->ShowFlags.SetScreenPercentage(false);
				CaptureComponent->ShowFlags.SetScreenSpaceReflections(false);
				CaptureComponent->ShowFlags.SetDistanceFieldAO(false);
				CaptureComponent->ShowFlags.SetLensFlares(false);
				CaptureComponent->ShowFlags.SetOnScreenDebug(false);
				CaptureComponent->ShowFlags.SetEyeAdaptation(true); // Check
				CaptureComponent->ShowFlags.SetColorGrading(false);
				CaptureComponent->ShowFlags.SetCameraImperfections(false);
				CaptureComponent->ShowFlags.SetVignette(false);
				CaptureComponent->ShowFlags.SetGrain(false);
				CaptureComponent->ShowFlags.SetSeparateTranslucency(false);
				CaptureComponent->ShowFlags.SetScreenPercentage(false);
				CaptureComponent->ShowFlags.SetScreenSpaceReflections(false);
				CaptureComponent->ShowFlags.SetTemporalAA(false);
				CaptureComponent->ShowFlags.SetAmbientOcclusion(false);
				CaptureComponent->ShowFlags.SetIndirectLightingCache(false);
				CaptureComponent->ShowFlags.SetLightShafts(false);
				CaptureComponent->ShowFlags.SetPostProcessMaterial(false);
				CaptureComponent->ShowFlags.SetHighResScreenshotMask(false);
				CaptureComponent->ShowFlags.SetHMDDistortion(false);
				CaptureComponent->ShowFlags.SetStereoRendering(false);
				CaptureComponent->ShowFlags.SetFog(false); // Check
				CaptureComponent->ShowFlags.SetVolumetricFog(false);
				CaptureComponent->ShowFlags.SetVolumetricLightmap(false);
				CaptureComponent->ShowFlags.SetSkyLighting(false);
				
				CaptureComponent->CaptureSource = InCaptureSource;
				CaptureComponent->ProfilingEventName = TEXT("Pocket Capture");
				CaptureComponent->CaptureScene();

				if (OriginalMaterials.Num() > 0)
				{
					int32 TotalMaterialIndex = 0;
					for (UPrimitiveComponent* PrimitiveComponent : PrimitiveComponents)
					{
						const int32 MaterialCount = PrimitiveComponent->GetNumMaterials();
						for (int32 MaterialIndex = 0; MaterialIndex < MaterialCount; MaterialIndex++)
						{
							PrimitiveComponent->SetMaterial(MaterialIndex, OriginalMaterials[TotalMaterialIndex]);
							TotalMaterialIndex++;
						}
					}
				}

				return true;
			}
		}
	}

	return false;
}

void UPocketCapture::CaptureDiffuse()
{
	if (UTextureRenderTarget2D* RenderTarget = GetOrCreateDiffuseRenderTarget())
	{
		TArray<AActor*> CaptureActors;
		if (AActor* CaptureTarget = CaptureTargetPtr.Get())
		{
			CaptureTarget->GetAttachedActors(CaptureActors, true, true);
			CaptureActors.Add(CaptureTarget);
		}

		CaptureScene(RenderTarget, CaptureActors, ESceneCaptureSource::SCS_FinalColorLDR, nullptr);
	}
}

void UPocketCapture::CaptureAlphaMask()
{
	if (UTextureRenderTarget2D* RenderTarget = GetOrCreateAlphaMaskRenderTarget())
	{
		TArray<AActor*> CaptureActors;
		for (const TWeakObjectPtr<AActor>& AlphaMaskTargetPtr : AlphaMaskActorPtrs)
		{
			if (AActor* AlphaMaskTarget = AlphaMaskTargetPtr.Get())
			{
				CaptureActors.Add(AlphaMaskTarget);
			}
		}

		CaptureScene(RenderTarget, CaptureActors, ESceneCaptureSource::SCS_SceneColorHDR, AlphaMaskMaterial);
	}
}

void UPocketCapture::CaptureEffects()
{
	if (UTextureRenderTarget2D* RenderTarget = GetOrCreateEffectsRenderTarget())
	{
		ensure(false);//TODO
		TArray<AActor*> CaptureActors;
		CaptureScene(RenderTarget, CaptureActors, ESceneCaptureSource::SCS_SceneColorHDR, EffectMaskMaterial);
	}
}

void UPocketCapture::ReleaseResources()
{
	if (DiffuseRT)
	{
		DiffuseRT->ReleaseResource();
	}

	if (AlphaMaskRT)
	{
		AlphaMaskRT->ReleaseResource();
	}

	if (EffectsRT)
	{
		EffectsRT->ReleaseResource();
	}

	//OnReleaseResources();
}

void UPocketCapture::ReclaimResources()
{
	if (DiffuseRT)
	{
		DiffuseRT->UpdateResource();
	}

	if (AlphaMaskRT)
	{
		AlphaMaskRT->UpdateResource();
	}

	if (EffectsRT)
	{
		EffectsRT->UpdateResource();
	}

	//OnReclaimResources();
}

int32 UPocketCapture::GetRendererIndex() const
{
	return RendererIndex;
}
