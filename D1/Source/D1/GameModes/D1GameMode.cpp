#include "D1GameMode.h"

#include "D1GameState.h"
#include "Character/D1Player.h"
#include "Player/D1PlayerController.h"
#include "Player/D1PlayerState.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1GameMode)

AD1GameMode::AD1GameMode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    GameStateClass = AD1GameState::StaticClass();
	DefaultPawnClass = AD1Player::StaticClass();
	PlayerStateClass = AD1PlayerState::StaticClass();
	PlayerControllerClass = AD1PlayerController::StaticClass();
}
