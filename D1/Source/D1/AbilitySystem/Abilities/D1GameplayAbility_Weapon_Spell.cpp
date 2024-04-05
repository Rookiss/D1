#include "D1GameplayAbility_Weapon_Spell.h"

#include "D1GameplayTags.h"
#include "AbilitySystem/D1AbilitySystemComponent.h"
#include "Actors/D1ProjectileBase.h"
#include "Actors/D1WeaponBase.h"
#include "Character/D1Player.h"
#include "System/D1AssetManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1GameplayAbility_Weapon_Spell)

UD1GameplayAbility_Weapon_Spell::UD1GameplayAbility_Weapon_Spell(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1GameplayAbility_Weapon_Spell::SpawnProjectile()
{
	if (HasAuthority(&CurrentActivationInfo) == false)
		return;
	
	if (WeaponActor == nullptr)
		return;
	
	UStaticMeshComponent* WeaponMesh = WeaponActor->WeaponMeshComponent;
	if (WeaponMesh == nullptr)
		return;

#if UE_EDITOR
	check(WeaponMesh->DoesSocketExist(ProjectileSocketName));
#endif
	
	if (WeaponMesh->DoesSocketExist(ProjectileSocketName) == false)
		return;

	// TODO
	FTransform SpawnTransform = WeaponMesh->GetSocketTransform(ProjectileSocketName);
	AD1Player* Player = Cast<AD1Player>(GetAvatarActorFromActorInfo());
	SpawnTransform.SetRotation(Player->GetController()->GetControlRotation().Quaternion());

	AD1ProjectileBase* Projectile = GetWorld()->SpawnActorDeferred<AD1ProjectileBase>(
		ProjectileClass,
		SpawnTransform,
		GetAvatarActorFromActorInfo(),
		Cast<APawn>(GetAvatarActorFromActorInfo()),
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn
	);

	UAbilitySystemComponent* ASC = GetAbilitySystemComponent();

	FGameplayEffectContextHandle EffectContextHandle = ASC->MakeEffectContext();
	EffectContextHandle.SetAbility(this);
	EffectContextHandle.AddSourceObject(Projectile);

	TSubclassOf<UGameplayEffect> DamageEffectClass = UD1AssetManager::GetSubclassByName<UGameplayEffect>("GE_Damage");
	const FGameplayEffectSpecHandle EffectSpecHandle = ASC->MakeOutgoingSpec(DamageEffectClass, GetAbilityLevel(), EffectContextHandle);
	Projectile->DamageEffectSpecHandle = EffectSpecHandle;
	Projectile->FinishSpawning(SpawnTransform);
}

void UD1GameplayAbility_Weapon_Spell::ShowCastTrailEffect(const FVector& Location)
{
	FScopedPredictionWindow ScopedPrediction(GetAbilitySystemComponent(), CurrentActivationInfo.GetActivationPredictionKey());

	FGameplayCueParameters CueParameters;
	CueParameters.Location = Location;
	
	UD1AbilitySystemComponent* AbilitySystemComponent = GetAbilitySystemComponent();
	AbilitySystemComponent->AddGameplayCue(D1GameplayTags::GameplayCue_Cast, CueParameters);
}
