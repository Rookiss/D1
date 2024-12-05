#include "D1GameplayAbility_Weapon_Bow_Projectile.h"

#include "Actors/D1EquipmentBase.h"
#include "Character//LyraCharacter.h"
#include "Actors/D1ProjectileBase.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Physics/LyraCollisionChannels.h"
#include "Player/LyraPlayerController.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1GameplayAbility_Weapon_Bow_Projectile)

UD1GameplayAbility_Weapon_Bow_Projectile::UD1GameplayAbility_Weapon_Bow_Projectile(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1GameplayAbility_Weapon_Bow_Projectile::SpawnProjectile()
{
	if (HasAuthority(&CurrentActivationInfo) == false)
		return;
	
	AD1EquipmentBase* WeaponActor = GetFirstEquipmentActor();
	if (WeaponActor == nullptr)
		return;
	
	ALyraCharacter* LyraCharacter = GetLyraCharacterFromActorInfo();
	ALyraPlayerController* LyraPlayerController = GetLyraPlayerControllerFromActorInfo();
		
	if (LyraCharacter && LyraPlayerController)
	{
		FTransform SocketTransform = LyraCharacter->GetMesh()->GetSocketTransform(SpawnSocketName, RTS_World);
		FVector SocketLocation = SocketTransform.GetLocation();
		FRotator SocketRotation = SocketTransform.GetRotation().Rotator();
			
		FVector CameraLocation;
		FRotator CameraRotation;
		LyraPlayerController->GetPlayerViewPoint(CameraLocation, CameraRotation);

		FTransform SpawnTransform;
		if (bApplyAimAssist)
		{
			float Distance = (SocketLocation - CameraLocation).Dot(CameraRotation.Vector());
			FVector StartLocation = CameraLocation + CameraRotation.Vector() * (Distance + AimAssistMinDistance);
			FVector EndLocation = StartLocation + (CameraRotation.Vector() * AimAssistMaxDistance);
			
			FHitResult HitResult;
			TArray<AActor*> ActorsToIgnore = { LyraCharacter, WeaponActor };

			bool bHit = UKismetSystemLibrary::LineTraceSingle(GetWorld(), StartLocation, EndLocation, UEngineTypes::ConvertToTraceType(D1_TraceChannel_AimAssist), false, ActorsToIgnore, EDrawDebugTrace::None, HitResult, true);
			SocketRotation = bHit ? (HitResult.ImpactPoint - SocketLocation).Rotation() : (EndLocation - SocketLocation).Rotation();
				
			SpawnTransform.SetLocation(SocketLocation);
			SpawnTransform.SetRotation(SocketRotation.Quaternion());
		}
		else
		{
			SpawnTransform.SetLocation(SocketLocation);
			SpawnTransform.SetRotation(CameraRotation.Quaternion());
		}
			
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnParams.Owner = WeaponActor;
		SpawnParams.Instigator = LyraCharacter;
			
		GetWorld()->SpawnActor<AD1ProjectileBase>(ProjectileClass, SpawnTransform, SpawnParams);
	}
}
