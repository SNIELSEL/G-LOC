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
	class UCameraComponent* CameraC;

protected:
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

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
	class UInputAction* SteerLeftAction;

	UPROPERTY(EditAnywhere, Category = "EnhancedInput")
	class UInputAction* SteerRightAction;

	//empty/cheap objects for start and end point for raycast/line trace so that i can use those and use their attachment feature to make my raycast automatically rotate
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		USceneComponent* LineTraceParent;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true", AdvancedDisplay))
		USceneComponent* LeftLineTrace;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true", AdvancedDisplay))
		USceneComponent* RightLineTrace;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true", AdvancedDisplay))
		USceneComponent* FrontLineTrace;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true", AdvancedDisplay))
		USceneComponent* BackLineTrace;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true", AdvancedDisplay))
		USceneComponent* CenterLineTrace;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true", AdvancedDisplay))
		USceneComponent* LeftWallLineTraceStart;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true", AdvancedDisplay))
		USceneComponent* LeftWallLineTraceEnd;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true", AdvancedDisplay))
		USceneComponent* RightWallLineTraceStart;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true", AdvancedDisplay))
		USceneComponent* RightWallLineTraceEnd;
private:

	//variables
	float LinetraceStartHeight = 15;
	float LinetraceEndHeight = -900;

	//hover
	float DesiredHoverHeight = 190;
	float HoverForceCoefficient = 700;
	float HoverDamping = 110;

	FVector CurrentHoverForce;
	FVector HoverPreviousForce;

	//engine
	float EngineForceCoefficient = 14000;

	//rotation
	float RotateSpeed = 8;
	float SteeringTorqueCoefficient = 150;
	float MaxSteeringAngularVelocity = 70;

	float RollInterpSpeed = 8;
	float MaxRollAngle = 50;

	//brakes
	float BrakingTorqueConstant = 130;
	float BrakeForceCoefficient = 90;
	float BrakeForceMultiplier = 10000000;

	//bools
	bool steerLeft = false;
	bool steerRight = false;

	//input
	float ThrottleInput;
	float SteeringInput;
	bool bBraking;

	//camera
	float CameraFollowDistance = 300;
	float CameraHeight = 190;
	float CameraInterpSpeedLocation = 20;
	float CameraInterpSpeedRotation = 8;

	FVector LastCameraPosition;
	FRotator LastCameraRotation;
	FVector DesiredCameraPosition;
	FRotator DesiredCameraLookAt;

	//void
	virtual void BeginPlay() override;
	void Tick(float DeltaTime) override;
	//Movement
	void Accelerate(const FInputActionValue& Value);
	void StopAccelerate(const FInputActionValue& Value);
	void Decelerate(const FInputActionValue& Value);
	void StopDecelerate(const FInputActionValue& Value);
	void SteerLeftPressed();
	void SteerLeftReleased();
	void SteerRightPressed();
	void SteerRightReleased();
	void BrakePressed();
	void BrakeReleased();

	//Look
	void LookVertical(float value);
	void LookHorizontal(float value);
};