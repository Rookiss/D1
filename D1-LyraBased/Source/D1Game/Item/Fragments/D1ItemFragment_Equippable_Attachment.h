#pragma once

#include "D1ItemFragment_Equippable.h"
#include "D1ItemFragment_Equippable_Attachment.generated.h"

class AD1WeaponBase;

USTRUCT(BlueprintType)
struct FD1WeaponAttachInfo
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AD1WeaponBase> SpawnWeaponClass;

	UPROPERTY(EditDefaultsOnly)
	FName AttachSocket;

	UPROPERTY(EditDefaultsOnly)
	FTransform AttachTransform;
};

UCLASS(Abstract, Const)
class UD1ItemFragment_Equippable_Attachment : public UD1ItemFragment_Equippable
{
	GENERATED_BODY()
	
public:
	UD1ItemFragment_Equippable_Attachment(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	UPROPERTY(EditDefaultsOnly)
	EWeaponHandType WeaponHandType = EWeaponHandType::Count;

	UPROPERTY(EditDefaultsOnly)
	FD1WeaponAttachInfo WeaponAttachInfo;

	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UAnimMontage> EquipMontage;

	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UAnimMontage> FrontHitMontage;
	
	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UAnimMontage> BackHitMontage;

	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UAnimMontage> LeftHitMontage;

	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UAnimMontage> RightHitMontage;

	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UAnimMontage> BlockHitMontage;

	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UAnimSequence> PocketWorldAnim;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UAnimInstance> AnimInstanceClass;
};
