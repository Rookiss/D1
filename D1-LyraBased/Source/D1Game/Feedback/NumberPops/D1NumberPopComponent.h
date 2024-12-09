#pragma once

#include "Components/ControllerComponent.h"
#include "GameplayTagContainer.h"
#include "D1NumberPopComponent.generated.h"

class UWidgetComponent;
class UD1NumberPopWidget;
class UD1NumberPopStyle;

USTRUCT(BlueprintType)
struct FNumberPopRequest
{
	GENERATED_BODY()

public:
	FNumberPopRequest()
		: WorldLocation(ForceInitToZero) { }

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="D1|Number Pop")
	FVector WorldLocation;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="D1|Number Pop")
	FGameplayTagContainer SourceTags;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="D1|Number Pop")
	FGameplayTagContainer TargetTags;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="D1|Number Pop")
	int32 NumberToDisplay = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="D1|Number Pop")
	bool bIsCriticalDamage = false;
};

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
class UD1NumberPopComponent : public UControllerComponent
{
	GENERATED_BODY()

public:
	UD1NumberPopComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	UFUNCTION(BlueprintCallable)
	void AddNumberPop(const FNumberPopRequest& NewRequest);

protected:
	void ReleaseNextWidgets();

private:
	FLinearColor DetermineColor(const FNumberPopRequest& Request);
	TSubclassOf<UD1NumberPopWidget> DetermineWidgetClass(const FNumberPopRequest& Request);

protected:
	UPROPERTY(EditDefaultsOnly)
	TArray<TObjectPtr<UD1NumberPopStyle>> Styles;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UD1NumberPopWidget> DefaultWidgetClass;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float WidgetLifeSpan = 3.f;

protected:
	int32 DamageNumber = 0;

	UPROPERTY(Transient)
	TMap<TSubclassOf<UD1NumberPopWidget>, FPooledNumberPopWidgetList> PooledWidgetMap;

	UPROPERTY(Transient)
	TArray<FLiveWidgetNumberPopEntry> LiveWidgets;

	FTimerHandle ReleaseTimerHandle;
};
