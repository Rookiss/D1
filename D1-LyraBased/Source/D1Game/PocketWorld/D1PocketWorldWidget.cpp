#include "D1PocketWorldWidget.h"

#include "D1PocketStage.h"
#include "D1PocketWorldSubsystem.h"
#include "PocketCapture.h"
#include "Components/Image.h"
#include "Engine/TextureRenderTarget2D.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1PocketWorldWidget)

UD1PocketWorldWidget::UD1PocketWorldWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1PocketWorldWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (UD1PocketWorldSubsystem* PocketWorldSubsystem = GetWorld()->GetSubsystem<UD1PocketWorldSubsystem>())
	{
		PocketWorldSubsystem->RegisterAndCallForGetPocketStage(GetOwningLocalPlayer(), FGetPocketStageDelegate::CreateUObject(this, &UD1PocketWorldWidget::OnPocketStageReady));
	}
}

void UD1PocketWorldWidget::NativeConstruct()
{
	Super::NativeConstruct();

	GetWorld()->GetTimerManager().SetTimer(TickTimerHandle, this, &ThisClass::RefreshUI, 0.03f, true);
}

void UD1PocketWorldWidget::NativeDestruct()
{
	GetWorld()->GetTimerManager().ClearTimer(TickTimerHandle);
	
	Super::NativeDestruct();
}

void UD1PocketWorldWidget::RefreshUI()
{
	if (CachedPocketStage.IsValid())
	{
		if (UPocketCapture* PocketCapture = CachedPocketStage->GetPocketCapute())
		{
			PocketCapture->CaptureDiffuse();
			PocketCapture->CaptureAlphaMask();
		}
	}
}

void UD1PocketWorldWidget::OnPocketStageReady(AD1PocketStage* PocketStage)
{
	CachedPocketStage = PocketStage;
	
	if (UPocketCapture* PocketCapture = PocketStage->GetPocketCapute())
	{
		DiffuseRenderTarget = PocketCapture->GetOrCreateDiffuseRenderTarget();
		AlphaRenderTarget = PocketCapture->GetOrCreateAlphaMaskRenderTarget();

		UMaterialInstanceDynamic* MaterialInstanceDynamic = Image_Render->GetDynamicMaterial();
		MaterialInstanceDynamic->SetTextureParameterValue("Diffuse", DiffuseRenderTarget);
		MaterialInstanceDynamic->SetTextureParameterValue("AlphaMask", AlphaRenderTarget);
	}
}
