#include "D1GameplayAbility_ComboAttack.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Weapon/D1WeaponBase.h"
#include "D1GameplayTags.h"
#include "Physics/D1CollisionChannels.h"
#include "System/D1AssetManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1GameplayAbility_ComboAttack)

UD1GameplayAbility_ComboAttack::UD1GameplayAbility_ComboAttack(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1GameplayAbility_ComboAttack::CheckAttackHit()
{
	if (bBlocked)
		return;
	
	if (bIsFirstCheck)
	{
		PrevWeaponLocation = WeaponActor->GetActorLocation();
		PrevWeaponRotation = WeaponActor->GetActorRotation();
	}
	
	TArray<FHitResult> OutHitResults;

	FComponentQueryParams Params = FComponentQueryParams::DefaultComponentQueryParams;
	Params.bReturnPhysicalMaterial = true;
	TArray<AActor*> IgnoredActors = { WeaponActor, WeaponActor->GetOwner() };
	Params.AddIgnoredActors(IgnoredActors);

	FVector Start = PrevWeaponLocation;
	FVector End = WeaponActor->GetActorLocation();
	GetWorld()->ComponentSweepMultiByChannel(OutHitResults, WeaponActor->WeaponMesh, Start, End, WeaponActor->GetActorRotation(), D1_TraceChannel_Attack, Params);

	FGameplayAbilityTargetDataHandle TargetDataHandle;
	
	for (const FHitResult& HitResult : OutHitResults)
	{
		if (AActor* HitActor = HitResult.GetActor())
		{
			if (HitActors.Contains(HitActor) == false)
			{
				HitActors.Add(HitActor);

				FGameplayAbilityTargetData_SingleTargetHit* TargetData = new FGameplayAbilityTargetData_SingleTargetHit();
				TargetData->HitResult = HitResult;
				TargetDataHandle.Add(TargetData);
				
				// FGameplayCueParameters CueParameters;
				// CueParameters.Location = HitResult.ImpactPoint;
				// CueParameters.Normal = HitResult.ImpactNormal;
				// CueParameters.PhysicalMaterial = HitResult.PhysMaterial;
				//
				// UAbilitySystemComponent* AbilitySystemComponent = GetAbilitySystemComponentFromActorInfo_Checked();
				// AbilitySystemComponent->ExecuteGameplayCue(D1GameplayTags::GameplayCue_Impact, CueParameters);
				//
				// FGameplayAbilityTargetDataHandle TargetDataHandle = UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActor(HitActor);
				// TSubclassOf<UGameplayEffect> DamageEffectClass = UD1AssetManager::GetSubclassByName<UGameplayEffect>("GE_Damage");
				// FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(DamageEffectClass);
				// EffectSpecHandle.Data->AddDynamicAssetTag(D1GameplayTags::Attack_Physical);
				// ApplyGameplayEffectSpecToTarget(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EffectSpecHandle, TargetDataHandle);
			}
		}
	}

	if (TargetDataHandle.Num() > 0)
	{
		// NotifyTargetDataReady();
	}
	
	PrevWeaponLocation = WeaponActor->GetActorLocation();
	PrevWeaponRotation = WeaponActor->GetActorRotation();
	bIsFirstCheck = false;
}

void UD1GameplayAbility_ComboAttack::CheckBlockHit()
{
	if (bBlocked)
		return;
	
	if (bIsFirstCheck)
	{
		PrevWeaponLocation = WeaponActor->GetActorLocation();
		PrevWeaponRotation = WeaponActor->GetActorRotation();
	}
		
	TArray<FHitResult> OutHitResults;

	FComponentQueryParams Params = FComponentQueryParams::DefaultComponentQueryParams;
	Params.bReturnPhysicalMaterial = true;
	TArray<AActor*> IgnoredActors = { WeaponActor, WeaponActor->GetOwner() };
	Params.AddIgnoredActors(IgnoredActors);

	FVector Start = PrevWeaponLocation;
	FVector End = WeaponActor->GetActorLocation();
	GetWorld()->ComponentSweepMultiByChannel(OutHitResults, WeaponActor->WeaponMesh, Start, End, WeaponActor->GetActorRotation(), D1_TraceChannel_Block, Params);

	for (const FHitResult& HitResult : OutHitResults)
	{
		if (HitResult.GetActor())
		{
			FGameplayCueParameters CueParameters;
			CueParameters.Location = HitResult.ImpactPoint;
			CueParameters.Normal = HitResult.ImpactNormal;
			CueParameters.PhysicalMaterial = HitResult.PhysMaterial;
				
			UAbilitySystemComponent* AbilitySystemComponent = GetAbilitySystemComponentFromActorInfo_Checked();
			AbilitySystemComponent->ExecuteGameplayCue(D1GameplayTags::GameplayCue_Impact, CueParameters);

			bBlocked = true;
		}
	}
		
	PrevWeaponLocation = WeaponActor->GetActorLocation();
	PrevWeaponRotation = WeaponActor->GetActorRotation();
	bIsFirstCheck = false;
}

bool UD1GameplayAbility_ComboAttack::CheckInputPress()
{
	bool bCanContinue = CurrentStage < AttackMontages.Num() - 1 && bInputPressed;
	if (bCanContinue)
	{
		CurrentStage++;
		HitActors.Reset();
		bInputPressed = false;
		bIsFirstCheck = true;
	}
	return bCanContinue;
}
