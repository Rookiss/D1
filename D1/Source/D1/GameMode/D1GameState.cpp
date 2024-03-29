#include "D1GameState.h"

#include "GameFramework/PlayerState.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1GameState)

AD1GameState::AD1GameState(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}

void AD1GameState::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	if (HasAuthority() && PlayerArray.Num() > 0)
	{
		float PingSum = 0.f;
	
		for (APlayerState* PlayerState : PlayerArray)
		{
			PingSum += PlayerState->GetPingInMilliseconds();
		}

		AvgPing = PingSum / PlayerArray.Num() / 1000.f;
	}
}

void AD1GameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, AvgPing);
}
