#pragma once

#include "D1Define.h"
#include "GenericTeamAgentInterface.h"
#include "UObject/Object.h"
#include "UObject/WeakObjectPtr.h"
#include "D1TeamAgentInterface.generated.h"

template <typename InterfaceType> class TScriptInterface;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnD1TeamIndexChangedDelegate, UObject*, ObjectChangingTeam, int32, OldTeamID, int32, NewTeamID);

inline int32 GenericTeamIdToInteger(FGenericTeamId ID)
{
	return (int32)ID;
}

inline FGenericTeamId IntegerToGenericTeamId(int32 ID)
{
	return (ID == INDEX_NONE) ? FGenericTeamId::NoTeam : FGenericTeamId((uint8)ID);
}

inline FGenericTeamId EnumToGenericTeamId(ED1TeamID ID)
{
	return (ID == ED1TeamID::NoTeam) ? FGenericTeamId::NoTeam : FGenericTeamId((uint8)ID);
}

UINTERFACE(meta=(CannotImplementInterfaceInBlueprint))
class UD1TeamAgentInterface : public UGenericTeamAgentInterface
{
	GENERATED_UINTERFACE_BODY()
};

class D1GAME_API ID1TeamAgentInterface : public IGenericTeamAgentInterface
{
	GENERATED_IINTERFACE_BODY()

public:
	virtual FOnD1TeamIndexChangedDelegate* GetOnTeamIndexChangedDelegate() { return nullptr; }
	virtual ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const override;

	static void ConditionalBroadcastTeamChanged(TScriptInterface<ID1TeamAgentInterface> This, FGenericTeamId OldTeamID, FGenericTeamId NewTeamID);
	
	FOnD1TeamIndexChangedDelegate& GetTeamChangedDelegateChecked()
	{
		FOnD1TeamIndexChangedDelegate* Result = GetOnTeamIndexChangedDelegate();
		check(Result);
		return *Result;
	}
};
