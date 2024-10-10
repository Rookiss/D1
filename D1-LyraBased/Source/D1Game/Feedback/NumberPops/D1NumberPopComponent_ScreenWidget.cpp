#include "D1NumberPopComponent_ScreenWidget.h"

#include "D1DamagePopStyleScreenWidget.h"
#include "Blueprint/SlateBlueprintLibrary.h"
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

	UD1NumberPopWidgetBase* WidgetToUse;
	
	if (WidgetPool.Widgets.Num() > 0)
	{
		WidgetToUse = WidgetPool.Widgets.Pop();
	}
	else
	{
		UWorld* LocalWorld = GetWorld();
		check(LocalWorld);
		
		WidgetToUse = CreateWidget<UD1NumberPopWidgetBase>(LocalWorld, WidgetClassToUse);
		LiveWidgets.Emplace(WidgetToUse, &WidgetPool, LocalWorld->GetTimeSeconds() + WidgetLifeSpan);

		if (LocalWorld->GetTimerManager().IsTimerActive(ReleaseTimerHandle) == false)
		{
			LocalWorld->GetTimerManager().SetTimer(ReleaseTimerHandle, this, &ThisClass::ReleaseNextWidgets, WidgetLifeSpan);
		}
	}

	if (WidgetToUse)
	{
		WidgetToUse->InitializeUI(NewRequest.NumberToDisplay, DetermineColor(NewRequest));
		WidgetToUse->AddToViewport();
		
		FVector2D ScreenPosition;
		PlayerController->ProjectWorldLocationToScreen(NewRequest.WorldLocation, ScreenPosition, false);
		
		FVector2D AbsolutePosition;
		USlateBlueprintLibrary::ScreenToWidgetAbsolute(WidgetToUse, ScreenPosition, AbsolutePosition, false);
		
		FVector2D PixelPosition, ViewportPosition;
		USlateBlueprintLibrary::AbsoluteToViewport(WidgetToUse, AbsolutePosition, PixelPosition, ViewportPosition);

		WidgetToUse->SetPositionInViewport(ViewportPosition, false);
	}
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
			if (ensure(LiveWidget.Widget))
			{
				LiveWidget.Widget->RemoveFromParent();

				if (ensure(LiveWidget.PoolList))
				{
					LiveWidget.PoolList->Widgets.Push(LiveWidget.Widget);
				}
				else
				{
					LiveWidget.Widget->SetFlags(RF_Transient);
					LiveWidget.Widget->Rename(nullptr, GetTransientPackage(), RF_NoFlags);
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
				return Style->TextWidget;
			}
		}
	}

	return DefaultWidgetClass;
}
