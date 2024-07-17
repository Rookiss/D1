#include "D1WorldInteractable.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1WorldInteractable)

AD1WorldInteractable::AD1WorldInteractable(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bReplicates = true;
}

void AD1WorldInteractable::OnInteractionStarted()
{
	K2_OnInteractionStarted();
}

void AD1WorldInteractable::OnInteractionFailed()
{
	K2_OnInteractionFailed();
}

void AD1WorldInteractable::OnInteractionSucceeded()
{
	K2_OnInteractionSucceeded();
}
