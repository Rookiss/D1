#include "D1DamageScreenWidget.h"

#include "Animation/UMGSequencePlayer.h"
#include "Character/LyraCharacter.h"
#include "Character/LyraHealthComponent.h"
#include "Components/Image.h"
#include "GameFramework/GameplayMessageSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1DamageScreenWidget)

UD1DamageScreenWidget::UD1DamageScreenWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1DamageScreenWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ALyraCharacter* LyraCharacter = Cast<ALyraCharacter>(GetOwningPlayerPawn());
	if (LyraCharacter == nullptr)
		return;
	
	ULyraHealthComponent* HealthComponent = LyraCharacter->GetComponentByClass<ULyraHealthComponent>();
	if (HealthComponent == nullptr)
		return;

	HealthComponent->OnHealthChanged.AddDynamic(this, &ThisClass::OnHealthChanged);
}

void UD1DamageScreenWidget::NativeDestruct()
{
	ALyraCharacter* LyraCharacter = Cast<ALyraCharacter>(GetOwningPlayerPawn());
	if (LyraCharacter == nullptr)
		return;
	
	ULyraHealthComponent* HealthComponent = LyraCharacter->GetComponentByClass<ULyraHealthComponent>();
	if (HealthComponent == nullptr)
		return;

	HealthComponent->OnHealthChanged.RemoveDynamic(this, &ThisClass::OnHealthChanged);
	
	Super::NativeDestruct();
}

void UD1DamageScreenWidget::OnHealthChanged(ULyraHealthComponent* HealthComponent, float OldValue, float NewValue, AActor* Instigator)
{
	if (OldValue > NewValue)
	{
		if (SequencePlayer && SequencePlayer->GetPlaybackStatus() == EMovieScenePlayerStatus::Playing)
		{
			if (SequencePlayer->IsPlayingForward() == false)
			{
				SequencePlayer->Reverse();
				SetTimerPlayReverse();
			}
			return;
		}
		
		if (GetWorld()->GetTimerManager().TimerExists(ReverseTimerHandle))
		{
			SetTimerPlayReverse();
			return;
		}

		int32 SelectedIndex = FMath::RandRange(0, ScreenMaterials.Num() - 1);
		if (ScreenMaterials.IsValidIndex(SelectedIndex) == false)
			return;
		
		UMaterialInterface* SelectedMaterial = ScreenMaterials[SelectedIndex];
		Image_Blood->SetBrushFromMaterial(SelectedMaterial);
		
		PlayForward();
	}
}

void UD1DamageScreenWidget::PlayForward()
{
	SequencePlayer = PlayAnimationForward(Animation_FadeIn);
	SetTimerPlayReverse();
}

void UD1DamageScreenWidget::PlayReverse()
{
	SequencePlayer = PlayAnimationReverse(Animation_FadeIn);
}

void UD1DamageScreenWidget::SetTimerPlayReverse()
{
	GetWorld()->GetTimerManager().SetTimer(ReverseTimerHandle, this, &ThisClass::PlayReverse, WaitDuration, false);
}
