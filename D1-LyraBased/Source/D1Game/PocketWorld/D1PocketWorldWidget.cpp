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

void UD1PocketWorldWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (UD1PocketWorldSubsystem* PocketWorldSubsystem = GetWorld()->GetSubsystem<UD1PocketWorldSubsystem>())
	{
		PocketWorldSubsystem->RegisterAndCallForGetPocketStage(GetOwningLocalPlayer(), FGetPocketStageDelegate::CreateUObject(this, &UD1PocketWorldWidget::OnPocketStageReady));
	}
}

void UD1PocketWorldWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	RefreshRenderTarget();
}

void UD1PocketWorldWidget::NativeDestruct()
{
	if (UMaterialInstanceDynamic* MaterialInstanceDynamic = Image_Render->GetDynamicMaterial())
	{
		MaterialInstanceDynamic->ClearParameterValues();
	}
	
	Super::NativeDestruct();
}

void UD1PocketWorldWidget::RefreshRenderTarget()
{
	if (CachedPocketStage.IsValid())
	{
		if (UPocketCapture* PocketCapture = CachedPocketStage->GetPocketCapture())
		{
			PocketCapture->CaptureDiffuse();
		}
	}
}

void UD1PocketWorldWidget::OnPocketStageReady(AD1PocketStage* PocketStage)
{
	CachedPocketStage = PocketStage;

	if (UPocketCapture* PocketCapture = CachedPocketStage->GetPocketCapture())
	{
		UTextureRenderTarget2D* DiffuseRenderTarget = PocketCapture->GetOrCreateDiffuseRenderTarget();
		
		if (UMaterialInstanceDynamic* MaterialInstanceDynamic = Image_Render->GetDynamicMaterial())
		{
			MaterialInstanceDynamic->SetTextureParameterValue("Diffuse", DiffuseRenderTarget);
		}
	}
}
