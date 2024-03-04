#include "D1GameplayAbility_ComboAttack.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Weapon/D1WeaponBase.h"
#include "D1GameplayTags.h"
#include "KismetTraceUtils.h"
#include "Character/D1Player.h"
#include "Item/Managers/D1EquipManagerComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "System/D1AssetManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1GameplayAbility_ComboAttack)

UD1GameplayAbility_ComboAttack::UD1GameplayAbility_ComboAttack(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1GameplayAbility_ComboAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (HasAuthority(&ActivationInfo))
	{
		AD1Player* PlayerCharacter = Cast<AD1Player>(ActorInfo->AvatarActor.Get());
		UD1EquipManagerComponent* EquipManager = PlayerCharacter->EquipManagerComponent;
		EWeaponEquipState WeaponEquipState = EquipManager->GetCurrentWeaponEquipState();
		const TArray<FD1EquipEntry>& Entries = EquipManager->GetAllEntries();
	
		const FGameplayAbilitySpec* AbilitySpec = GetCurrentAbilitySpec();
		if (AbilitySpec->DynamicAbilityTags.HasTagExact(D1GameplayTags::Input_Action_Attack_MainHand))
		{
			switch (WeaponEquipState)
			{
			case EWeaponEquipState::Primary:
				WeaponActor = Entries[(int32)EEquipmentSlotType::Primary_RightHand].GetSpawnedWeaponActor();
				if (WeaponActor == nullptr)
				{
					WeaponActor = Entries[(int32)EEquipmentSlotType::Primary_TwoHand].GetSpawnedWeaponActor();
				}
				break;
			case EWeaponEquipState::Secondary:
				WeaponActor = Entries[(int32)EEquipmentSlotType::Secondary_RightHand].GetSpawnedWeaponActor();
				if (WeaponActor == nullptr)
				{
					WeaponActor = Entries[(int32)EEquipmentSlotType::Secondary_TwoHand].GetSpawnedWeaponActor();
				}
				break;
			}
		}
		else if (AbilitySpec->DynamicAbilityTags.HasTagExact(D1GameplayTags::Input_Action_Attack_OffHand))
		{
			switch (WeaponEquipState)
			{
			case EWeaponEquipState::Primary:
				WeaponActor = Entries[(int32)EEquipmentSlotType::Primary_LeftHand].GetSpawnedWeaponActor();
				break;
			case EWeaponEquipState::Secondary:
				WeaponActor =  Entries[(int32)EEquipmentSlotType::Secondary_LeftHand].GetSpawnedWeaponActor();
				break;
			}
		}

		if (WeaponActor == nullptr)
		{
			EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		}
	}
}

void UD1GameplayAbility_ComboAttack::CheckWeaponOverlap()
{
	if (bIsFirstCheck)
	{
		PrevWeaponLocation = WeaponActor->GetActorLocation();
		PrevWeaponRotation = WeaponActor->GetActorRotation();
	}
	
	TArray<FHitResult> OutHitResults;

	FComponentQueryParams Params = FComponentQueryParams::DefaultComponentQueryParams;
	TArray<AActor*> IgnoredActors = { WeaponActor, WeaponActor->GetOwner() };
	Params.AddIgnoredActors(IgnoredActors);

	FVector Start = PrevWeaponLocation;
	FVector End = WeaponActor->GetActorLocation();
	FRotator Rotation = UKismetMathLibrary::WeightedMovingAverage_FRotator(WeaponActor->GetActorRotation(), PrevWeaponRotation, 0.5f);
	GetWorld()->ComponentSweepMulti(OutHitResults, WeaponActor->WeaponMesh, Start, End, Rotation, Params);

	for (const FHitResult& HitResult : OutHitResults)
	{
		if (AActor* HittedActor = HitResult.GetActor())
		{
			if (HittedActors.Contains(HittedActor) == false)
			{
				HittedActors.Add(HittedActor);

				if (HittedActor->CanBeDamaged())
				{
					FGameplayAbilityTargetDataHandle TargetDataHandle = UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActor(HittedActor);
					TSubclassOf<UGameplayEffect> DamageEffectClass = UD1AssetManager::GetSubclassByName<UGameplayEffect>("GE_Damage");
					FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(DamageEffectClass);
					EffectSpecHandle.Data->AddDynamicAssetTag(D1GameplayTags::Attack_Physical);
					ApplyGameplayEffectSpecToTarget(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EffectSpecHandle, TargetDataHandle);
				}
				
				FGameplayCueParameters CueParameters;
				CueParameters.Location = HitResult.ImpactPoint;
				CueParameters.Normal = HitResult.ImpactNormal;
				
				UAbilitySystemComponent* AbilitySystemComponent = GetAbilitySystemComponentFromActorInfo_Checked();
				AbilitySystemComponent->ExecuteGameplayCue(D1GameplayTags::GameplayCue_Impact, CueParameters);
			}
		}
	}
	
	PrevWeaponLocation = WeaponActor->GetActorLocation();
	PrevWeaponRotation = WeaponActor->GetActorRotation();
	bIsFirstCheck = false;
}

bool UD1GameplayAbility_ComboAttack::CheckInputPress()
{
	bool bContinue = CurrentStage < AttackMontages.Num() - 1 && bInputPressed;
	if (bContinue)
	{
		CurrentStage++;
		HittedActors.Reset();
		bInputPressed = false;
		bIsFirstCheck = true;
	}
	return bContinue;
}
