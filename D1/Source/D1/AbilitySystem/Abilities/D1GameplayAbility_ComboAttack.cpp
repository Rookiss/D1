#include "D1GameplayAbility_ComboAttack.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Weapon/D1WeaponBase.h"
#include "D1GameplayTags.h"
#include "AbilitySystem/D1AbilitySystemComponent.h"
#include "Character/D1Character.h"
#include "System/D1AssetManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1GameplayAbility_ComboAttack)

UD1GameplayAbility_ComboAttack::UD1GameplayAbility_ComboAttack(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1GameplayAbility_ComboAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	OnTargetDataReadyDelegateHandle = GetAbilitySystemComponent()->AbilityTargetDataSetDelegate(CurrentSpecHandle, CurrentActivationInfo.GetActivationPredictionKey()).AddUObject(this, &ThisClass::OnTargetDataReady);
	
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UD1GameplayAbility_ComboAttack::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	UD1AbilitySystemComponent* AbilitySystemComponent = GetAbilitySystemComponent();
	AbilitySystemComponent->AbilityTargetDataSetDelegate(CurrentSpecHandle, CurrentActivationInfo.GetActivationPredictionKey()).Remove(OnTargetDataReadyDelegateHandle);
	AbilitySystemComponent->ConsumeClientReplicatedTargetData(CurrentSpecHandle, CurrentActivationInfo.GetActivationPredictionKey());
		
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UD1GameplayAbility_ComboAttack::PerformHitDetection()
{
	if (bIsFirstTrace)
	{
		PrevWeaponLocation = WeaponActor->GetActorLocation();
		PrevWeaponRotation = WeaponActor->GetActorRotation();
	}
	
	FComponentQueryParams Params = FComponentQueryParams::DefaultComponentQueryParams;
	Params.bReturnPhysicalMaterial = true;
	TArray<AActor*> IgnoredActors = { WeaponActor, WeaponActor->GetOwner() };
	Params.AddIgnoredActors(IgnoredActors);

	TArray<FHitResult> HitResults;
	FVector Start = PrevWeaponLocation;
	FVector End = WeaponActor->GetActorLocation();
	GetWorld()->ComponentSweepMulti(HitResults, WeaponActor->WeaponMesh, Start, End, WeaponActor->GetActorRotation(), Params);
	
	PrevWeaponLocation = WeaponActor->GetActorLocation();
	PrevWeaponRotation = WeaponActor->GetActorRotation();
	bIsFirstTrace = false;

	FGameplayAbilityTargetDataHandle TargetDataHandle;

	for (const FHitResult& HitResult : HitResults)
	{
		if (AActor* HitActor = HitResult.GetActor())
		{
			if (HitActor->IsA(AD1Character::StaticClass()))
			{
				if (HitActors.Contains(HitActor) == false)
				{
					HitActors.Add(HitActor);
				
					FGameplayAbilityTargetData_SingleTargetHit* NewTargetData = new FGameplayAbilityTargetData_SingleTargetHit();
					NewTargetData->HitResult = HitResult;
					TargetDataHandle.Add(NewTargetData);
				}
			}
		}
	}

	if (TargetDataHandle.Num() > 0)
	{
		OnTargetDataReady(TargetDataHandle, FGameplayTag());
	}
}

bool UD1GameplayAbility_ComboAttack::CheckInputPress()
{
	bool bCanContinue = CurrentStage < AttackMontages.Num() - 1 && bInputPressed;
	if (bCanContinue)
	{
		CurrentStage++;
		HitActors.Reset();
		bInputPressed = false;
		bIsFirstTrace = true;
		bAttacked = false;
	}
	return bCanContinue;
}

void UD1GameplayAbility_ComboAttack::OnTargetDataReady(const FGameplayAbilityTargetDataHandle& InData, FGameplayTag ApplicationTag)
{
	UD1AbilitySystemComponent* AbilitySystemComponent = GetAbilitySystemComponent();

	if (AbilitySystemComponent->FindAbilitySpecFromHandle(CurrentSpecHandle))
	{
		FScopedPredictionWindow	ScopedPrediction(AbilitySystemComponent);

		FGameplayAbilityTargetDataHandle LocalTargetDataHandle(MoveTemp(const_cast<FGameplayAbilityTargetDataHandle&>(InData)));

		const bool bShouldNotifyServer = CurrentActorInfo->IsLocallyControlled() && CurrentActorInfo->IsNetAuthority() == false;
		if (bShouldNotifyServer)
		{
			AbilitySystemComponent->CallServerSetReplicatedTargetData(CurrentSpecHandle, CurrentActivationInfo.GetActivationPredictionKey(), LocalTargetDataHandle, ApplicationTag, AbilitySystemComponent->ScopedPredictionKey);
		}
		
		ProcessTargetData(LocalTargetDataHandle);
	}
	
	AbilitySystemComponent->ConsumeClientReplicatedTargetData(CurrentSpecHandle, CurrentActivationInfo.GetActivationPredictionKey());
}

void UD1GameplayAbility_ComboAttack::ProcessTargetData(const FGameplayAbilityTargetDataHandle& InTargetData)
{
	for (const TSharedPtr<FGameplayAbilityTargetData> TargetData : InTargetData.Data)
	{
		if (const FHitResult* HitResult = TargetData->GetHitResult())
		{
			FGameplayCueParameters CueParameters;
			CueParameters.Location = HitResult->ImpactPoint;
			CueParameters.Normal = HitResult->ImpactNormal;
			CueParameters.PhysicalMaterial = HitResult->PhysMaterial;

			UD1AbilitySystemComponent* AbilitySystemComponent = GetAbilitySystemComponent();
			AbilitySystemComponent->ExecuteGameplayCue(D1GameplayTags::GameplayCue_Impact, CueParameters);

			if (HasAuthority(&CurrentActivationInfo))
			{
				FGameplayAbilityTargetDataHandle TargetDataHandle = UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActor(HitResult->GetActor());
				TSubclassOf<UGameplayEffect> DamageEffectClass = UD1AssetManager::GetSubclassByName<UGameplayEffect>("GE_Damage");
				FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(DamageEffectClass);
				EffectSpecHandle.Data->AddDynamicAssetTag(D1GameplayTags::Attack_Physical);
				ApplyGameplayEffectSpecToTarget(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EffectSpecHandle, TargetDataHandle);
				
				AbilitySystemComponent->Multicast_SlowAnimMontageForSeconds(AbilitySystemComponent->GetCurrentActiveMontage(), 0.2f, 0.1f);
			}
			else
			{
				AbilitySystemComponent->SlowAnimMontageForSecondsLocal(AbilitySystemComponent->GetCurrentActiveMontage(), 0.2f, 0.1f);
			}
		}
	}
	
	// 				
	// bAttacked = true;
	
	
	// for (const FHitResult& HitResult : HitResults)
	// {
	// 	if (AActor* HitActor = HitResult.GetActor())
	// 	{
	// 		if (HitActor->IsA(AD1Character::StaticClass()))
	// 		{
	// 			if (HitActors.Contains(HitActor) == false)
	// 			{
	// 				HitActors.Add(HitActor);
	// 			
	// 				FGameplayCueParameters CueParameters;
	// 				CueParameters.Location = HitResult.ImpactPoint;
	// 				CueParameters.Normal = HitResult.ImpactNormal;
	// 				CueParameters.PhysicalMaterial = HitResult.PhysMaterial;
	// 			
	// 				UD1AbilitySystemComponent* AbilitySystemComponent = GetAbilitySystemComponent();
	// 				AbilitySystemComponent->ExecuteGameplayCue(D1GameplayTags::GameplayCue_Impact, CueParameters);
	//
	// 				FGameplayAbilityTargetDataHandle TargetDataHandle = UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActor(HitActor);
	// 				TSubclassOf<UGameplayEffect> DamageEffectClass = UD1AssetManager::GetSubclassByName<UGameplayEffect>("GE_Damage");
	// 				FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(DamageEffectClass);
	// 				EffectSpecHandle.Data->AddDynamicAssetTag(D1GameplayTags::Attack_Physical);
	// 				ApplyGameplayEffectSpecToTarget(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EffectSpecHandle, TargetDataHandle);
	// 				
	// 				bAttacked = true;
	// 				HitState = EHitState::Attacked;
	// 				// AbilitySystemComponent->SlowAnimMontageForSeconds(1.f, 0.2f);
	// 			}
	// 		}
	// 		else
	// 		{
	// 			// if (bAttacked)
	// 			// 	continue;
	// 			//
	// 			// bool bBlocked = false;
	// 			//
	// 			// if (AD1WeaponBase* WeaponBase = Cast<AD1WeaponBase>(HitActor))
	// 			// {
	// 			// 	if (WeaponBase->WeaponMesh->bBlocking)
	// 			// 	{
	// 			// 		bBlocked = true;
	// 			// 	}
	// 			// }
	// 			// else
	// 			// {
	// 			// 	bBlocked = true;
	// 			// }
	// 			//
	// 			// if (bBlocked)
	// 			// {
	// 			// 	FGameplayCueParameters CueParameters;
	// 			// 	CueParameters.Location = HitResult.ImpactPoint;
	// 			// 	CueParameters.Normal = HitResult.ImpactNormal;
	// 			// 	CueParameters.PhysicalMaterial = HitResult.PhysMaterial;
	// 			//
	// 			// 	UAbilitySystemComponent* AbilitySystemComponent = GetAbilitySystemComponentFromActorInfo_Checked();
	// 			// 	AbilitySystemComponent->ExecuteGameplayCue(D1GameplayTags::GameplayCue_Impact, CueParameters);
	// 			//
	// 			// 	// EndAbility()
	// 			// 	break;
	// 			// }
	// 		}
	// 	}
	// }
}
