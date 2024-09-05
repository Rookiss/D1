#include "D1GameplayAbility_Weapon.h"

#include "AbilitySystemGlobals.h"
#include "D1GameplayTags.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "Abilities/Async/AbilityAsync_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Actors/D1WeaponBase.h"
#include "AbilitySystem/LyraAbilitySystemComponent.h"
#include "Character/LyraCharacter.h"
#include "Development/LyraDeveloperSettings.h"
#include "Item/D1ItemInstance.h"
#include "Item/Managers/D1EquipManagerComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Player/LyraPlayerController.h"
#include "Shakes/LegacyCameraShake.h"
#include "System/LyraAssetManager.h"
#include "System/LyraGameData.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1GameplayAbility_Weapon)

UD1GameplayAbility_Weapon::UD1GameplayAbility_Weapon(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bServerRespectsRemoteAbilityCancellation = false;
	NetSecurityPolicy = EGameplayAbilityNetSecurityPolicy::ServerOnlyTermination;
}

void UD1GameplayAbility_Weapon::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	ResetHitActors();
	WeaponActor = nullptr;
	
	if (ALyraCharacter* PlayerCharacter = Cast<ALyraCharacter>(ActorInfo->AvatarActor.Get()))
	{
		if (UD1EquipManagerComponent* EquipManager = PlayerCharacter->FindComponentByClass<UD1EquipManagerComponent>())
		{
			WeaponActor = EquipManager->GetEquippedActor(WeaponHandType);
		}
	}
	
	if (WeaponActor == nullptr)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}

	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UD1GameplayAbility_Weapon::ParseTargetData(const FGameplayAbilityTargetDataHandle& InTargetDataHandle, TArray<int32>& OutCharacterHitIndexes, TArray<int32>& OutBlockHitIndexes)
{
	for (int32 i = 0; i < InTargetDataHandle.Data.Num(); i++)
	{
		const TSharedPtr<FGameplayAbilityTargetData>& TargetData = InTargetDataHandle.Data[i];

		if (FHitResult* HitResult = const_cast<FHitResult*>(TargetData->GetHitResult()))
		{
			if (AActor* HitActor = HitResult->GetActor())
			{
				ALyraCharacter* TargetCharacter = Cast<ALyraCharacter>(HitActor);
				if (TargetCharacter == nullptr)
				{
					TargetCharacter = Cast<ALyraCharacter>(HitActor->GetOwner());
					HitActor = TargetCharacter;
				}

				if (HitActors.Contains(HitActor))
					continue;

				HitActors.Add(HitActor);

				if (TargetCharacter)
				{
					UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(TargetCharacter);
					if (TargetASC && TargetASC->HasMatchingGameplayTag(D1GameplayTags::Status_Block))
					{
						FVector TargetLocation = HitActor->GetActorLocation();
						FVector TargetDirection = HitActor->GetActorForwardVector();
								
						FVector InstigatorLocation = GetAvatarActorFromActorInfo()->GetActorLocation();
						FVector TargetToInstigator = InstigatorLocation - TargetLocation;
								
						float Degree = UKismetMathLibrary::DegAcos(TargetDirection.Dot(TargetToInstigator.GetSafeNormal()));
						if (Degree <= 45.f)
						{
							OutBlockHitIndexes.Add(i);
							continue;
						}
					}
					
					OutCharacterHitIndexes.Add(i);
				}
				else
				{
					OutBlockHitIndexes.Add(i);
				}
			}
		}
	}
}

