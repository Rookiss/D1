#include "D1FootprintActor.h"

#include "Components/ArrowComponent.h"
#include "Components/DecalComponent.h"
#include "Components/TimelineComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1FootprintActor)

AD1FootprintActor::AD1FootprintActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	SetRootComponent(SceneComponent);

	ArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowComponent"));
	ArrowComponent->SetupAttachment(SceneComponent);

	DecalComponent = CreateDefaultSubobject<UDecalComponent>(TEXT("DecalComponent"));
	DecalComponent->SetupAttachment(ArrowComponent);
	DecalComponent->SetRelativeRotation(FRotator(90.f, 0.f, -90.f));
	DecalComponent->DecalSize = FVector(8.f, 25.6f, 12.8f);
	
	TimelineComponent = CreateDefaultSubobject<UTimelineComponent>(TEXT("TimelineComponent"));
	TimelineComponent->SetLooping(false);
	TimelineComponent->SetTickGroup(TG_PrePhysics);
	TimelineComponent->SetTimelineLengthMode(TL_LastKeyFrame);
}

void AD1FootprintActor::BeginPlay()
{
	Super::BeginPlay();
	
	DynamicMaterialInstance = DecalComponent->CreateDynamicMaterialInstance();
	
	FOnTimelineFloat TimelineTickCallback;
	TimelineTickCallback.BindDynamic(this, &ThisClass::OnTimelineTick);
	TimelineComponent->AddInterpFloat(FadeCurve, TimelineTickCallback);
}

void AD1FootprintActor::InitializeActor(bool bIsRightSide, EPhysicalSurface SurfaceType)
{
	if (DynamicMaterialInstance)
	{
		DynamicMaterialInstance->SetTextureParameterValue("Tex_Base", bIsRightSide ? RightFootprintTexture : LeftFootprintTexture);
		TimelineComponent->PlayFromStart();
	}
}

void AD1FootprintActor::OnTimelineTick(float Value)
{
	if (DynamicMaterialInstance)
	{
		DynamicMaterialInstance->SetScalarParameterValue("Dissolve", Value);
	}
}
