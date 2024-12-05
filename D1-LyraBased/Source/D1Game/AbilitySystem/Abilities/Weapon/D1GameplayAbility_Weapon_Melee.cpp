#include "D1GameplayAbility_Weapon_Melee.h"

#include "AbilitySystemGlobals.h"
#include "D1GameplayTags.h"
#include "AbilitySystem/LyraAbilitySystemComponent.h"
#include "Actors/D1EquipmentBase.h"
#include "Development/D1DeveloperSettings.h"
#include "Kismet/KismetMathLibrary.h"
#include "System/LyraAssetManager.h"
#include "System/LyraGameData.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1GameplayAbility_Weapon_Melee)

UD1GameplayAbility_Weapon_Melee::UD1GameplayAbility_Weapon_Melee(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}

void UD1GameplayAbility_Weapon_Melee::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	ResetHitActors();
}

void UD1GameplayAbility_Weapon_Melee::ParseTargetData(const FGameplayAbilityTargetDataHandle& InTargetDataHandle, TArray<int32>& OutCharacterHitIndexes, TArray<int32>& OutBlockHitIndexes)
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
				}

				ID1TeamAgentInterface* TeamAgentInterface = Cast<ID1TeamAgentInterface>(TargetCharacter);
				if (TeamAgentInterface && (TeamAgentInterface->GetTeamAttitudeTowards(*GetLyraCharacterFromActorInfo()) != ETeamAttitude::Hostile))
					continue;
				
				bool bIsCharacterBlockingHit = TargetCharacter ? IsCharacterBlockingHit(TargetCharacter) : false;
				
				AD1EquipmentBase* HitWeaponActor = Cast<AD1EquipmentBase>(HitActor);
				if (HitWeaponActor && (HitWeaponActor->bCanBlock == false || bIsCharacterBlockingHit == false))
					continue;
				
				AActor* SelectedActor = TargetCharacter ? TargetCharacter : HitActor;
				if (CachedHitActors.Contains(SelectedActor))
					continue;

				CachedHitActors.Add(SelectedActor);

				if (TargetCharacter)
				{
					bIsCharacterBlockingHit ? OutBlockHitIndexes.Add(i) : OutCharacterHitIndexes.Add(i);
				}
				else
				{
					OutBlockHitIndexes.Add(i);
				}
			}
		}
	}
}

void UD1GameplayAbility_Weapon_Melee::ProcessHitResult(FHitResult HitResult, float Damage, bool bBlockingHit, UAnimMontage* BackwardMontage, AD1EquipmentBase* WeaponActor)
{
	ULyraAbilitySystemComponent* SourceASC = GetLyraAbilitySystemComponentFromActorInfo();
	if (SourceASC == nullptr)
		return;
	
	FScopedPredictionWindow	ScopedPrediction(SourceASC, GetCurrentActivationInfo().GetActivationPredictionKey());
	
	FGameplayCueParameters SourceCueParams;
	SourceCueParams.Location = HitResult.ImpactPoint;
	SourceCueParams.Normal = HitResult.ImpactNormal;
	SourceCueParams.PhysicalMaterial = bBlockingHit ? nullptr : HitResult.PhysMaterial;
	SourceASC->ExecuteGameplayCue(D1GameplayTags::GameplayCue_Weapon_Impact, SourceCueParams);
	
	if (BackwardMontage)
	{
		SourceASC->BlockAnimMontageForSeconds(BackwardMontage);
	}
	
	if (HasAuthority(&CurrentActivationInfo))
	{
		if (BackwardMontage)
		{
			FOnMontageEnded MontageEnded = FOnMontageEnded::CreateWeakLambda(this, [this](UAnimMontage* AnimMontage, bool bInterrupted)
			{
				EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
			});
			UAnimInstance* AnimInstance = SourceASC->AbilityActorInfo->GetAnimInstance();
			AnimInstance->Montage_SetEndDelegate(MontageEnded, BackwardMontage);
		}
		
		FGameplayAbilityTargetDataHandle TargetDataHandle = UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActor(HitResult.GetActor());
		const TSubclassOf<UGameplayEffect> DamageGE = ULyraAssetManager::GetSubclassByPath(ULyraGameData::Get().DamageGameplayEffect_SetByCaller);
		FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(DamageGE);

		FGameplayEffectContextHandle EffectContextHandle = SourceASC->MakeEffectContext();
		HitResult.bBlockingHit = bBlockingHit;
		EffectContextHandle.AddHitResult(HitResult);
		EffectContextHandle.AddInstigator(SourceASC->AbilityActorInfo->OwnerActor.Get(), WeaponActor);
		EffectSpecHandle.Data->SetContext(EffectContextHandle);
		
		Damage = bBlockingHit ? Damage * BlockHitDamageMultiplier : Damage;
		
		EffectSpecHandle.Data->SetSetByCallerMagnitude(D1GameplayTags::SetByCaller_BaseDamage, Damage);
		ApplyGameplayEffectSpecToTarget(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EffectSpecHandle, TargetDataHandle);
	}
	
	DrawDebugHitPoint(HitResult);
}

void UD1GameplayAbility_Weapon_Melee::ResetHitActors()
{
	CachedHitActors.Reset();
}

void UD1GameplayAbility_Weapon_Melee::DrawDebugHitPoint(const FHitResult& HitResult)
{
#if UE_EDITOR
	if (GIsEditor)
	{
		const UD1DeveloperSettings* DeveloperSettings = GetDefault<UD1DeveloperSettings>();
		if (DeveloperSettings->bForceDisableDebugTrace == false && bShowDebug)
		{
			FColor Color = HasAuthority(&CurrentActivationInfo) ? FColor::Red : FColor::Green;
			DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, 4, 32, Color, false, 5);
		}
	}
#endif // UE_EDITOR
}

bool UD1GameplayAbility_Weapon_Melee::IsCharacterBlockingHit(ALyraCharacter* TargetCharacter)
{
	UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(TargetCharacter);
	if (TargetASC && TargetASC->HasMatchingGameplayTag(D1GameplayTags::Status_Block))
	{
		FVector TargetLocation = TargetCharacter->GetActorLocation();
		FVector TargetDirection = TargetCharacter->GetActorForwardVector();
								
		FVector InstigatorLocation = GetAvatarActorFromActorInfo()->GetActorLocation();
		FVector TargetToInstigator = InstigatorLocation - TargetLocation;
								
		float Degree = UKismetMathLibrary::DegAcos(TargetDirection.Dot(TargetToInstigator.GetSafeNormal()));
		if (Degree <= BlockingAngle)
			return true;
	}
	return false;
}
