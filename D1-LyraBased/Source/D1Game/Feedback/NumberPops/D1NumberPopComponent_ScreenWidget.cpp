#include "D1NumberPopComponent_ScreenWidget.h"

#include "D1DamagePopStyleScreenWidget.h"
#include "Blueprint/SlateBlueprintLibrary.h"
#include "Components/WidgetComponent.h"
#include "UI/HUD/D1NumberPopWidgetBase.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1NumberPopComponent_ScreenWidget)

UD1NumberPopComponent_ScreenWidget::UD1NumberPopComponent_ScreenWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1NumberPopComponent_ScreenWidget::AddNumberPop(const FD1NumberPopRequest& NewRequest)
{
	Super::AddNumberPop(NewRequest);

	APlayerController* PlayerController = GetController<APlayerController>();
	if (PlayerController->IsLocalController() == false)
		return;
	
	TSubclassOf<UD1NumberPopWidgetBase> WidgetClassToUse = DetermineWidgetClass(NewRequest);
	if (WidgetClassToUse == nullptr)
		return;

	FPooledNumberPopWidgetList& WidgetPool = PooledWidgetMap.FindOrAdd(WidgetClassToUse);
	
	UWidgetComponent* ComponentToUse;
	if (WidgetPool.Widgets.Num() > 0)
	{
		ComponentToUse = WidgetPool.Widgets.Pop();
	}
	else
	{		
		ComponentToUse = NewObject<UWidgetComponent>(GetOwner());
		ComponentToUse->SetupAttachment(nullptr);
		ComponentToUse->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
		ComponentToUse->SetWidgetSpace(EWidgetSpace::Screen);
		ComponentToUse->SetWidgetClass(WidgetClassToUse);
	}

	check(ComponentToUse);
	ComponentToUse->RegisterComponent();

	UWorld* LocalWorld = GetWorld();
	check(LocalWorld);
	LiveWidgets.Emplace(ComponentToUse, &WidgetPool, LocalWorld->GetTimeSeconds() + WidgetLifeSpan);

	if (LocalWorld->GetTimerManager().IsTimerActive(ReleaseTimerHandle) == false)
	{
		LocalWorld->GetTimerManager().SetTimer(ReleaseTimerHandle, this, &ThisClass::ReleaseNextWidgets, WidgetLifeSpan);
	}

	if (UD1NumberPopWidgetBase* NumberPopWidget = Cast<UD1NumberPopWidgetBase>(ComponentToUse->GetWidget()))
	{
		NumberPopWidget->InitializeUI(NewRequest.NumberToDisplay, DetermineColor(NewRequest));
	}

	ComponentToUse->SetWorldLocation(NewRequest.WorldLocation);
}

void UD1NumberPopComponent_ScreenWidget::ReleaseNextWidgets()
{
	UWorld* LocalWorld = GetWorld();
	check(LocalWorld);

	const float CurrentTime = LocalWorld->GetTimeSeconds();

	int32 NumRelased = 0;
	for (const FLiveWidgetNumberPopEntry& LiveWidget : LiveWidgets)
	{
		if (CurrentTime >= LiveWidget.ReleaseTime)
		{
			NumRelased++;
			if (ensure(LiveWidget.WidgetComponent))
			{
				LiveWidget.WidgetComponent->UnregisterComponent();

				if (ensure(LiveWidget.PoolList))
				{
					LiveWidget.PoolList->Widgets.Push(LiveWidget.WidgetComponent);
				}
				else
				{
					LiveWidget.WidgetComponent->SetFlags(RF_Transient);
					LiveWidget.WidgetComponent->Rename(nullptr, GetTransientPackage(), RF_NoFlags);
				}
			}
		}
		else
		{
			// 시간에 따라 정렬되어 있어서 더 이상 찾아볼 필요가 없음
			break;
		}
	}

	LiveWidgets.RemoveAt(0, NumRelased);

	if (LiveWidgets.Num() > 0)
	{
		const float TimeUntilNextRelease = LiveWidgets[0].ReleaseTime - CurrentTime;
		LocalWorld->GetTimerManager().SetTimer(ReleaseTimerHandle, this, &ThisClass::ReleaseNextWidgets, TimeUntilNextRelease);
	}
}

FLinearColor UD1NumberPopComponent_ScreenWidget::DetermineColor(const FD1NumberPopRequest& Request)
{
	for (UD1DamagePopStyleScreenWidget* Style : Styles)
	{
		if (Style && Style->bOverrideColor)
		{
			if (Style->MatchPattern.Matches(Request.TargetTags) || Style->MatchPattern.IsEmpty())
			{
				return Request.bIsCriticalDamage ? Style->CriticalColor : Style->Color;
			}
		}
	}

	return FLinearColor::White;
}

TSubclassOf<UD1NumberPopWidgetBase> UD1NumberPopComponent_ScreenWidget::DetermineWidgetClass(const FD1NumberPopRequest& Request)
{
	for (UD1DamagePopStyleScreenWidget* Style : Styles)
	{
		if (Style && Style->bOverrideWidget)
		{
			if (Style->MatchPattern.Matches(Request.TargetTags) || Style->MatchPattern.IsEmpty())
			{
				return Style->WidgetClass;
			}
		}
	}

	return DefaultWidgetClass;
}
