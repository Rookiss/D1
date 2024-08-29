#pragma once

#include "Framework/Commands/Commands.h"
#include "Styling/AppStyle.h"
 
#define LOCTEXT_NAMESPACE "D1CustomCommands"

class FD1CustomCommands : public TCommands<FD1CustomCommands>
{
public:
	FD1CustomCommands()
		: TCommands<FD1CustomCommands>(
			// Unique name of the commands set
			"D1CustomCommands",
 
			// Human readable name (will be displayed in the editor preferences window)
			LOCTEXT("D1CustomCommandsName", "D1 Custom Commands"),
 
			// Name of the parent commands set this one is extending (if any)
			NAME_None,
 
			// Name of the style set from which command icons should be loaded (if any)
			FAppStyle::GetAppStyleSetName()
		)
	{}
 
	// Commands should be declared as members
	TSharedPtr<FUICommandInfo> Reimport;
 
	// RegisterCommands should be overridden to define the commands
	void RegisterCommands() override
	{
		UI_COMMAND(Reimport, "Reimport", "Reimport current selection", EUserInterfaceActionType::Button, FInputChord(EKeys::R, EModifierKey::Control));
	}
};
 
#undef LOCTEXT_NAMESPACE
