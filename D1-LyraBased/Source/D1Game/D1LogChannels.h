#pragma once

#include "Logging/LogMacros.h"

D1GAME_API DECLARE_LOG_CATEGORY_EXTERN(LogD1, Log, All);
D1GAME_API DECLARE_LOG_CATEGORY_EXTERN(LogD1Experience, Log, All);
D1GAME_API DECLARE_LOG_CATEGORY_EXTERN(LogD1AbilitySystem, Log, All);
D1GAME_API DECLARE_LOG_CATEGORY_EXTERN(LogD1Team, Log, All);

D1GAME_API FString GetClientServerContextString(UObject* ContextObject = nullptr);
