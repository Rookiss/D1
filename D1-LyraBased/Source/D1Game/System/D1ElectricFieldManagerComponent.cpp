#include "D1ElectricFieldManagerComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "LyraAssetManager.h"
#include "Character/LyraCharacter.h"
#include "Data/D1ElectricFieldPhaseData.h"
#include "GameModes/LyraGameState.h"
#include "Messages/LyraVerbMessage.h"
#include "Net/UnrealNetwork.h"

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
	if (HasAuthority() == false)
		return;
	
#if WITH_SERVER_CODE
	TSubclassOf<AD1ElectricField> ElectricFieldClass = ULyraAssetManager::Get().GetSubclassByName<AD1ElectricField>("ElectricFieldClass");
	check(ElectricFieldClass);

	ALyraGameState* LyraGameState = GetGameState<ALyraGameState>();
	if (LyraGameState == nullptr)
		return;

	ElectricFieldActor = GetWorld()->SpawnActor<AD1ElectricField>(ElectricFieldClass);
	StartPhasePosition = TargetPhasePosition = FVector(0.f, 0.f, -50.f * ElectricFieldActor->GetActorScale3D().Z);
	ElectricFieldActor->SetActorLocation(StartPhasePosition);

	const UD1ElectricFieldPhaseData& PhaseData = ULyraAssetManager::Get().GetElectricFieldPhaseData();
	FD1ElectricFieldPhaseEntry PhaseEntry = PhaseData.GetPhaseEntry(CurrentPhaseIndex);
	StartPhaseRadius = TargetPhaseRadius = PhaseEntry.TargetRadius;

	float Scale = StartPhaseRadius / 50.f;
	ElectricFieldActor->SetActorScale3D(FVector(Scale, Scale, ElectricFieldActor->GetActorScale3D().Z));
    	
	SetupNextElectricFieldPhase();
	
	GetWorldTimerManager().SetTimer(OverlapCheckTimerHandle, this, &ThisClass::CheckCharacterOverlap, OverlapCheckInterval, true);
#endif
}

void UD1ElectricFieldManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (HasAuthority() == false)
		return;
	
#if WITH_SERVER_CODE
	if (IsValid(ElectricFieldActor) == false)
	{
		RemoveAllDamageEffects();
		return;
	}
	
	ALyraGameState* LyraGameState = GetGameState<ALyraGameState>();
	if (LyraGameState == nullptr)
		return;
	
	if (CurrentPhaseState == ED1ElectricFieldState::Break)
	{
		RemainTime -= DeltaTime;
		
		if (RemainTime < 0.f)
		{
			RemainTime = CurrentPhaseEntry.NoticeTime;
			CurrentPhaseState = ED1ElectricFieldState::Notice;
		}
	}
	else if (CurrentPhaseState == ED1ElectricFieldState::Notice)
	{
		bShouldShow = true;
		RemainTime -= DeltaTime;
		
		if (RemainTime < 0.f)
		{
			RemainTime = CurrentPhaseEntry.ShrinkTime;
			CurrentPhaseState = ED1ElectricFieldState::Shrink;
		}
	}
	else if (CurrentPhaseState == ED1ElectricFieldState::Shrink)
	{
		RemainTime -= DeltaTime;
	
		if (RemainTime > 0.f)
		{
			float Alpha = 1.f - (CurrentPhaseEntry.ShrinkTime / RemainTime);
			
			FVector Position = FMath::Lerp(StartPhasePosition, TargetPhasePosition, Alpha);
			ElectricFieldActor->SetActorLocation(Position);

			float Radius = FMath::Lerp(StartPhaseRadius, TargetPhaseRadius, Alpha);
			float Scale = Radius / 50.f;
			ElectricFieldActor->SetActorScale3D(FVector(Scale, Scale, ElectricFieldActor->GetActorScale3D().Z));
		}
		else
		{
			bShouldShow = false;
			CurrentPhaseState = SetupNextElectricFieldPhase() ? ED1ElectricFieldState::Break : ED1ElectricFieldState::End;
		}
	}

	RemainTimeSeconds = FMath::RoundToInt(FMath::Max(0.f, RemainTime));
#endif
}

