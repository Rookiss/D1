#include "D1AnimInstance.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1AnimInstance)

UD1AnimInstance::UD1AnimInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1AnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	if (AActor* OwningActor = GetOwningActor())
	{
		if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OwningActor))
		{
			InitializedWithAbilitySystem(ASC);
		}
	}
}

void UD1AnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	// TODO: Pitch 동기화 구현 필요함
	if (APawn* Pawn = TryGetPawnOwner())
	{
		if (AController* Controller = Pawn->GetController())
		{
			Pitch = (Controller->GetControlRotation() - Pawn->GetActorRotation()).GetNormalized().Pitch;
		}
	}
}

void UD1AnimInstance::InitializedWithAbilitySystem(UAbilitySystemComponent* ASC)
{
	GameplayTagPropertyMap.Initialize(this, ASC);
}
