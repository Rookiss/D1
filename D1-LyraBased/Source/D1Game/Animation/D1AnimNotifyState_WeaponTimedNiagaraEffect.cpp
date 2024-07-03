#include "D1AnimNotifyState_WeaponTimedNiagaraEffect.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Actors/D1WeaponBase.h"
#include "Character/LyraCharacter.h"
#include "Item/Managers/D1EquipManagerComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1AnimNotifyState_WeaponTimedNiagaraEffect)

UD1AnimNotifyState_WeaponTimedNiagaraEffect::UD1AnimNotifyState_WeaponTimedNiagaraEffect(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	Template = nullptr;
	LocationOffset.Set(0.0f, 0.0f, 0.0f);
	RotationOffset = FRotator(0.0f, 0.0f, 0.0f);
}

void UD1AnimNotifyState_WeaponTimedNiagaraEffect::NotifyBegin(USkeletalMeshComponent* MeshComponent, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	if (ALyraCharacter* Character = Cast<ALyraCharacter>(MeshComponent->GetOwner()))
	{
		if (UD1EquipManagerComponent* EquipManager = Character->FindComponentByClass<UD1EquipManagerComponent>())
		{
			if (AD1WeaponBase* WeaponActor = EquipManager->GetEquippedWeaponActor(WeaponHandType))
			{
				CachedWeaponActor = WeaponActor;
				
				if (UFXSystemComponent* FXSystem = SpawnEffect(WeaponActor->WeaponMeshComponent, Animation))
				{
					FXSystem->ComponentTags.AddUnique(GetSpawnedComponentTag());
				}
			}
		}
	}
	
	Super::NotifyBegin(MeshComponent, Animation, TotalDuration, EventReference);
}

void UD1AnimNotifyState_WeaponTimedNiagaraEffect::NotifyEnd(USkeletalMeshComponent* MeshComponent, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	if (AD1WeaponBase* WeaponActor = CachedWeaponActor.Get())
	{
		if (UFXSystemComponent* FXComponent = GetSpawnedEffect(WeaponActor->WeaponMeshComponent))
		{
			FXComponent->ComponentTags.Remove(GetSpawnedComponentTag());
			
			if (bDestroyAtEnd)
			{
				FXComponent->DestroyComponent();
			}
			else
			{
				FXComponent->Deactivate();
			}
		}
	}
	
	Super::NotifyEnd(MeshComponent, Animation, EventReference);
}

UFXSystemComponent* UD1AnimNotifyState_WeaponTimedNiagaraEffect::SpawnEffect(USkeletalMeshComponent* MeshComponent, UAnimSequenceBase* Animation) const
{
	if (AD1WeaponBase* WeaponActor = CachedWeaponActor.Get())
	{
		if (ValidateParameters(WeaponActor->WeaponMeshComponent))
		{
			if (UNiagaraComponent* NewComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(Template, WeaponActor->WeaponMeshComponent, SocketName, LocationOffset, RotationOffset, EAttachLocation::KeepRelativeOffset, !bDestroyAtEnd))
			{
				if (bApplyRateScaleAsTimeDilation)
				{
					NewComponent->SetCustomTimeDilation(Animation->RateScale);
				}
				return NewComponent;
			}
		}
	}
	
	return nullptr;
}

UFXSystemComponent* UD1AnimNotifyState_WeaponTimedNiagaraEffect::GetSpawnedEffect(UMeshComponent* MeshComponent)
{
	if (AD1WeaponBase* WeaponActor = CachedWeaponActor.Get())
	{
		TArray<USceneComponent*> Children;
		WeaponActor->WeaponMeshComponent->GetChildrenComponents(false, Children);

		if (Children.Num())
		{
			for (USceneComponent* Component : Children)
			{
				if (Component && Component->ComponentHasTag(GetSpawnedComponentTag()))
				{
					if (UFXSystemComponent* FXComponent = CastChecked<UFXSystemComponent>(Component))
					{
						return FXComponent;
					}
				}
			}
		}
	}
	
	return nullptr;
}

FString UD1AnimNotifyState_WeaponTimedNiagaraEffect::GetNotifyName_Implementation() const
{
	if (Template)
	{
		return Template->GetName();
	}
	
	return UAnimNotifyState::GetNotifyName_Implementation();
}

bool UD1AnimNotifyState_WeaponTimedNiagaraEffect::ValidateParameters(USkeletalMeshComponent* MeshComp) const
{
	bool bValid = true;

	if (Template == nullptr)
	{
		bValid = false;
	}
	else if (MeshComp->DoesSocketExist(SocketName) == false && MeshComp->GetBoneIndex(SocketName) == INDEX_NONE)
	{
		bValid = false;
	}

	return bValid;
}
