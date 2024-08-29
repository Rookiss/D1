#include "D1GameplayAbility_Weapon_Projectile.h"

#include "AbilitySystemComponent.h"
#include "Actors/D1ProjectileBase.h"
#include "Actors/D1WeaponBase.h"
#include "Character/LyraCharacter.h"
#include "System/LyraAssetManager.h"
#include "System/LyraGameData.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1GameplayAbility_Weapon_Projectile)

UD1GameplayAbility_Weapon_Projectile::UD1GameplayAbility_Weapon_Projectile(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
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
	
	if (WeaponActor == nullptr)
		return;
	
	USkeletalMeshComponent* WeaponMeshComponent = WeaponActor->WeaponMeshComponent;
	if (WeaponMeshComponent == nullptr)
		return;
	
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (ASC == nullptr)
		return;

#if UE_EDITOR
	check(WeaponMeshComponent->DoesSocketExist(ProjectileSocketName));
#endif
	
	if (WeaponMeshComponent->DoesSocketExist(ProjectileSocketName) == false)
		return;

	if (ProjectileClasses.IsValidIndex(CurrentIndex) == false)
		return;
	
	FTransform SpawnTransform = WeaponMeshComponent->GetSocketTransform(ProjectileSocketName);
	if (ALyraCharacter* LyraCharacter = Cast<ALyraCharacter>(GetAvatarActorFromActorInfo()))
	{
		SpawnTransform.SetRotation(LyraCharacter->GetController()->GetControlRotation().Quaternion());
	}
	
	AD1ProjectileBase* Projectile = GetWorld()->SpawnActorDeferred<AD1ProjectileBase>(
		ProjectileClasses[CurrentIndex],
		SpawnTransform,
		GetAvatarActorFromActorInfo(),
		Cast<APawn>(GetAvatarActorFromActorInfo()),
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn
	);
	
	FGameplayEffectContextHandle EffectContextHandle = ASC->MakeEffectContext();
	EffectContextHandle.SetAbility(this);
	EffectContextHandle.AddSourceObject(Projectile);

	const TSubclassOf<UGameplayEffect> DamageGE = ULyraAssetManager::GetSubclassByPath(ULyraGameData::Get().DamageGameplayEffect_SetByCaller);
	const FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(DamageGE);
	
	Projectile->Init(EffectSpecHandle);
	Projectile->FinishSpawning(SpawnTransform);
}
