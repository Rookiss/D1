#pragma once

#include "CoreMinimal.h"

class FGladiatorCoreRuntimeModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