void UD1ElectricFieldManagerComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, TargetPhaseRadius);
	DOREPLIFETIME(ThisClass, TargetPhasePosition);
	DOREPLIFETIME(ThisClass, bShouldShow);
	DOREPLIFETIME(ThisClass, RemainTimeSeconds);
}

void UD1ElectricFieldManagerComponent::CheckCharacterOverlap()
{
	float FieldSquardLength = FMath::Pow(ElectricFieldActor->GetActorScale3D().X * 50.f, 2);
	
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerController* PlayerController = Iterator->Get();
		if (PlayerController && PlayerController->PlayerState)
		{
			if (ALyraCharacter* LyraCharacter = Cast<ALyraCharacter>(PlayerController->GetPawn()))
			{
				if (UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(LyraCharacter))
				{
					float DistanceSquared = (FVector2D(LyraCharacter->GetActorLocation()) - FVector2D(ElectricFieldActor->GetActorLocation())).SquaredLength();
					if (DistanceSquared > FieldSquardLength)
					{
						if (OutsideCharacters.Contains(LyraCharacter) == false)
						{
							TSubclassOf<UGameplayEffect> ElectricFieldDamageClass = ULyraAssetManager::GetSubclassByName<UGameplayEffect>("ElectricFieldDamageClass");
							FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(ElectricFieldDamageClass, 1.0f, ASC->MakeEffectContext());
							FGameplayEffectSpec* Spec = SpecHandle.Data.Get();
							FActiveGameplayEffectHandle Handle = ASC->ApplyGameplayEffectSpecToSelf(*Spec);

							OutsideCharacters.Add(LyraCharacter, Handle);
						}
					}
					else
					{
						if (FActiveGameplayEffectHandle* HandlePtr = OutsideCharacters.Find(LyraCharacter))
						{
							ASC->RemoveActiveGameplayEffect(*HandlePtr);
							OutsideCharacters.Remove(LyraCharacter);
						}
					}
				}
				else
				{
					OutsideCharacters.Remove(LyraCharacter);
				}
			}
		}
	}
}

bool UD1ElectricFieldManagerComponent::SetupNextElectricFieldPhase()
{
	if (HasAuthority() == false)
		return false;
	
#if WITH_SERVER_CODE
	ALyraGameState* LyraGameState = GetGameState<ALyraGameState>();
	if (LyraGameState == nullptr)
		return false;
	
	CurrentPhaseIndex++;
	
	const UD1ElectricFieldPhaseData& ElectricFieldPhaseData = ULyraAssetManager::Get().GetElectricFieldPhaseData();
	if (ElectricFieldPhaseData.IsValidPhaseIndex(CurrentPhaseIndex) == false)
		return false;

	CurrentPhaseEntry = ElectricFieldPhaseData.GetPhaseEntry(CurrentPhaseIndex);
	RemainTime = CurrentPhaseEntry.BreakTime;
	
	StartPhaseRadius = TargetPhaseRadius;
	TargetPhaseRadius = CurrentPhaseEntry.TargetRadius;

	if (CurrentPhaseIndex > 1)
	{
		FVector RandDirection = FMath::VRand();
		RandDirection = FVector(RandDirection.X, RandDirection.Y, 0.f).GetSafeNormal();
	
		float MaxLength = TargetPhaseRadius - StartPhaseRadius;
		float RandLength = FMath::RandRange(0.f, MaxLength);

		StartPhasePosition = TargetPhasePosition;
		TargetPhasePosition = StartPhasePosition + (RandDirection * RandLength);
	}
#endif
	
	return true;
}

void UD1ElectricFieldManagerComponent::RemoveAllDamageEffects()
{
	if (HasAuthority() == false)
		return;
	
#if WITH_SERVER_CODE
	if (OutsideCharacters.Num() <= 0)
		return;
	
	for (auto& OutsideCharacter : OutsideCharacters)
	{
		if (ALyraCharacter* LyraCharacter = OutsideCharacter.Key.Get())
		{
			if (UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(LyraCharacter))
			{
				ASC->RemoveActiveGameplayEffect(OutsideCharacter.Value);
			}
		}
	}
	OutsideCharacters.Reset();
#endif
}
