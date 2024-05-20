#include "Modules/ModuleManager.h"

class FD1GameModule : public FDefaultGameModuleImpl
{
	virtual void StartupModule() override
	{
		
	}

	virtual void ShutdownModule() override
	{
		
	}
};

IMPLEMENT_PRIMARY_GAME_MODULE(FD1GameModule, D1Game, "D1Game");
