#pragma once

#include "AbilitySystemInterface.h"
#include "Data/D1AbilitySystemData.h"
#include "GameFramework/Character.h"
#include "D1Character.generated.h"

class UD1PrimarySet;
class UD1AbilitySystemComponent;

UCLASS()
class AD1Character : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:
	AD1Character(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	virtual void InitAbilityActorInfo();
	virtual void ApplyAbilitySystemData(const FName& DataName);

public:
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
protected:
	UPROPERTY()
	FD1AbilitySystemData_GrantedHandles GrantedHandles;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName InitialAbilitySystemDataName;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UD1AbilitySystemComponent> D1ASC;
};
