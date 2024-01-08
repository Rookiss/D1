#pragma once

#include "D1Character.h"
#include "D1Player.generated.h"

class UCameraComponent;
class USpringArmComponent;

UCLASS()
class AD1Player : public AD1Character
{
	GENERATED_BODY()
	
public:
	AD1Player(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	virtual void PostInitializeComponents() override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;
	virtual void InitAbilityActorInfo() override;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UCameraComponent> CameraComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USkeletalMeshComponent> FirstPersonMeshComponent;
};
