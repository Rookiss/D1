#include "D1GameplayAbility_Weapon_Spell.h"

#include "Actors/D1AOEBase.h"
#include "Actors/D1ProjectileBase.h"
#include "Actors/D1WeaponBase.h"
#include "Character/LyraCharacter.h"
#include "System/LyraAssetManager.h"
#include "System/LyraGameData.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1GameplayAbility_Weapon_Spell)

UD1GameplayAbility_Weapon_Spell::UD1GameplayAbility_Weapon_Spell(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1GameplayAbility_Weapon_Spell::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	bCastTimePassed = false;
	
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UD1GameplayAbility_Weapon_Spell::SpawnProjectile()
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
	
	FTransform SpawnTransform = WeaponMeshComponent->GetSocketTransform(ProjectileSocketName);
	if (ALyraCharacter* LyraCharacter = Cast<ALyraCharacter>(GetAvatarActorFromActorInfo()))
	{
		SpawnTransform.SetRotation(LyraCharacter->GetController()->GetControlRotation().Quaternion());
	}
	
	AD1ProjectileBase* Projectile = GetWorld()->SpawnActorDeferred<AD1ProjectileBase>(
		ProjectileClass,
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

void UD1GameplayAbility_Weapon_Spell::SpawnAOE()
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (ASC == nullptr)
		return;
	
	if (TargetDataHandle.Data.IsValidIndex(0))
	{
		if (FGameplayAbilityTargetData* TargetData = TargetDataHandle.Data[0].Get())
		{
			if (const FHitResult* HitResultPtr = TargetData->GetHitResult())
			{
				const FHitResult& HitResult = *HitResultPtr;

				FTransform SpawnTransform;
				SpawnTransform.SetLocation(HitResult.Location);
				if (ALyraCharacter* LyraCharacter = Cast<ALyraCharacter>(GetAvatarActorFromActorInfo()))
				{
					SpawnTransform.SetRotation(LyraCharacter->GetController()->GetControlRotation().Quaternion());
				}
				
				AD1AOEBase* AOE = GetWorld()->SpawnActorDeferred<AD1AOEBase>(
					AOESpawnerClass,
					SpawnTransform,
					GetAvatarActorFromActorInfo(),
					Cast<APawn>(GetAvatarActorFromActorInfo()),
					ESpawnActorCollisionHandlingMethod::AlwaysSpawn
				);
				
				FGameplayEffectContextHandle EffectContextHandle = ASC->MakeEffectContext();
				EffectContextHandle.SetAbility(this);
				EffectContextHandle.AddSourceObject(AOE);

				const TSubclassOf<UGameplayEffect> DamageGE = ULyraAssetManager::GetSubclassByPath(ULyraGameData::Get().DamageGameplayEffect_SetByCaller);
				const FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(DamageGE);
	
				AOE->Init(EffectSpecHandle);
				AOE->FinishSpawning(SpawnTransform);
			}
		}
	}

	TargetDataHandle.Clear();
}
