#pragma once

#include "D1AOEBase.generated.h"

class ULyraCameraMode;
class USphereComponent;
class UArrowComponent;

UCLASS(BlueprintType, Abstract)
class AD1AOEBase : public AActor
{
	GENERATED_BODY()
	
public:
	AD1AOEBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void BeginPlay() override;

private:
	void BeginAOE();
	void TickAOE();

protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UArrowComponent> ArrowComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USphereComponent> SphereComponent;
	
protected:
	UPROPERTY(EditDefaultsOnly, Category="D1|AOE")
	TSubclassOf<AActor> AOEElementClass;

	UPROPERTY(EditDefaultsOnly, Category="D1|AOE")
	float StartDelay = 2.f;
	
	UPROPERTY(EditDefaultsOnly, Category="D1|AOE")
	float AttackTotalTime = 10.f;
	
	UPROPERTY(EditDefaultsOnly, Category="D1|AOE")
	int32 TargetAttackCount = 5;

	UPROPERTY(EditDefaultsOnly, Category="D1|AOE")
	TSubclassOf<ULyraCameraMode> StartCameraModeClass;

	UPROPERTY(EditDefaultsOnly, Category="D1|AOE")
	TSubclassOf<ULyraCameraMode> TickCameraModeClass;
	
private:
	int32 CurrentAttackCount = 0;
	float AttackIntervalTime = 0.f;
	FTimerHandle AOETimerHandle;
};
