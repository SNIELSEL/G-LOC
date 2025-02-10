// Copyright Epic Games, Inc. All Rights Reserved.

#include "Untitled_race_gameWheelRear.h"
#include "UObject/ConstructorHelpers.h"

UUntitled_race_gameWheelRear::UUntitled_race_gameWheelRear()
{
	AxleType = EAxleType::Rear;
	bAffectedByHandbrake = true;
	bAffectedByEngine = true;
}