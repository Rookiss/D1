#include "D1GameplayAbility_Weapon_GroundBreaker.h"

#include "D1LogChannels.h"
#include "Character/LyraCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1GameplayAbility_Weapon_GroundBreaker)

UD1GameplayAbility_Weapon_GroundBreaker::UD1GameplayAbility_Weapon_GroundBreaker(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1GameplayAbility_Weapon_GroundBreaker::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	CachedHitCharacters.Reset();
	
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UD1GameplayAbility_Weapon_GroundBreaker::Execute()
{
	ALyraCharacter* LyraCharacter = GetLyraCharacterFromActorInfo();
	if (LyraCharacter == nullptr)
		return;

	float ScaledCapsuleRadius = LyraCharacter->GetCapsuleComponent()->GetScaledCapsuleRadius();
	float ScaledCapsuleHalfHeight = LyraCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	
	FVector Start = LyraCharacter->GetActorLocation() + (LyraCharacter->GetActorForwardVector() * Distance);
	FVector End = Start;
	FVector HalfSize = FVector(ScaledCapsuleRadius * 3.f, ScaledCapsuleRadius * 3.f, ScaledCapsuleHalfHeight);;
	FRotator Orientation = UKismetMathLibrary::MakeRotFromX(LyraCharacter->GetActorForwardVector());
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes = { UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn) };
	TArray<AActor*> ActorsToIgnore = { LyraCharacter };
	
	TArray<FHitResult> OutHitResults;
	if (UKismetSystemLibrary::BoxTraceMultiForObjects(GetWorld(), Start, End, HalfSize, Orientation, ObjectTypes, false, ActorsToIgnore, EDrawDebugTrace::ForDuration, OutHitResults, true))
	{
		for (const FHitResult& HitResult : OutHitResults)
		{
			LOG_SCREEN(TEXT("Hit: %s"), *HitResult.GetActor()->GetName());
			
			ALyraCharacter* HitCharacter = Cast<ALyraCharacter>(HitResult.GetActor());
			if (HitCharacter == nullptr || CachedHitCharacters.Contains(HitCharacter))
				continue;

			CachedHitCharacters.Add(HitCharacter);

			// MakeOutgoingGameplayEffectSpec()
		}
	}
}
