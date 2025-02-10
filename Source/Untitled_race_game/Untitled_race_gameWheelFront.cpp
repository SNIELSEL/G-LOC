// Copyright Epic Games, Inc. All Rights Reserved.

#include "Untitled_race_gameWheelFront.h"
#include "UObject/ConstructorHelpers.h"

UUntitled_race_gameWheelFront::UUntitled_race_gameWheelFront()
{
	AxleType = EAxleType::Front;
	bAffectedBySteering = true;
	MaxSteerAngle = 40.f;
}