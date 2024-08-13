#include "D1GameplayAbility_Utility_Throw.h"

#include "Character/LyraCharacter.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1GameplayAbility_Utility_Throw)

UD1GameplayAbility_Utility_Throw::UD1GameplayAbility_Utility_Throw(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

FVector UD1GameplayAbility_Utility_Throw::GetSpawnLocation()
{
	FVector SpawnLocation;
	
	if (ALyraCharacter* LyraCharacter = GetLyraCharacterFromActorInfo())
	{
		FVector SocketLocation = LyraCharacter->GetMesh()->GetSocketLocation("head");
		FVector ForwardOffset = LyraCharacter->GetActorForwardVector() * 5.f;
		FVector UpOffset = LyraCharacter->GetActorUpVector() * 10.f;
		FVector RightOffset = LyraCharacter->GetActorRightVector() * 25.f;

		return SocketLocation + ForwardOffset + UpOffset + RightOffset;
	}
	else
	{
		SpawnLocation = GetAvatarActorFromActorInfo()->GetActorLocation();
	}

	return SpawnLocation;
}

FRotator UD1GameplayAbility_Utility_Throw::GetSpawnRotation()
{
	return FRotator();
}
