#include "D1TeamDisplayAsset.h"

#include "Components/MeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Engine/Texture.h"
#include "Teams/D1TeamSubsystem.h"
#include "UObject/UObjectIterator.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1TeamDisplayAsset)

#if WITH_EDITOR
void UD1TeamDisplayAsset::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	for (UD1TeamSubsystem* TeamSubsystem : TObjectRange<UD1TeamSubsystem>())
	{
		TeamSubsystem->NotifyTeamDisplayAssetModified(this);
	}
}
#endif

FLinearColor UD1TeamDisplayAsset::GetColorByName(FName Name) const
{
	const FLinearColor* Color = Colors.Find(Name);
	return Color ? *Color : FLinearColor::Black;
}
