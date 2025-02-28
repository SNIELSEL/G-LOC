// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WheeledVehiclePawn.h"
#include "MainCar.generated.h"


UCLASS()
class UNTITLED_RACE_GAME_API AMainCar : public AWheeledVehiclePawn
{
	GENERATED_BODY()
	
public:

	AMainCar();

	UPROPERTY(BluePrintReadWrite, EditAnywhere)
		class USpringArmComponent* SpringArmC;
	UPROPERTY(BluePrintReadWrite, EditAnywhere)
		class UCameraComponent* CameraC;
protected:
	virtual void SetupPlayerInputComponent(UInputComponent * PlayerInputComponent) override;
private:

	//Movement
	void Accelerate(float value);
	void Decelerate(float value);
	void MoveHorizontal(float value);
	void BrakePressed();
	void BrakeReleased();

	//Look
	void LookVertical(float value);
	void LookHorizontal(float value);

	//End Movement

};
