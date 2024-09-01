#include "D1AnimNotifyState_OverlayEffect.h"

#include "Actors/D1WeaponBase.h"
#include "Character/LyraCharacter.h"
#include "Components/TimelineComponent.h"
#include "Curves/CurveLinearColor.h"
#include "Item/Managers/D1EquipManagerComponent.h"
#include "Kismet/KismetMaterialLibrary.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1AnimNotifyState_OverlayEffect)

UD1AnimNotifyState_OverlayEffect::UD1AnimNotifyState_OverlayEffect(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
#if WITH_EDITORONLY_DATA
	bShouldFireInEditor = false;
#endif
}

void UD1AnimNotifyState_OverlayEffect::NotifyBegin(USkeletalMeshComponent* MeshComponent, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComponent, Animation, TotalDuration, EventReference);
	
	if (OverlayTargetType == EOverlayTargetType::None)
		return;

	if (MeshComponent->GetOwnerRole() == ROLE_Authority)
		return;

	Clear();
	OverlayMaterialInstance = UKismetMaterialLibrary::CreateDynamicMaterialInstance(MeshComponent, OverlayMaterial);
	
	switch (OverlayTargetType)
	{
	case EOverlayTargetType::Weapon:
		ApplyWeaponMeshComponent(MeshComponent);
		break;
                                      		
	case EOverlayTargetType::Character:
		ApplyCharacterMeshComponents(MeshComponent);
		break;
                                      		
	case EOverlayTargetType::All:
		ApplyAllWeaponMeshComponents(MeshComponent);
		ApplyCharacterMeshComponents(MeshComponent);
		break;
	}
}

void UD1AnimNotifyState_OverlayEffect::NotifyTick(USkeletalMeshComponent* MeshComponent, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComponent, Animation, FrameDeltaTime, EventReference);

	if (MeshComponent->GetOwnerRole() == ROLE_Authority)
		return;
	
	if (OverlayMaterialInstance)
	{
		PassedTime += FrameDeltaTime;
		const FLinearColor& Value = LinearColorCurve->GetLinearColorValue(PassedTime / Animation->RateScale);
		OverlayMaterialInstance->SetVectorParameterValue(ParameterName, Value);
	}
}

void UD1AnimNotifyState_OverlayEffect::NotifyEnd(USkeletalMeshComponent* MeshComponent, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	if (MeshComponent->GetOwnerRole() == ROLE_Authority)
		return;
	
	Clear();
	
	Super::NotifyEnd(MeshComponent, Animation, EventReference);
}

void UD1AnimNotifyState_OverlayEffect::ApplyWeaponMeshComponent(USkeletalMeshComponent* MeshComponent)
{
	if (WeaponHandType == EWeaponHandType::Count)
		return;
	
	if (ALyraCharacter* LyraCharacter = Cast<ALyraCharacter>(MeshComponent->GetOwner()))
	{
		if (UD1EquipManagerComponent* EquipManager = LyraCharacter->FindComponentByClass<UD1EquipManagerComponent>())
		{
			if (AD1WeaponBase* WeaponActor = EquipManager->GetEquippedActor(WeaponHandType))
			{
				USkeletalMeshComponent* WeaponMeshComponent = WeaponActor->WeaponMeshComponent;
				WeaponMeshComponent->SetOverlayMaterial(OverlayMaterialInstance);
				CachedMeshComponents.Add(WeaponMeshComponent);
			}
		}
	}
}

void UD1AnimNotifyState_OverlayEffect::ApplyAllWeaponMeshComponents(USkeletalMeshComponent* MeshComponent)
{
	if (ALyraCharacter* LyraCharacter = Cast<ALyraCharacter>(MeshComponent->GetOwner()))
	{
		if (UD1EquipManagerComponent* EquipManager = LyraCharacter->FindComponentByClass<UD1EquipManagerComponent>())
		{
			TArray<AD1WeaponBase*> WeaponActors;
			EquipManager->GetAllEquippedActors(WeaponActors);

			for (AD1WeaponBase* WeaponActor : WeaponActors)
			{
				USkeletalMeshComponent* WeaponMeshComponent = WeaponActor->WeaponMeshComponent;
				WeaponMeshComponent->SetOverlayMaterial(OverlayMaterialInstance);
				CachedMeshComponents.Add(WeaponMeshComponent);
			}
		}
	}
}

void UD1AnimNotifyState_OverlayEffect::ApplyCharacterMeshComponents(USkeletalMeshComponent* MeshComponent)
{
	if (ALyraCharacter* LyraCharacter = Cast<ALyraCharacter>(MeshComponent->GetOwner()))
	{
		TArray<UMeshComponent*> CharacterMeshComponents;
		LyraCharacter->GetMeshComponents(CharacterMeshComponents);

		for (UMeshComponent* CharacterMeshComponent : CharacterMeshComponents)
		{
			CharacterMeshComponent->SetOverlayMaterial(OverlayMaterialInstance);
			CachedMeshComponents.Add(CharacterMeshComponent);
		}
	}
}

void UD1AnimNotifyState_OverlayEffect::Clear()
{
	PassedTime = 0.f;
	
	for (TWeakObjectPtr<UMeshComponent> CachedMeshComponent : CachedMeshComponents)
	{
		if (CachedMeshComponent.IsValid())
		{
			CachedMeshComponent->SetOverlayMaterial(nullptr);
		}
	}

	OverlayMaterialInstance = nullptr;
	CachedMeshComponents.Reset();
}
