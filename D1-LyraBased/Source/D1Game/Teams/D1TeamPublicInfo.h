#pragma once

#include "D1TeamInfoBase.h"
#include "D1TeamPublicInfo.generated.h"

class UD1TeamCreationComponent;
class UD1TeamDisplayAsset;

UCLASS()
class AD1TeamPublicInfo : public AD1TeamInfoBase
{
	GENERATED_BODY()

	friend UD1TeamCreationComponent;

public:
	AD1TeamPublicInfo(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	UD1TeamDisplayAsset* GetTeamDisplayAsset() const { return TeamDisplayAsset; }

private:
	UFUNCTION()
	void OnRep_TeamDisplayAsset();

	void SetTeamDisplayAsset(TObjectPtr<UD1TeamDisplayAsset> NewDisplayAsset);

private:
	UPROPERTY(ReplicatedUsing=OnRep_TeamDisplayAsset)
	TObjectPtr<UD1TeamDisplayAsset> TeamDisplayAsset;
};
