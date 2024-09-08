#include "D1GameplayAbility_Weapon_Bow_Shoot.h"

#include "D1GameplayTags.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Actors/D1ProjectileBase.h"
#include "Actors/D1WeaponBase.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Physics/LyraCollisionChannels.h"
#include "Player/LyraPlayerController.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1GameplayAbility_Weapon_Bow_Shoot)

UD1GameplayAbility_Weapon_Bow_Shoot::UD1GameplayAbility_Weapon_Bow_Shoot(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1GameplayAbility_Weapon_Bow_Shoot::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	// if (K2_CheckAbilityCost() == false || K2_CheckAbilityCooldown() == false)
	// {
	// 	K2_EndAbility();
	// 	return;
	// }
	//
	// CommitAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo);
	//
	// SpawnProjectile();
	//
	// ReleaseMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, ReleaseMontage, GetAttackRate(), NAME_None, true);
	// ReleaseMontageTask->OnCompleted.AddDynamic(this, &ThisClass::OnMontageFinished);
	// ReleaseMontageTask->OnBlendOut.AddDynamic(this, &ThisClass::OnMontageFinished);
	// ReleaseMontageTask->OnInterrupted.AddDynamic(this, &ThisClass::OnMontageFinished);
	// ReleaseMontageTask->OnCancelled.AddDynamic(this, &ThisClass::OnMontageFinished);
	// ReleaseMontageTask->ReadyForActivation();
	
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UD1GameplayAbility_Weapon_Bow_Shoot::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	// if (IsValid(ReleaseMontageTask))
	// {
	// 	ReleaseMontageTask->EndTask();
	// }
	//
	// FGameplayEventData EventData;
	// EventData.EventMagnitude = 1.f;
	// SendGameplayEvent(D1GameplayTags::GameplayEvent_ArrowVisibility, EventData);
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UD1GameplayAbility_Weapon_Bow_Shoot::SpawnProjectile()
{
	if (K2_HasAuthority())
	{
		ALyraCharacter* LyraCharacter = GetLyraCharacterFromActorInfo();
		ALyraPlayerController* LyraPlayerController = GetLyraPlayerControllerFromActorInfo();
		
		if (LyraCharacter && LyraPlayerController)
		{
			FTransform SocketTransform = LyraCharacter->GetMesh()->GetSocketTransform(SpawnSocketName, RTS_World);
			FVector SpawnLocation = SocketTransform.GetLocation();
			FRotator SpawnRotation = SocketTransform.GetRotation().Rotator();
			
			FVector CameraLocation;
			FRotator CameraRotation;
			LyraPlayerController->GetPlayerViewPoint(CameraLocation, CameraRotation);

			float Distance = (SpawnLocation - CameraLocation).Dot(CameraRotation.Vector());
			FVector StartLocation = CameraLocation + CameraRotation.Vector() * (Distance + AimAssistMinDistance);
			FVector EndLocation = StartLocation + (CameraRotation.Vector() * AimAssistMaxDistance);
			
			FHitResult HitResult;
			TArray<AActor*> ActorsToIgnore = { LyraCharacter, WeaponActor };

			bool bHit = UKismetSystemLibrary::LineTraceSingle(GetWorld(), StartLocation, EndLocation, UEngineTypes::ConvertToTraceType(D1_TraceChannel_AimAssist), false, ActorsToIgnore, EDrawDebugTrace::None, HitResult, true);
			SpawnRotation = bHit ? (HitResult.ImpactPoint - SpawnLocation).Rotation() : (EndLocation - SpawnLocation).Rotation();

			FTransform SpawnTransform;
			SpawnTransform.SetLocation(SpawnLocation);
			SpawnTransform.SetRotation(SpawnRotation.Quaternion());
			
			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			SpawnParams.Owner = WeaponActor;
			SpawnParams.Instigator = LyraCharacter;
			
			GetWorld()->SpawnActor<AD1ProjectileBase>(ProjectileClass, SpawnTransform, SpawnParams);
		}
	}
}

void UD1GameplayAbility_Weapon_Bow_Shoot::OnMontageFinished()
{
	K2_EndAbility();
}
