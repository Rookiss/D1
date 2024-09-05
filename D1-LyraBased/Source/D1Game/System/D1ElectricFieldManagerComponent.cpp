#include "D1ElectricFieldManagerComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "LyraAssetManager.h"
#include "Camera/CameraComponent.h"
#include "Camera/LyraCameraComponent.h"
#include "Character/LyraCharacter.h"
#include "Data/D1ElectricFieldPhaseData.h"
#include "GameModes/LyraGameState.h"
#include "Messages/LyraVerbMessage.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1ElectricFieldManagerComponent)

UD1ElectricFieldManagerComponent::UD1ElectricFieldManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
	SetIsReplicatedByDefault(true);
}

void UD1ElectricFieldManagerComponent::Initialize()
{
#if WITH_SERVER_CODE
	TSubclassOf<AD1ElectricField> ElectricFieldClass = ULyraAssetManager::Get().GetSubclassByName<AD1ElectricField>("ElectricFieldClass");
	check(ElectricFieldClass);

	ALyraGameState* LyraGameState = GetGameState<ALyraGameState>();
	if (LyraGameState == nullptr)
		return;

	ElectricFieldActor = GetWorld()->SpawnActor<AD1ElectricField>(ElectricFieldClass);
	StartPhasePosition = LyraGameState->TargetPhasePosition = FVector(0.f, 0.f, -50.f * ElectricFieldActor->GetActorScale3D().Z);
	ElectricFieldActor->SetActorLocation(StartPhasePosition);

	const UD1ElectricFieldPhaseData& PhaseData = ULyraAssetManager::Get().GetElectricFieldPhaseData();
	FD1ElectricFieldPhaseEntry PhaseEntry = PhaseData.GetPhaseEntry(CurrentPhaseIndex);
	StartPhaseRadius = LyraGameState->TargetPhaseRadius = PhaseEntry.TargetRadius;

	float Scale = StartPhaseRadius / 50.f;
	ElectricFieldActor->SetActorScale3D(FVector(Scale, Scale, ElectricFieldActor->GetActorScale3D().Z));
	
	SetupNextElectricFieldPhase();
#endif
}

void UD1ElectricFieldManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

