#include "D1ElectricFieldManagerComponent.h"

#include "LyraAssetManager.h"
#include "Data/D1ElectricFieldPhaseData.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1ElectricFieldManagerComponent)

UD1ElectricFieldManagerComponent::UD1ElectricFieldManagerComponent (const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1ElectricFieldManagerComponent::Initialize()
{
	TSubclassOf<AD1ElectricField> ElectricFieldClass = ULyraAssetManager::Get().GetSubclassByName<AD1ElectricField>("ElectricFieldClass");
	check(ElectricFieldClass);

	ElectricFieldActor = GetWorld()->SpawnActor<AD1ElectricField>(ElectricFieldClass);

	const UD1ElectricFieldPhaseData& PhaseData = ULyraAssetManager::Get().GetElectricFieldPhaseData();
	FD1ElectricFieldPhaseEntry PhaseEntry = PhaseData.GetPhaseEntry(CurrentPhaseIndex);
	CurrentPhaseRadius = TargetPhaseRadius = PhaseEntry.TargetRadius;

	float Scale = CurrentPhaseRadius / 100.f * 2.f;
	ElectricFieldActor->SetActorScale3D(FVector(Scale, Scale, 2000.f));
	
	NextElectricFieldPhase();
}

void UD1ElectricFieldManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (ElectricFieldActor == nullptr)
		return;
	
	if (CurrentPhaseState == ED1ElectricFieldState::Break)
	{
	// 	WaitTime--;
	//
	// 	if (WaitTime < 0)
	// 	{
	// 		if (IsValid(GameMode))
	// 			GameMode->NoticeToAll(TEXT("새로운 자기장 영역이 나타났습니다"));
	// 		CircleState = ECircleState::ShowTargetCircle;
	// 	}
	}
	else if (CurrentPhaseState == ED1ElectricFieldState::Notice)
	{
	// 	bVisibleTargetCircle = true;
	// 	ShowTime--;
	//
	// 	if (ShowTime < 0)
	// 	{
	// 		if (IsValid(GameMode))
	// 			GameMode->NoticeToAll(TEXT("자기장 영역이 줄어들기 시작합니다"));
	// 		CircleState = ECircleState::ShrinkCircle;
	// 	}
	}
	else if (CurrentPhaseState == ED1ElectricFieldState::Shrink)
	{
	// 	bVisibleCurrCircle = true;
	// 	ShrinkTime--;
	//
	// 	if (ShrinkTime >= 0)
	// 	{
	// 		CurrCirclePos += DeltaPosPerSec;
	// 		CurrCircleRadius += DeltaRadiusPerSec;
	// 	}
	// 	else
	// 	{
	// 		bVisibleTargetCircle = false;
	// 		
	// 		CurrPhase++;
	// 		if (CurrPhase > GameInstance->GetElectricFieldPhaseRowCount())
	// 		{
	// 			CircleState = ECircleState::End;
	// 			return;
	// 		}
	//
	// 		SetElectricFieldPhase();
	// 		CircleState = ECircleState::Wait;
	// 	}
	}
}

void UD1ElectricFieldManagerComponent::NextElectricFieldPhase()
{
	CurrentPhaseIndex++;
	
	const UD1ElectricFieldPhaseData& ElectricFieldPhaseData = ULyraAssetManager::Get().GetElectricFieldPhaseData();
	CurrentPhaseEntry = ElectricFieldPhaseData.GetPhaseEntry(CurrentPhaseIndex);
	
	FVector RandDirection = FMath::VRand();
	RandDirection = FVector(RandDirection.X, RandDirection.Y, 0.f).GetSafeNormal();
	
	float MaxLength = CurrentPhaseRadius - CurrentPhaseEntry.TargetRadius;
	float RandLength = FMath::RandRange(0.f, MaxLength);
	
	TargetPhasePosition = RandDirection * RandLength + CurrentPhasePosition;
}
