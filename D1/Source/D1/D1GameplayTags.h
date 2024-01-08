#pragma once

#include "NativeGameplayTags.h"

namespace D1GameplayTags
{
	FGameplayTag FindTagByString(const FString& TagString, bool bMatchPartialString = false);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(ASC_DamageImmunity);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(ASC_InputBlocked);
	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Action_Move);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Action_Look);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Action_Jump);
}