void UD1GameplayAbility_Weapon::ProcessHitResult(FHitResult HitResult, float Damage, bool bBlockingHit, UAnimMontage* BlockMontage)
{
	ULyraAbilitySystemComponent* SourceASC = GetLyraAbilitySystemComponentFromActorInfo();
	if (SourceASC == nullptr)
		return;
	
	FGameplayCueParameters SourceCueParams;
	SourceCueParams.Location = HitResult.ImpactPoint;
	SourceCueParams.Normal = HitResult.ImpactNormal;
	SourceCueParams.PhysicalMaterial = HitResult.PhysMaterial;
	SourceASC->ExecuteGameplayCue(D1GameplayTags::GameplayCue_Weapon_Impact, SourceCueParams);

	if (BlockMontage)
	{
		SourceASC->BlockAnimMontageForSeconds(BlockMontage);
	}
	
	if (HasAuthority(&CurrentActivationInfo))
	{
		if (BlockMontage)
		{
			FOnMontageEnded MontageEnded = FOnMontageEnded::CreateWeakLambda(this, [this](UAnimMontage* AnimMontage, bool bInterrupted)
			{
				EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
			});
			UAnimInstance* AnimInstance = SourceASC->AbilityActorInfo->GetAnimInstance();
			AnimInstance->Montage_SetEndDelegate(MontageEnded, BlockMontage);
		}
		
		FGameplayAbilityTargetDataHandle TargetDataHandle = UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActor(HitResult.GetActor());
		const TSubclassOf<UGameplayEffect> DamageGE = ULyraAssetManager::GetSubclassByPath(ULyraGameData::Get().DamageGameplayEffect_SetByCaller);
		FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(DamageGE);

		FGameplayEffectContextHandle EffectContextHandle = SourceASC->MakeEffectContext();
		HitResult.bBlockingHit = bBlockingHit;
		EffectContextHandle.AddHitResult(HitResult);
		EffectContextHandle.AddInstigator(SourceASC->AbilityActorInfo->OwnerActor.Get(), WeaponActor);
		EffectSpecHandle.Data->SetContext(EffectContextHandle);
		
		Damage = bBlockingHit ? Damage / 3.f : Damage;
		
		EffectSpecHandle.Data->SetSetByCallerMagnitude(D1GameplayTags::SetByCaller_BaseDamage, Damage);
		ApplyGameplayEffectSpecToTarget(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EffectSpecHandle, TargetDataHandle);
	}
	
	DrawDebugHitPoint(HitResult);
}

void UD1GameplayAbility_Weapon::ResetHitActors()
{
	HitActors.Reset();
}

void UD1GameplayAbility_Weapon::DrawDebugHitPoint(const FHitResult& HitResult)
{
#if UE_EDITOR
	if (GIsEditor)
	{
		const ULyraDeveloperSettings* DeveloperSettings = GetDefault<ULyraDeveloperSettings>();
		if (DeveloperSettings->bForceDisableDebugTrace == false && bShowDebug)
		{
			FColor Color = (HasAuthority(&CurrentActivationInfo)) ? FColor::Red : FColor::Green;
			DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, 4, 32, Color, false, 5);
		}
	}
#endif // UE_EDITOR
}

int32 UD1GameplayAbility_Weapon::GetWeaponStatValue(FGameplayTag StatTag) const
{
	int32 StatValue = 0;
	if (UD1EquipManagerComponent* EquipManager = GetLyraCharacterFromActorInfo()->FindComponentByClass<UD1EquipManagerComponent>())
	{
		if (const UD1ItemInstance* ItemInstance = EquipManager->GetEquippedItemInstance(WeaponHandType))
		{
			StatValue = ItemInstance->GetStackCountByTag(StatTag);
		}
	}
	return StatValue;
}

float UD1GameplayAbility_Weapon::GetAttackRate() const
{
	float AttackRate = DefaultAttackRate;
	
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		if (const UD1CombatSet* CombatSet = Cast<UD1CombatSet>(ASC->GetAttributeSet(UD1CombatSet::StaticClass())))
		{
			float AttackSpeedPercent = CombatSet->GetAttackSpeedPercent();
			AttackRate = DefaultAttackRate + (DefaultAttackRate * (AttackSpeedPercent / 100.f));
		}
	}
	
	return AttackRate;
}
