#include "D1AnimNotify_SendWeaponEvent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Actors/D1WeaponBase.h"
#include "Character/LyraCharacter.h"
#include "Item/Managers/D1EquipManagerComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1AnimNotify_SendWeaponEvent)

UD1AnimNotify_SendWeaponEvent::UD1AnimNotify_SendWeaponEvent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
#if WITH_EDITORONLY_DATA
	bShouldFireInEditor = false;
#endif
	bIsNativeBranchingPoint = true;
}

void UD1AnimNotify_SendWeaponEvent::Notify(USkeletalMeshComponent* MeshComponent, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComponent, Animation, EventReference);

	if (ALyraCharacter* Character = Cast<ALyraCharacter>(MeshComponent->GetOwner()))
	{
		if (UD1EquipManagerComponent* EquipManager = Character->FindComponentByClass<UD1EquipManagerComponent>())
		{
			const TArray<FD1EquipEntry>& Entries = EquipManager->GetAllEntries();
			AD1WeaponBase* WeaponActor = Entries[(int32)EquipManager->ConvertToEquipmentSlotType(WeaponHandType)].SpawnedWeaponActor;

			if (WeaponActor && EventData.EventTag.IsValid())
			{
				UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(WeaponActor, EventData.EventTag, EventData);
			}
		}
	}
}
