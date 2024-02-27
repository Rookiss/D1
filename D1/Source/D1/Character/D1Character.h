#pragma once

#include "Data/D1AbilitySystemData.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "D1Character.generated.h"

class UD1AbilitySystemComponent;

UCLASS()
class AD1Character : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:
	AD1Character(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	// TODO: RefreshAbilityActorInfo() / PossessedBy
	virtual void InitAbilitySystem();
	virtual void ApplyAbilitySystemData(const FName& DataName);

public:
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
protected:
	UPROPERTY()
	FD1AbilitySystemData_GrantedHandles GrantedHandles;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UD1AbilitySystemComponent> AbilitySystemComponent;
};
