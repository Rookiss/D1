#include "D1GameInstance.h"

#include "D1AssetManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1GameInstance)

UD1GameInstance::UD1GameInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1GameInstance::Init()
{
	Super::Init();

	UD1AssetManager::Initialize();
}

void UD1GameInstance::Shutdown()
{
	Super::Shutdown();
}
