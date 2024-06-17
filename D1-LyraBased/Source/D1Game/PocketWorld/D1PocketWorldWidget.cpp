#include "D1PocketWorldWidget.h"

#include "D1PocketStage.h"
#include "D1PocketWorldSubsystem.h"
#include "PocketCapture.h"
#include "Components/Image.h"

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
		FPocketStageDelegate Delegate;
		Delegate.AddDynamic(this, &UD1PocketWorldWidget::OnPocketStageReady);
		
		PocketWorldSubsystem->GetPocketStage(Delegate);
	}
}

void UD1PocketWorldWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	
}

void UD1PocketWorldWidget::OnPocketStageReady(AD1PocketStage* PocketStage)
{
	if (UPocketCapture* PocketCapture = PocketStage->GetPocketCapute())
	{
		DiffuseRenderTarget = PocketCapture->GetOrCreateDiffuseRenderTarget();
		AlphaRenderTarget = PocketCapture->GetOrCreateAlphaMaskRenderTarget();

		UMaterialInstanceDynamic* MaterialInstanceDynamic = Image_Render->GetDynamicMaterial();
		MaterialInstanceDynamic->SetTextureParameterValue("Diffuse", DiffuseRenderTarget->GetTexture);
		MaterialInstanceDynamic->SetTextureParameterValue("AlphaMask", AlphaRenderTarget);
	}
}
