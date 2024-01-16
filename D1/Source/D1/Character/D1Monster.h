#pragma once

#include "D1Character.h"
#include "D1Monster.generated.h"

class UD1MonsterSet;
class UD1AbilitySystemComponent;

UCLASS()
class AD1Monster : public AD1Character
{
	GENERATED_BODY()
	
public:
	AD1Monster(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	virtual void BeginPlay() override;
	virtual void InitAbilityActorInfo() override;
	
public:
	const UD1MonsterSet* GetPrimarySet() const;
	
protected:
	UPROPERTY()
	TObjectPtr<const UD1MonsterSet> MonsterSet;
};
