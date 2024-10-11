#pragma once

#include "D1NumberPopComponent.h"
#include "D1NumberPopComponent_ScreenWidget.generated.h"

class UWidgetComponent;
class UD1NumberPopWidgetBase;
class UD1DamagePopStyleScreenWidget;

USTRUCT()
struct FPooledNumberPopWidgetList
{
	GENERATED_BODY()

public:
	UPROPERTY(Transient)
	TArray<TObjectPtr<UWidgetComponent>> Widgets;
};

USTRUCT()
struct FLiveWidgetNumberPopEntry
{
	GENERATED_BODY()

public:
	FLiveWidgetNumberPopEntry() { }

	FLiveWidgetNumberPopEntry(UWidgetComponent* InWidgetComponent, FPooledNumberPopWidgetList* InPoolList, float InReleaseTime)
		: WidgetComponent(InWidgetComponent), PoolList(InPoolList), ReleaseTime(InReleaseTime) { }
	
public:
	UPROPERTY(Transient)
	TObjectPtr<UWidgetComponent> WidgetComponent = nullptr;

	FPooledNumberPopWidgetList* PoolList = nullptr;

	float ReleaseTime = 0.f;
};

UCLASS(Blueprintable)
class UD1NumberPopComponent_ScreenWidget : public UD1NumberPopComponent
{
	GENERATED_BODY()
	
public:
	UD1NumberPopComponent_ScreenWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	virtual void AddNumberPop(const FD1NumberPopRequest& NewRequest) override;

protected:
	void ReleaseNextWidgets();

private:
	FLinearColor DetermineColor(const FD1NumberPopRequest& Request);
	TSubclassOf<UD1NumberPopWidgetBase> DetermineWidgetClass(const FD1NumberPopRequest& Request);

protected:
	UPROPERTY(EditDefaultsOnly)
	TArray<TObjectPtr<UD1DamagePopStyleScreenWidget>> Styles;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UD1NumberPopWidgetBase> DefaultWidgetClass;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float WidgetLifeSpan = 5.f;

protected:
	int32 DamageNumber = 0;

	UPROPERTY(Transient)
	TMap<TSubclassOf<UD1NumberPopWidgetBase>, FPooledNumberPopWidgetList> PooledWidgetMap;

	UPROPERTY(Transient)
	TArray<FLiveWidgetNumberPopEntry> LiveWidgets;

	FTimerHandle ReleaseTimerHandle;
};
