#pragma once

#include "Engine/DataAsset.h"
#include "D1TeamDisplayAsset.generated.h"

struct FPropertyChangedEvent;

UCLASS(BlueprintType)
class UD1TeamDisplayAsset : public UDataAsset
{
	GENERATED_BODY()

public:
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

public:
	UFUNCTION(BlueprintCallable, Category=Teams)
	FLinearColor GetColorByName(FName Name) const;
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TMap<FName, FLinearColor> Colors;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText TeamShortName;
};
