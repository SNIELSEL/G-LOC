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

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
		class UStaticMeshComponent* CarMesh;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		class USpringArmComponent* SpringArmC;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
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

	//empty/cheap objects for start and end point for raycast/line trace so that i can use those and use their attachment feature to make my raycast automatically rotate
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		USceneComponent* LineTraceParent;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true", AdvancedDisplay))
		USceneComponent* LeftLineTraceStart;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true", AdvancedDisplay))
		USceneComponent* RightLineTraceStart;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true", AdvancedDisplay))
		USceneComponent* FrontLineTraceStart;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true", AdvancedDisplay))
		USceneComponent* BackLineTraceStart;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true", AdvancedDisplay))
		USceneComponent* CenterLineTraceStart;
private:

	//variables
	float LinetraceStartHeight = 50;
	float LinetraceEndHeight = -900;

	float DesiredHoverHeight = 220.0f;
	float HoverForceCoefficient = 800.0f;
	float HoverDamping = 90.0f;

	float EngineForceCoefficient = 14000.0f;
	float SteeringTorqueCoefficient = 75.0f;

	float SteeringTorque = 2000000000.f;
	float BrakeForceCoefficient = 90.0f;
	float BrakeForceMultiplier = 10000000;
	float ThrottleInput;
	float SteeringInput;
	bool bBraking;

	void Tick(float DeltaTime) override;
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
