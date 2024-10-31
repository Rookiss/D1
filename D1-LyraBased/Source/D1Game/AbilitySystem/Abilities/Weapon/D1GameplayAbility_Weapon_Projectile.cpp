#include "D1GameplayAbility_Weapon_Projectile.h"

#include "AbilitySystemComponent.h"
#include "Actors/D1ProjectileBase.h"
#include "Actors/D1WeaponBase.h"
#include "Character/LyraCharacter.h"
#include "Player/LyraPlayerController.h"
#include "System/LyraGameData.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1GameplayAbility_Weapon_Projectile)

UD1GameplayAbility_Weapon_Projectile::UD1GameplayAbility_Weapon_Projectile(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	Phase2_ProjectileClasses.SetNum(3);
	Phase3_ProjectileClasses.SetNum(3);
}

void UD1GameplayAbility_Weapon_Projectile::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	CurrentIndex = 0;
	
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UD1GameplayAbility_Weapon_Projectile::SpawnProjectile()
{
	if (HasAuthority(&CurrentActivationInfo) == false)
		return;
	
	AD1WeaponBase* WeaponActor = GetFirstWeaponActor();
	if (WeaponActor == nullptr)
		return;
	
	USkeletalMeshComponent* WeaponMeshComponent = WeaponActor->WeaponMeshComponent;
	if (WeaponMeshComponent == nullptr)
		return;

	ALyraPlayerController* LyraPlayerController = GetLyraPlayerControllerFromActorInfo();
	if (LyraPlayerController == nullptr)
		return;

#if UE_EDITOR
	ensure(WeaponMeshComponent->DoesSocketExist(ProjectileSocketName));
#endif
	
	if (WeaponMeshComponent->DoesSocketExist(ProjectileSocketName) == false)
		return;

	FVector CameraLocation;
	FRotator CameraRotation;
	LyraPlayerController->GetPlayerViewPoint(CameraLocation, CameraRotation);
	
	FVector CameraForward = CameraRotation.Vector();
	FVector WeaponSocketLocation = WeaponMeshComponent->GetSocketLocation(ProjectileSocketName);
	float Distance = CameraForward.Dot(WeaponSocketLocation - CameraLocation);
	
	FTransform SpawnTransform_Left;
	SpawnTransform_Left.SetLocation(CameraLocation + CameraForward * Distance);
	SpawnTransform_Left.SetRotation(CameraRotation.Quaternion() * FRotator(0.f, -DeltaDegree, 0.f).Quaternion());

	FTransform SpawnTransform_Center;
	SpawnTransform_Center.SetLocation(CameraLocation + CameraForward * Distance);
	SpawnTransform_Center.SetRotation(CameraRotation.Quaternion());
	
	FTransform SpawnTransform_Right;
	SpawnTransform_Right.SetLocation(CameraLocation + CameraForward * Distance);
	SpawnTransform_Right.SetRotation(CameraRotation.Quaternion() * FRotator(0.f, DeltaDegree, 0.f).Quaternion());

	TArray<FTransform> SpawnTransforms = { SpawnTransform_Left, SpawnTransform_Center, SpawnTransform_Right };
	
	TArray<TSubclassOf<AD1ProjectileBase>> SelectedProjectileClasses;
	SelectedProjectileClasses.SetNumZeroed(3);
	
	if (CurrentIndex == 0)
	{
		SelectedProjectileClasses[1] = Phase1_ProjectileClass;
	}
	else if (CurrentIndex == 1)
	{
		SelectedProjectileClasses = Phase2_ProjectileClasses;
	}
	else if (CurrentIndex == 2)
	{
		SelectedProjectileClasses = Phase3_ProjectileClasses;
	}

	for (int i = 0; i < SelectedProjectileClasses.Num(); i++)
	{
		const TSubclassOf<AD1ProjectileBase>& ProjectileClass = SelectedProjectileClasses[i];
		if (ProjectileClass == nullptr)
			continue;

		const FTransform& SpawnTransform = SpawnTransforms[i];
		
		AD1ProjectileBase* Projectile = GetWorld()->SpawnActorDeferred<AD1ProjectileBase>(
			ProjectileClass,
			SpawnTransform,
			GetAvatarActorFromActorInfo(),
			Cast<APawn>(GetAvatarActorFromActorInfo()),
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn
		);
	
		Projectile->FinishSpawning(SpawnTransform);
	}
}
