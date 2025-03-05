// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "InputActionValue.h"
#include "MainCar.generated.h"

UCLASS()
class UNTITLED_RACE_GAME_API AMainCar : public APawn
{
	GENERATED_BODY()
	
public:

	AMainCar();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		class UStaticMeshComponent* CarMesh;
	UPROPERTY(BluePrintReadWrite, EditAnywhere)
		class USpringArmComponent* SpringArmC;
	UPROPERTY(BluePrintReadWrite, EditAnywhere)
		class UCameraComponent* CameraC;

protected:
	virtual void SetupPlayerInputComponent(UInputComponent * PlayerInputComponent) override;

	UPROPERTY(EditAnywhere, Category = "EnhancedInput")
	class UInputMappingContext* InputMapping;

	UPROPERTY(EditAnywhere, Category = "EnhancedInput")
	class UInputAction* PressBrake;

	UPROPERTY(EditAnywhere, Category = "EnhancedInput")
	class UInputAction* ReleaseBrake;

	UPROPERTY(EditAnywhere, Category = "EnhancedInput")
	class UInputAction* MoveForwards;

	UPROPERTY(EditAnywhere, Category = "EnhancedInput")
	class UInputAction* MoveBackwards;

	UPROPERTY(EditAnywhere, Category = "EnhancedInput")
	class UInputAction* HorizontalMovement;
private:

	//variables
	const float DesiredHoverHeight = 210.0f;
	const float HoverForceCoefficient = 700.0f;
	const float HoverDamping = 110.0f;

	const float EngineForceCoefficient = 7000.0f;
	const float SteeringTorqueCoefficient = 100.0f;

	float AccelerationForce = 10000.f;
	float SteeringTorque = 500000.f;
	float BrakeForceMultiplier = 5000.f;
	float ThrottleInput;
	float SteeringInput;
	bool bBraking;

	void Tick(float DeltaTime);
	//Movement
	void Accelerate(const FInputActionValue& Value);
	void StopAccelerate(const FInputActionValue& Value);
	void Decelerate(const FInputActionValue& Value);
	void StopDecelerate(const FInputActionValue& Value);
	void MoveHorizontal(const FInputActionValue& Value);
	void StopSteering(const FInputActionValue& Value);
	void BrakePressed();
	void BrakeReleased();

	//Look
	void LookVertical(float value);
	void LookHorizontal(float value);
};
