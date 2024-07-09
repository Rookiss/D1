#include "D1PlayerSpawningManagerComponent.h"

#include "EngineUtils.h"
#include "D1PlayerStart.h"
#include "Engine/PlayerStartPIE.h"
#include "GameFramework/PlayerState.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1PlayerSpawningManagerComponent)

UD1PlayerSpawningManagerComponent::UD1PlayerSpawningManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetIsReplicatedByDefault(false);
	bAutoRegister = true;
	bAutoActivate = true;
	bWantsInitializeComponent = true;
	PrimaryComponentTick.TickGroup = TG_PrePhysics;
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bAllowTickOnDedicatedServer = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UD1PlayerSpawningManagerComponent::InitializeComponent()
{
	Super::InitializeComponent();
	
	FWorldDelegates::LevelAddedToWorld.AddUObject(this, &ThisClass::OnLevelAdded);

	UWorld* World = GetWorld();
	World->AddOnActorSpawnedHandler(FOnActorSpawned::FDelegate::CreateUObject(this, &ThisClass::HandleOnActorSpawned));

	for (TActorIterator<AD1PlayerStart> It(World); It; ++It)
	{
		if (AD1PlayerStart* PlayerStart = *It)
		{
			CachedPlayerStarts.Add(PlayerStart);
		}
	}
}

void UD1PlayerSpawningManagerComponent::OnLevelAdded(ULevel* InLevel, UWorld* InWorld)
{
	if (InWorld == GetWorld())
	{
		for (AActor* Actor : InLevel->Actors)
		{
			if (AD1PlayerStart* PlayerStart = Cast<AD1PlayerStart>(Actor))
			{
				ensure(!CachedPlayerStarts.Contains(PlayerStart));
				CachedPlayerStarts.Add(PlayerStart);
			}
		}
	}
}

void UD1PlayerSpawningManagerComponent::HandleOnActorSpawned(AActor* SpawnedActor)
{
	if (AD1PlayerStart* PlayerStart = Cast<AD1PlayerStart>(SpawnedActor))
	{
		CachedPlayerStarts.Add(PlayerStart);
	}
}

AActor* UD1PlayerSpawningManagerComponent::ChoosePlayerStart(AController* Player)
{
	if (Player)
	{
#if WITH_EDITOR
		if (APlayerStart* PlayerStart = FindPlayFromHereStart(Player))
		{
			return PlayerStart;
		}
#endif

		TArray<AD1PlayerStart*> StarterPoints;
		for (auto StartIt = CachedPlayerStarts.CreateIterator(); StartIt; ++StartIt)
		{
			if (AD1PlayerStart* D1PlayerStart = (*StartIt).Get())
			{
				StarterPoints.Add(D1PlayerStart);
			}
			else
			{
				StartIt.RemoveCurrent();
			}
		}

		if (APlayerState* PlayerState = Player->GetPlayerState<APlayerState>())
		{
			if (PlayerState->IsOnlyASpectator())
			{
				if (StarterPoints.IsEmpty() == false)
				{
					return StarterPoints[FMath::RandRange(0, StarterPoints.Num() - 1)];
				}
				return nullptr;
			}
		}

		AActor* PlayerStart = OnChoosePlayerStart(Player, StarterPoints);

		if (PlayerStart == nullptr)
		{
			PlayerStart = GetFirstRandomUnoccupiedPlayerStart(Player, StarterPoints);
		}

		if (AD1PlayerStart* D1PlayerStart = Cast<AD1PlayerStart>(PlayerStart))
		{
			D1PlayerStart->TryClaim(Player);
		}

		return PlayerStart;
	}

	return nullptr;
}

#if WITH_EDITOR
APlayerStart* UD1PlayerSpawningManagerComponent::FindPlayFromHereStart(AController* Player)
{
	if (Player->IsA<APlayerController>())
	{
		if (UWorld* World = GetWorld())
		{
			for (TActorIterator<APlayerStart> It(World); It; ++It)
			{
				if (APlayerStart* PlayerStart = *It)
				{
					if (PlayerStart->IsA<APlayerStartPIE>())
					{
						return PlayerStart;
					}
				}
			}
		}
	}

	return nullptr;
}
#endif

bool UD1PlayerSpawningManagerComponent::ControllerCanRestart(AController* Player)
{
	return true;
}

void UD1PlayerSpawningManagerComponent::FinishRestartPlayer(AController* NewPlayer, const FRotator& StartRotation)
{
	OnFinishRestartPlayer(NewPlayer, StartRotation);
	K2_OnFinishRestartPlayer(NewPlayer, StartRotation);
}

APlayerStart* UD1PlayerSpawningManagerComponent::GetFirstRandomUnoccupiedPlayerStart(AController* Controller, const TArray<AD1PlayerStart*>& StartPoints) const
{
	if (Controller)
	{
		TArray<AD1PlayerStart*> UnOccupiedStartPoints;
		TArray<AD1PlayerStart*> OccupiedStartPoints;

		for (AD1PlayerStart* StartPoint : StartPoints)
		{
			ED1PlayerStartLocationOccupancy State = StartPoint->GetLocationOccupancy(Controller);

			switch (State)
			{
			case ED1PlayerStartLocationOccupancy::Empty:
				UnOccupiedStartPoints.Add(StartPoint);
				break;
			case ED1PlayerStartLocationOccupancy::Partial:
				OccupiedStartPoints.Add(StartPoint);
				break;
			}
		}

		if (UnOccupiedStartPoints.Num() > 0)
		{
			return UnOccupiedStartPoints[FMath::RandRange(0, UnOccupiedStartPoints.Num() - 1)];
		}
		else if (OccupiedStartPoints.Num() > 0)
		{
			return OccupiedStartPoints[FMath::RandRange(0, OccupiedStartPoints.Num() - 1)];
		}
	}

	return nullptr;
}

AActor* UD1PlayerSpawningManagerComponent::OnChoosePlayerStart(AController* Player, TArray<AD1PlayerStart*>& PlayerStarts)
{
	AD1PlayerStart* BestPlayerStart = nullptr;
	AD1PlayerStart* FallbackPlayerStart = nullptr;

	for (AD1PlayerStart* PlayerStart : PlayerStarts)
	{
		if (PlayerStart->IsClaimed() == false)
		{
			if (PlayerStart->GetLocationOccupancy(Player) < ED1PlayerStartLocationOccupancy::Full)
			{
				if (BestPlayerStart == nullptr)
				{
					BestPlayerStart = PlayerStart;
				}
			}
			else
			{
				if (FallbackPlayerStart == nullptr)
				{
					FallbackPlayerStart = PlayerStart;
				}
			}
		}
	}

	if (BestPlayerStart)
	{
		return BestPlayerStart;
	}

	return FallbackPlayerStart;
}

void UD1PlayerSpawningManagerComponent::OnFinishRestartPlayer(AController* Player, const FRotator& StartRotation)
{
	
}







