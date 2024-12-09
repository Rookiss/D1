#pragma once

#include "D1FootprintActor.generated.h"

class UArrowComponent;
class UTimelineComponent;

UCLASS()
class AD1FootprintActor : public AActor
{
	GENERATED_BODY()
	
public:
	AD1FootprintActor(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void BeginPlay() override;
	
public:
	void InitializeActor(bool bIsRightSide, EPhysicalSurface SurfaceType = SurfaceType_Default);
	
private:
	UFUNCTION()
	void OnTimelineTick(float Value);
	
protected:
	UPROPERTY(EditDefaultsOnly, Category="Footprint")
	TObjectPtr<UCurveFloat> FadeCurve;

	UPROPERTY(EditDefaultsOnly, Category="Footprint")
	TObjectPtr<UTexture2D> LeftFootprintTexture;

	UPROPERTY(EditDefaultsOnly, Category="Footprint")
	TObjectPtr<UTexture2D> RightFootprintTexture;
	
private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> SceneComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UArrowComponent> ArrowComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UDecalComponent> DecalComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UTimelineComponent> TimelineComponent;

private:
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> DynamicMaterialInstance;
};
