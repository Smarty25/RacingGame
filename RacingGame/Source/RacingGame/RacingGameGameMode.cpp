// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "RacingGameGameMode.h"
#include "RacingGamePawn.h"
#include "RacingGameHud.h"

ARacingGameGameMode::ARacingGameGameMode()
{
	DefaultPawnClass = ARacingGamePawn::StaticClass();
	HUDClass = ARacingGameHud::StaticClass();
}
