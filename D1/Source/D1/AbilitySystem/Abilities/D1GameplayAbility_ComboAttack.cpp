#include "D1GameplayAbility_ComboAttack.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Weapon/D1WeaponBase.h"
#include "D1GameplayTags.h"
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
			WeaponActor = Entries[(int32)EEquipmentSlotType::Primary_RightHand].SpawnedWeaponActor;
			if (WeaponActor == nullptr)
			{
				WeaponActor = Entries[(int32)EEquipmentSlotType::Primary_TwoHand].SpawnedWeaponActor;
			}
			break;
		case EWeaponEquipState::Secondary:
			WeaponActor = Entries[(int32)EEquipmentSlotType::Secondary_RightHand].SpawnedWeaponActor;
			if (WeaponActor == nullptr)
			{
				WeaponActor = Entries[(int32)EEquipmentSlotType::Secondary_TwoHand].SpawnedWeaponActor;
			}
			break;
		}
	}
	else if (AbilitySpec->DynamicAbilityTags.HasTagExact(D1GameplayTags::Input_Action_Attack_OffHand))
	{
		switch (WeaponEquipState)
		{
		case EWeaponEquipState::Primary:
			WeaponActor = Entries[(int32)EEquipmentSlotType::Primary_LeftHand].SpawnedWeaponActor;
			break;
		case EWeaponEquipState::Secondary:
			WeaponActor =  Entries[(int32)EEquipmentSlotType::Secondary_LeftHand].SpawnedWeaponActor;
			break;
		}
	}

	if (WeaponActor == nullptr)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}
}

void UD1GameplayAbility_ComboAttack::CheckAttackHit()
{
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
	GetWorld()->ComponentSweepMulti(OutHitResults, WeaponActor->WeaponMesh, Start, End, WeaponActor->GetActorRotation(), Params);

	for (const FHitResult& HitResult : OutHitResults)
	{
		if (AActor* HittedActor = HitResult.GetActor())
		{
			if (HittedActor->IsA(AD1Character::StaticClass()) == false)
				return;

			if (HasAuthority(&CurrentActivationInfo))
			{
				if (HittedActors.Contains(HittedActor) == false)
				{
					HittedActors.Add(HittedActor);

					FGameplayCueParameters CueParameters;
					CueParameters.Location = HitResult.ImpactPoint;
					CueParameters.Normal = HitResult.ImpactNormal;
					CueParameters.PhysicalMaterial = HitResult.PhysMaterial;
				
					UAbilitySystemComponent* AbilitySystemComponent = GetAbilitySystemComponentFromActorInfo_Checked();
					AbilitySystemComponent->ExecuteGameplayCue(D1GameplayTags::GameplayCue_Impact, CueParameters);
				
					FGameplayAbilityTargetDataHandle TargetDataHandle = UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActor(HittedActor);
					TSubclassOf<UGameplayEffect> DamageEffectClass = UD1AssetManager::GetSubclassByName<UGameplayEffect>("GE_Damage");
					FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(DamageEffectClass);
					EffectSpecHandle.Data->AddDynamicAssetTag(D1GameplayTags::Attack_Physical);
					ApplyGameplayEffectSpecToTarget(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EffectSpecHandle, TargetDataHandle);
				}
			}
		}
	}
	
	PrevWeaponLocation = WeaponActor->GetActorLocation();
	PrevWeaponRotation = WeaponActor->GetActorRotation();
	bIsFirstCheck = false;

	return;
}

bool UD1GameplayAbility_ComboAttack::CheckBlockHit()
{
	return true;
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
