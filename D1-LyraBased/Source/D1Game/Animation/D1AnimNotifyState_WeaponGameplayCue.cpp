#include "D1AnimNotifyState_WeaponGameplayCue.h"

#include "AbilitySystemGlobals.h"
#include "Actors/D1WeaponBase.h"
#include "Character/LyraCharacter.h"
#include "Item/Managers/D1EquipManagerComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1AnimNotifyState_WeaponGameplayCue)

typedef void (*GameplayCueFunc)(AActor* Target, const FGameplayTag GameplayCueTag, const FGameplayCueParameters& Parameters);

static void ProcessGameplayCue(GameplayCueFunc Func, USkeletalMeshComponent* MeshComp, FGameplayTag GameplayCue, UAnimSequenceBase* Animation)
{
	if (MeshComp && GameplayCue.IsValid())
	{
		AActor* OwnerActor = MeshComp->GetOwner();

#if WITH_EDITOR
		if (GIsEditor && (OwnerActor == nullptr))
		{
			// Make preview work in anim preview window
			UGameplayCueManager::PreviewComponent = MeshComp;
			UGameplayCueManager::PreviewWorld = MeshComp->GetWorld();

			if (UGameplayCueManager* GCM = UAbilitySystemGlobals::Get().GetGameplayCueManager())
			{
				GCM->LoadNotifyForEditorPreview(GameplayCue);
			}
		}
#endif // #if WITH_EDITOR

		FGameplayCueParameters Parameters;
		Parameters.Instigator = OwnerActor;

		// Try to get the ability level. This may not be able to find the ability level
		// in cases where a blend out is happening or two abilities are trying to play animations
		if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OwnerActor))
		{
			if (ASC->GetCurrentMontage() == Animation)
			{
				if (const UGameplayAbility* Ability = ASC->GetAnimatingAbility())
				{
					Parameters.AbilityLevel = Ability->GetAbilityLevel();
				}
			}

			// Always use ASC's owner for instigator
			Parameters.Instigator = ASC->GetOwner();
		}

		Parameters.TargetAttachComponent = MeshComp;

		(*Func)(OwnerActor, GameplayCue, Parameters);
	}

#if WITH_EDITOR
	if (GIsEditor)
	{
		UGameplayCueManager::PreviewComponent = nullptr;
		UGameplayCueManager::PreviewWorld = nullptr;
	}
#endif // #if WITH_EDITOR
}

UD1AnimNotifyState_WeaponGameplayCue::UD1AnimNotifyState_WeaponGameplayCue()
{
#if WITH_EDITORONLY_DATA
	bShouldFireInEditor = false;
#endif
}

void UD1AnimNotifyState_WeaponGameplayCue::NotifyBegin(USkeletalMeshComponent* MeshComponent, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	USkeletalMeshComponent* WeaponMeshComponent = GetWeaponMeshComponent(MeshComponent);
	MeshComponent = WeaponMeshComponent ? WeaponMeshComponent : MeshComponent;
	
	Super::NotifyBegin(MeshComponent, Animation, TotalDuration, EventReference);

	ProcessGameplayCue(&UGameplayCueManager::AddGameplayCue_NonReplicated, MeshComponent, GameplayCue.GameplayCueTag, Animation);

#if WITH_EDITORONLY_DATA
	if (UGameplayCueManager::PreviewProxyTick.IsBound())
	{
		PreviewProxyTick = UGameplayCueManager::PreviewProxyTick;
		UGameplayCueManager::PreviewProxyTick.Unbind();
	}
#endif // #if WITH_EDITORONLY_DATA
}

void UD1AnimNotifyState_WeaponGameplayCue::NotifyTick(USkeletalMeshComponent* MeshComponent, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	USkeletalMeshComponent* WeaponMeshComponent = GetWeaponMeshComponent(MeshComponent);
	MeshComponent = WeaponMeshComponent ? WeaponMeshComponent : MeshComponent;
	
	AActor* OwnerActor = MeshComponent->GetOwner();

#if WITH_EDITOR
	if (GIsEditor && OwnerActor == nullptr)
	{
		// Make preview work in anim preview window.
		UGameplayCueManager::PreviewComponent = MeshComponent;
		UGameplayCueManager::PreviewWorld = MeshComponent->GetWorld();

#if WITH_EDITORONLY_DATA
		PreviewProxyTick.ExecuteIfBound(FrameDeltaTime);
#endif // #if WITH_EDITORONLY_DATA
	}
#endif // #if WITH_EDITOR

	Super::NotifyTick(MeshComponent, Animation, FrameDeltaTime, EventReference);

#if WITH_EDITOR
	if (GIsEditor)
	{
		UGameplayCueManager::PreviewComponent = nullptr;
		UGameplayCueManager::PreviewWorld = nullptr;
	}
#endif // #if WITH_EDITOR
}

void UD1AnimNotifyState_WeaponGameplayCue::NotifyEnd(USkeletalMeshComponent* MeshComponent, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	USkeletalMeshComponent* WeaponMeshComponent = GetWeaponMeshComponent(MeshComponent);
	MeshComponent = WeaponMeshComponent ? WeaponMeshComponent : MeshComponent;
	
	ProcessGameplayCue(&UGameplayCueManager::RemoveGameplayCue_NonReplicated, MeshComponent, GameplayCue.GameplayCueTag, Animation);
}

FString UD1AnimNotifyState_WeaponGameplayCue::GetNotifyName_Implementation() const
{
	FString DisplayName = TEXT("Weapon GameplayCue");

	if (GameplayCue.GameplayCueTag.IsValid())
	{
		DisplayName = GameplayCue.GameplayCueTag.ToString();
		DisplayName += TEXT(" (Looping)");
	}

	return DisplayName;
}

bool UD1AnimNotifyState_WeaponGameplayCue::CanBePlaced(UAnimSequenceBase* Animation) const
{
	return (Animation && Animation->IsA(UAnimMontage::StaticClass()));
}

USkeletalMeshComponent* UD1AnimNotifyState_WeaponGameplayCue::GetWeaponMeshComponent(USkeletalMeshComponent* CharacterMeshComponent) const
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
