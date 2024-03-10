#include "D1GameplayAbility_ComboAttack.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AIHelpers.h"
#include "Weapon/D1WeaponBase.h"
#include "D1GameplayTags.h"
#include "AbilitySystem/D1AbilitySystemComponent.h"
#include "Character/D1Character.h"
#include "Kismet/KismetMathLibrary.h"
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
	if (bBlocked)
		return;
	
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

	// TODO: Sub-Step Detection
	GetWorld()->ComponentSweepMulti(HitResults, WeaponActor->WeaponMesh, Start, End, WeaponActor->GetActorRotation(), Params);
	
	PrevWeaponLocation = WeaponActor->GetActorLocation();
	PrevWeaponRotation = WeaponActor->GetActorRotation();
	bIsFirstTrace = false;

	FGameplayAbilityTargetDataHandle TargetDataHandle;

	for (const FHitResult& HitResult : HitResults)
	{
		if (AActor* HitActor = HitResult.GetActor())
		{
			if (HitActors.Contains(HitActor) == false)
			{
				HitActors.Add(HitActor);
				
				bool bShouldAdd = false;
				if (HitActor->IsA(AD1Character::StaticClass()))
				{
					bShouldAdd = true;
				}
				else
				{
					if (AD1WeaponBase* WeaponBase = Cast<AD1WeaponBase>(HitResult.GetActor()))
					{
						if (WeaponBase->bCanBlock)
						{
							float Degree = UKismetMathLibrary::DegAcos((HitResult.ImpactPoint - WeaponBase->WeaponMesh->GetComponentLocation()).GetSafeNormal().Dot(WeaponBase->WeaponMesh->GetForwardVector())); 
							if (Degree <= 90.f)
							{
								bShouldAdd = true;
							}
						}
					}
					else
					{
						bShouldAdd = true;
					}
				}

				if (bShouldAdd)
				{
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
	if (bBlocked)
		return;
	
	// TODO: Check Validation
	TArray<FHitResult> AttackHitResults;
	TArray<FHitResult> BlockHitResults;
	
	for (const TSharedPtr<FGameplayAbilityTargetData> TargetData : InTargetData.Data)
	{
		if (FHitResult* HitResult = const_cast<FHitResult*>(TargetData->GetHitResult()))
		{
			if (AActor* HitActor = HitResult->GetActor())
			{
				if (HitActor->IsA(AD1Character::StaticClass()))
				{
					AttackHitResults.Emplace(MoveTemp(*HitResult));
				}
				else
				{
					BlockHitResults.Emplace(MoveTemp(*HitResult));
				}
			}
		}
	}
	
	for (const FHitResult& HitResult : BlockHitResults)
	{
		FGameplayCueParameters CueParameters;
		CueParameters.Location = HitResult.ImpactPoint;
		CueParameters.Normal = HitResult.ImpactNormal;
		CueParameters.PhysicalMaterial = HitResult.PhysMaterial;
			
		UD1AbilitySystemComponent* AbilitySystemComponent = GetAbilitySystemComponent();
		AbilitySystemComponent->ExecuteGameplayCue(D1GameplayTags::GameplayCue_Impact, CueParameters);
		
		UAnimInstance* AnimInstance = AbilitySystemComponent->AbilityActorInfo->GetAnimInstance();
		float EffectivePlayRate = AnimInstance->Montage_GetEffectivePlayRate(CurrentMontage);
		float Position = AnimInstance->Montage_GetPosition(CurrentMontage);
		
		AnimInstance->Montage_PlayWithBlendSettings(CurrentMontage, FMontageBlendSettings(0.f), -EffectivePlayRate / 3.f, EMontagePlayReturnType::MontageLength, Position);
		GetWorld()->GetTimerManager().SetTimer(BlockMontageTimerHandle, this, &ThisClass::HandleBlockMontage, 0.25f, false);

		bBlocked = true;
		return;
	}

	for (const FHitResult& HitResult : AttackHitResults)
	{
		FGameplayCueParameters CueParameters;
		CueParameters.Location = HitResult.ImpactPoint;
		CueParameters.Normal = HitResult.ImpactNormal;
		CueParameters.PhysicalMaterial = HitResult.PhysMaterial;

		UD1AbilitySystemComponent* AbilitySystemComponent = GetAbilitySystemComponent();
		AbilitySystemComponent->ExecuteGameplayCue(D1GameplayTags::GameplayCue_Impact, CueParameters);
		
		if (HasAuthority(&CurrentActivationInfo))
		{
			FGameplayAbilityTargetDataHandle TargetDataHandle = UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActor(HitResult.GetActor());
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

void UD1GameplayAbility_ComboAttack::HandleBlockMontage()
{
	UD1AbilitySystemComponent* AbilitySystemComponent = GetAbilitySystemComponent();
	UAnimInstance* AnimInstance = AbilitySystemComponent->AbilityActorInfo->GetAnimInstance();
	AnimInstance->Montage_Stop(0.2f, CurrentMontage);
	
	if (HasAuthority(&CurrentActivationInfo))
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}
}

bool UD1GameplayAbility_ComboAttack::CanContinueToNextStage()
{
	bool bCanContinue = (CurrentStage < AttackMontages.Num() - 1) && bInputPressed && (bBlocked == false);
	if (bCanContinue)
	{
		CurrentStage++;
		HitActors.Reset();
		bInputPressed = false;
		bIsFirstTrace = true;
	}
	return bCanContinue;
}
