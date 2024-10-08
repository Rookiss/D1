#pragma once

#include "Components/ControllerComponent.h"
#include "GameplayTagContainer.h"
#include "D1NumberPopComponent.generated.h"

USTRUCT(BlueprintType)
struct FD1NumberPopRequest
{
	GENERATED_BODY()

public:
	FD1NumberPopRequest()
		: WorldLocation(ForceInitToZero) { }

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "D1|Number Pops")
	FVector WorldLocation;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "D1|Number Pops")
	FGameplayTagContainer SourceTags;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="D1|Number Pops")
	FGameplayTagContainer TargetTags;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "D1|Number Pops")
	int32 NumberToDisplay = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "D1|Number Pops")
	bool bIsCriticalDamage = false;
};

UCLASS(Abstract)
class UD1NumberPopComponent : public UControllerComponent
{
	GENERATED_BODY()

public:
	UD1NumberPopComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	UFUNCTION(BlueprintCallable)
	virtual void AddNumberPop(const FD1NumberPopRequest& NewRequest) { }
};
