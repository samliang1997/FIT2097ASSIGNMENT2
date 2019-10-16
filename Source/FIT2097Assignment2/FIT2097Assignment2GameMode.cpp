// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "FIT2097Assignment2GameMode.h"
#include "FIT2097Assignment2HUD.h"
#include "FIT2097Assignment2Character.h"
#include "UObject/ConstructorHelpers.h"

AFIT2097Assignment2GameMode::AFIT2097Assignment2GameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AFIT2097Assignment2HUD::StaticClass();
}
