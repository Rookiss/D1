#include "D1NumberPopComponent.h"

#include "D1NumberPopStyle.h"
#include "D1NumberPopWidget.h"
#include "Components/WidgetComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1NumberPopComponent)

UD1NumberPopComponent::UD1NumberPopComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1NumberPopComponent::AddNumberPop(const FNumberPopRequest& NewRequest)
{
	APlayerController* PlayerController = GetController<APlayerController>();
	if (PlayerController->IsLocalController() == false)
		return;
	
	TSubclassOf<UD1NumberPopWidget> WidgetClassToUse = DetermineWidgetClass(NewRequest);
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
	
	LiveWidgets.Emplace(ComponentToUse, &WidgetPool, GetWorld()->GetTimeSeconds() + WidgetLifeSpan);

	if (GetWorld()->GetTimerManager().IsTimerActive(ReleaseTimerHandle) == false)
	{
		GetWorld()->GetTimerManager().SetTimer(ReleaseTimerHandle, this, &ThisClass::ReleaseNextWidgets, WidgetLifeSpan);
	}

	if (UD1NumberPopWidget* NumberPopWidget = Cast<UD1NumberPopWidget>(ComponentToUse->GetWidget()))
	{
		NumberPopWidget->InitializeUI(NewRequest.NumberToDisplay, DetermineColor(NewRequest));
	}

	ComponentToUse->SetWorldLocation(NewRequest.WorldLocation);
}

void UD1NumberPopComponent::ReleaseNextWidgets()
{
	const float CurrentTime = GetWorld()->GetTimeSeconds();

	int32 NumReleased = 0;
	for (const FLiveWidgetNumberPopEntry& LiveWidget : LiveWidgets)
	{
		if (CurrentTime >= LiveWidget.ReleaseTime)
		{
			NumReleased++;
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
			break;
		}
	}

	LiveWidgets.RemoveAt(0, NumReleased);

	if (LiveWidgets.Num() > 0)
	{
		const float TimeUntilNextRelease = LiveWidgets[0].ReleaseTime - CurrentTime;
		GetWorld()->GetTimerManager().SetTimer(ReleaseTimerHandle, this, &ThisClass::ReleaseNextWidgets, TimeUntilNextRelease);
	}
}

FLinearColor UD1NumberPopComponent::DetermineColor(const FNumberPopRequest& Request)
{
	for (UD1NumberPopStyle* Style : Styles)
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

TSubclassOf<UD1NumberPopWidget> UD1NumberPopComponent::DetermineWidgetClass(const FNumberPopRequest& Request)
{
	for (UD1NumberPopStyle* Style : Styles)
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
