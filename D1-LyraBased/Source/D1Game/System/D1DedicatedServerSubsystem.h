#pragma once

#include "D1DedicatedServerSubsystem.generated.h"

UCLASS()
class UD1DedicatedServerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	UD1DedicatedServerSubsystem();

public:
	UFUNCTION(BlueprintCallable, Category = "DedicatedServer")
	void CreateDedicatedServerSession(FString InMapPath);

	UFUNCTION()
	void OnSessionCreationReply(FName SessionName, bool bWasSuccessful);

public:
	UPROPERTY()
	FString MapPath;
};
