#include "D1NumberPopComponent_NiagaraText.h"

#include "Feedback/NumberPops/D1NumberPopComponent.h"
#include "D1DamagePopStyleNiagara.h"
#include "NiagaraComponent.h"
#include "NiagaraDataInterfaceArrayFunctionLibrary.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1NumberPopComponent_NiagaraText)

UD1NumberPopComponent_NiagaraText::UD1NumberPopComponent_NiagaraText(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}

void UD1NumberPopComponent_NiagaraText::AddNumberPop(const FD1NumberPopRequest& NewRequest)
{
	int32 LocalDamage = NewRequest.NumberToDisplay;
	
	if (NewRequest.bIsCriticalDamage)
	{
		LocalDamage *= -1;
	}

	if (NiagaraComp == nullptr)
	{
		NiagaraComp = NewObject<UNiagaraComponent>(GetOwner());
		if (Style)
		{
			NiagaraComp->SetAsset(Style->TextNiagara);
			NiagaraComp->bAutoActivate = false;
			
		}
		
		NiagaraComp->SetupAttachment(nullptr);
		check(NiagaraComp);
		NiagaraComp->RegisterComponent();
	}

	NiagaraComp->Activate(false);
	NiagaraComp->SetWorldLocation(NewRequest.WorldLocation);
	
	// XYZ = Position, W = Damage
	TArray<FVector4> DamageList = UNiagaraDataInterfaceArrayFunctionLibrary::GetNiagaraArrayVector4(NiagaraComp, Style->NiagaraArrayName);
	DamageList.Add(FVector4(NewRequest.WorldLocation.X, NewRequest.WorldLocation.Y, NewRequest.WorldLocation.Z, LocalDamage));
	UNiagaraDataInterfaceArrayFunctionLibrary::SetNiagaraArrayVector4(NiagaraComp, Style->NiagaraArrayName, DamageList);
}
