#include "D1GameplayAbility_Weapon_Spell.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "D1GameplayTags.h"
#include "AbilitySystem/D1AbilitySystemComponent.h"
#include "Actors/D1ProjectileBase.h"
#include "Actors/D1WeaponBase.h"
#include "Character/D1Player.h"
#include "System/D1AssetManager.h"
#include "UI/D1HUD.h"
#include "UI/D1SpellProgressWidget.h"

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

void UD1GameplayAbility_Weapon_Spell::NotifyWidget_Implementation()
{
	bCastTimePassed = true;

	if (AD1HUD* HUD = GetHUD())
	{
		if (UD1SpellProgressWidget* SpellProgressWidget = HUD->GetSpellProgressWidget())
		{
			SpellProgressWidget->NotifyWidget();
		}
	}
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
	
	FTransform SpawnTransform = WeaponMesh->GetSocketTransform(ProjectileSocketName);
	if (AD1Player* Player = Cast<AD1Player>(GetAvatarActorFromActorInfo()))
	{
		SpawnTransform.SetRotation(Player->GetController()->GetControlRotation().Quaternion());
	}
	
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
	EffectSpecHandle.Data->AddDynamicAssetTag(D1GameplayTags::Attack_Magical);
	
	Projectile->DamageEffectSpecHandle = EffectSpecHandle;
	Projectile->FinishSpawning(SpawnTransform);
}

void UD1GameplayAbility_Weapon_Spell::SpawnAOE()
{
	if (TargetDataHandle.Data.IsValidIndex(0))
	{
		if (FGameplayAbilityTargetData* TargetData = TargetDataHandle.Data[0].Get())
		{
			if (const FHitResult* HitResultPtr = TargetData->GetHitResult())
			{
				const FHitResult& HitResult = *HitResultPtr;

				FTransform SpawnTransform;
				SpawnTransform.SetLocation(HitResult.Location);
				if (AD1Player* Player = Cast<AD1Player>(GetAvatarActorFromActorInfo()))
				{
					SpawnTransform.SetRotation(Player->GetController()->GetControlRotation().Quaternion());
				}
				
				AActor* AOE = GetWorld()->SpawnActorDeferred<AActor>(
					AOEActorClass,
					SpawnTransform,
					GetAvatarActorFromActorInfo(),
					Cast<APawn>(GetAvatarActorFromActorInfo()),
					ESpawnActorCollisionHandlingMethod::AlwaysSpawn
				);

				UAbilitySystemComponent* ASC = GetAbilitySystemComponent();

				FGameplayEffectContextHandle EffectContextHandle = ASC->MakeEffectContext();
				EffectContextHandle.SetAbility(this);
				EffectContextHandle.AddSourceObject(AOE);

				TSubclassOf<UGameplayEffect> DamageEffectClass = UD1AssetManager::GetSubclassByName<UGameplayEffect>("GE_Damage");
				const FGameplayEffectSpecHandle EffectSpecHandle = ASC->MakeOutgoingSpec(DamageEffectClass, GetAbilityLevel(), EffectContextHandle);
				EffectSpecHandle.Data->AddDynamicAssetTag(D1GameplayTags::Attack_Magical);
	
				Projectile->DamageEffectSpecHandle = EffectSpecHandle;

				
				AOE->FinishSpawning(SpawnTransform);
			}
		}
	}

	TargetDataHandle.Clear();
}
