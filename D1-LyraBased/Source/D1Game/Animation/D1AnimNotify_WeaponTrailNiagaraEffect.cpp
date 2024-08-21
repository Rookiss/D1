#include "D1AnimNotify_WeaponTrailNiagaraEffect.h"

#include "NiagaraComponent.h"
#include "Actors/D1WeaponBase.h"
#include "Character/LyraCharacter.h"
#include "Item/Managers/D1EquipManagerComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1AnimNotify_WeaponTrailNiagaraEffect)

UD1AnimNotify_WeaponTrailNiagaraEffect::UD1AnimNotify_WeaponTrailNiagaraEffect(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
#if WITH_EDITORONLY_DATA
	bShouldFireInEditor = false;
#endif
	
	Template = nullptr;
	LocationOffset.Set(0.0f, 0.0f, 0.0f);
	RotationOffset = FRotator(0.0f, 0.0f, 0.0f);
}

void UD1AnimNotify_WeaponTrailNiagaraEffect::NotifyBegin(class USkeletalMeshComponent* MeshComponent, class UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	USkeletalMeshComponent* WeaponMeshComponent = GetWeaponMeshComponent(MeshComponent);
	Super::NotifyBegin(WeaponMeshComponent ? WeaponMeshComponent : MeshComponent, Animation, TotalDuration, EventReference);

	UpdateNiagaraParameters(WeaponMeshComponent);
}

void UD1AnimNotify_WeaponTrailNiagaraEffect::NotifyTick(USkeletalMeshComponent* MeshComponent, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	USkeletalMeshComponent* WeaponMeshComponent = GetWeaponMeshComponent(MeshComponent);
	Super::NotifyTick(WeaponMeshComponent ? WeaponMeshComponent : MeshComponent, Animation, FrameDeltaTime, EventReference);
	
	UpdateNiagaraParameters(WeaponMeshComponent);
}

void UD1AnimNotify_WeaponTrailNiagaraEffect::NotifyEnd(class USkeletalMeshComponent* MeshComponent, class UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	USkeletalMeshComponent* WeaponMeshComponent = GetWeaponMeshComponent(MeshComponent);
	Super::NotifyEnd(WeaponMeshComponent ? WeaponMeshComponent : MeshComponent, Animation, EventReference);
	
	UpdateNiagaraParameters(WeaponMeshComponent);
}

void UD1AnimNotify_WeaponTrailNiagaraEffect::UpdateNiagaraParameters(USkeletalMeshComponent* WeaponMeshComponent)
{
	if (WeaponMeshComponent == nullptr)
		return;

	UNiagaraComponent* NiagaraComponent = Cast<UNiagaraComponent>(GetSpawnedEffect(WeaponMeshComponent));
	if (NiagaraComponent == nullptr)
		return;

	if (WeaponMeshComponent->DoesSocketExist(StartSocketName))
	{
		NiagaraComponent->SetVectorParameter(StartParameterName, WeaponMeshComponent->GetSocketLocation(StartSocketName));
	}

	if (WeaponMeshComponent->DoesSocketExist(EndSocketName))
	{
		NiagaraComponent->SetVectorParameter(EndParameterName, WeaponMeshComponent->GetSocketLocation(EndSocketName));
	}
}

USkeletalMeshComponent* UD1AnimNotify_WeaponTrailNiagaraEffect::GetWeaponMeshComponent(USkeletalMeshComponent* CharacterMeshComponent) const
{
	USkeletalMeshComponent* WeaponMeshComponent = nullptr;
	
	if (ALyraCharacter* LyraCharacter = Cast<ALyraCharacter>(CharacterMeshComponent->GetOwner()))
	{
		if (UD1EquipManagerComponent* EquipManager = LyraCharacter->FindComponentByClass<UD1EquipManagerComponent>())
		{
			if (AD1WeaponBase* WeaponActor = EquipManager->GetEquippedActor(WeaponHandType))
			{
				WeaponMeshComponent = WeaponActor->WeaponMeshComponent;
			}
		}
	}

	return WeaponMeshComponent;
}
