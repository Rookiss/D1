#pragma once

#include "Logging/LogMacros.h"

D1GAME_API DECLARE_LOG_CATEGORY_EXTERN(LogD1, Log, All);
D1GAME_API DECLARE_LOG_CATEGORY_EXTERN(LogD1Experience, Log, All);
D1GAME_API DECLARE_LOG_CATEGORY_EXTERN(LogD1AbilitySystem, Log, All);
D1GAME_API DECLARE_LOG_CATEGORY_EXTERN(LogD1Team, Log, All);

#define CALLINFO (FString(__FUNCTION__) + TEXT("(") + FString::FromInt(__LINE__) + TEXT(")"))
#define LOG_CALLINFO(Verbosity) UE_LOG(LogD1, Verbosity, TEXT("%s"), *CALLINFO)

#define LOG(Verbosity, Format, ...) UE_LOG(LogD1, Verbosity, TEXT("%s %s"), *CALLINFO, *FString::Printf(Format, ##__VA_ARGS__))
#define LOG_WARNING(Format, ...) UE_LOG(LogD1, Warning, TEXT("%s %s"), *CALLINFO, *FString::Printf(Format, ##__VA_ARGS__))
#define LOG_ERROR(Format, ...) UE_LOG(LogD1, Error, TEXT("%s %s"), *CALLINFO, *FString::Printf(Format, ##__VA_ARGS__))

#define	LOG_SCREEN(Format, ...) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(Format, ##__VA_ARGS__))
#define	LOG_SCREEN_CONTEXT(ContextObject, Format, ...) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("%s: %s"), *GetClientServerContextString(ContextObject), *FString::Printf(Format, ##__VA_ARGS__)))
#define	LOG_SCREEN_ORDER(Order, Format, ...) GEngine->AddOnScreenDebugMessage(Order, 5.f, FColor::Red, FString::Printf(Format, ##__VA_ARGS__))
#define	LOG_SCREEN_COLOR(Color, Format, ...) GEngine->AddOnScreenDebugMessage(-1, 5.f, Color, FString::Printf(Format, ##__VA_ARGS__))

D1GAME_API FString GetClientServerContextString(UObject* ContextObject = nullptr);