#if WITH_SERVER_CODE
	if (IsValid(ElectricFieldActor) == false)
	{
		RemoveAllDamageEffects();
		return;
	}

	// TODO: Use TimerManager & Interp instead of Tick
	ALyraGameState* LyraGameState = GetGameState<ALyraGameState>();
	if (LyraGameState == nullptr)
		return;
	
	if (CurrentPhaseState == ED1ElectricFieldState::Break)
	{
		CurrentPhaseEntry.BreakTime -= DeltaTime;
		if (CurrentPhaseEntry.BreakTime < 0.f)
		{
			CurrentPhaseState = ED1ElectricFieldState::Notice;
		}
	}
	else if (CurrentPhaseState == ED1ElectricFieldState::Notice)
	{
		LyraGameState->bShouldShow = true;
		CurrentPhaseEntry.NoticeTime -= DeltaTime;
		if (CurrentPhaseEntry.NoticeTime < 0.f)
		{
			CurrentPhaseState = ED1ElectricFieldState::Shrink;
		}
	}
	else if (CurrentPhaseState == ED1ElectricFieldState::Shrink)
	{
		CurrentPhaseEntry.ShrinkTime -= DeltaTime;
	
		if (CurrentPhaseEntry.ShrinkTime > 0.f)
		{
			float Alpha = 1.f - (CurrentPhaseEntry.ShrinkTime / CachedPhaseEntry.ShrinkTime);
			
			FVector Position = FMath::Lerp(StartPhasePosition, LyraGameState->TargetPhasePosition, Alpha);
			ElectricFieldActor->SetActorLocation(Position);

			float Radius = FMath::Lerp(StartPhaseRadius, LyraGameState->TargetPhaseRadius, Alpha);
			float Scale = Radius / 50.f;
			ElectricFieldActor->SetActorScale3D(FVector(Scale, Scale, ElectricFieldActor->GetActorScale3D().Z));
		}
		else
		{
			LyraGameState->bShouldShow = false;
			if (SetupNextElectricFieldPhase())
			{
				CurrentPhaseState = ED1ElectricFieldState::Break;
			}
			else
			{
				CurrentPhaseState = ED1ElectricFieldState::End;
			}
		}
	}

	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerController* PlayerController = Iterator->Get();
		if (PlayerController && PlayerController->PlayerState)
		{
			if (ALyraCharacter* LyraCharacter = Cast<ALyraCharacter>(PlayerController->GetPawn()))
			{
				if (UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(LyraCharacter))
				{
					float Length = (FVector2D(LyraCharacter->GetActorLocation()) - FVector2D(ElectricFieldActor->GetActorLocation())).Length();
					if (Length > ElectricFieldActor->GetActorScale3D().X * 50.f)
					{
						if (OutSideCharacters.Contains(LyraCharacter) == false)
						{
							TSubclassOf<UGameplayEffect> ElectricFieldDamageClass = ULyraAssetManager::GetSubclassByName<UGameplayEffect>("ElectricFieldDamageClass");
							FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(ElectricFieldDamageClass, 1.0f, ASC->MakeEffectContext());
							FGameplayEffectSpec* Spec = SpecHandle.Data.Get();
							FActiveGameplayEffectHandle Handle = ASC->ApplyGameplayEffectSpecToSelf(*Spec);

							OutSideCharacters.Add(LyraCharacter, Handle);
						}
					}
					else
					{
						if (FActiveGameplayEffectHandle* HandlePtr = OutSideCharacters.Find(LyraCharacter))
						{
							ASC->RemoveActiveGameplayEffect(*HandlePtr);
							OutSideCharacters.Remove(LyraCharacter);
						}
					}
				}
				else
				{
					OutSideCharacters.Remove(LyraCharacter);
				}
			}
		}
	}
#endif
}

bool UD1ElectricFieldManagerComponent::SetupNextElectricFieldPhase()
{
#if WITH_SERVER_CODE
	ALyraGameState* LyraGameState = GetGameState<ALyraGameState>();
	if (LyraGameState == nullptr)
		return false;
	
	CurrentPhaseIndex++;
	
	const UD1ElectricFieldPhaseData& ElectricFieldPhaseData = ULyraAssetManager::Get().GetElectricFieldPhaseData();
	if (ElectricFieldPhaseData.IsValidPhaseIndex(CurrentPhaseIndex) == false)
		return false;

	CachedPhaseEntry = CurrentPhaseEntry = ElectricFieldPhaseData.GetPhaseEntry(CurrentPhaseIndex);

	StartPhaseRadius = LyraGameState->TargetPhaseRadius;
	LyraGameState->TargetPhaseRadius = CachedPhaseEntry.TargetRadius;

	if (CurrentPhaseIndex > 1)
	{
		FVector RandDirection = FMath::VRand();
		RandDirection = FVector(RandDirection.X, RandDirection.Y, 0.f).GetSafeNormal();
	
		float MaxLength = LyraGameState->TargetPhaseRadius - StartPhaseRadius;
		float RandLength = FMath::RandRange(0.f, MaxLength);

		StartPhasePosition = LyraGameState->TargetPhasePosition;
		LyraGameState->TargetPhasePosition = StartPhasePosition + (RandDirection * RandLength);
	}
#endif
	
	return true;
}

void UD1ElectricFieldManagerComponent::RemoveAllDamageEffects()
{
	if (OutSideCharacters.Num() <= 0)
		return;
	
	for (auto& OutSideCharacter : OutSideCharacters)
	{
		if (ALyraCharacter* LyraCharacter = OutSideCharacter.Key.Get())
		{
			if (UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(LyraCharacter))
			{
				ASC->RemoveActiveGameplayEffect(OutSideCharacter.Value);
			}
		}
	}

	OutSideCharacters.Reset();
}
