#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "InputActionValue.h"
#include "Blueprint/UserWidget.h"
#include "../UI/IngameUI.h"

#include "MainCar.generated.h"

UCLASS()
class UNTITLED_RACE_GAME_API AMainCar : public APawn
{
	GENERATED_BODY()

public:

	AMainCar();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	class UStaticMeshComponent* FlameTrailMeshL;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	class UStaticMeshComponent* FlameTrailMeshR;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	class UStaticMeshComponent* CarMesh;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UCameraComponent* CameraC;

protected:
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	//HUD
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<class UIngameUI> PlayerHUDClass;

	UPROPERTY()
	class UIngameUI* PlayerHUD;

	//Input
	UPROPERTY(EditAnywhere, Category = "EnhancedInput")
	class UInputMappingContext* InputMapping;

	UPROPERTY(EditAnywhere, Category = "EnhancedInput")
	class UInputAction* PressBrake;

	UPROPERTY(EditAnywhere, Category = "EnhancedInput")
	class UInputAction* ReleaseBrake;

	UPROPERTY(EditAnywhere, Category = "EnhancedInput")
	class UInputAction* PressBoost;

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
		USceneComponent* CenterLineTrace;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true", AdvancedDisplay))
		USceneComponent* CenterLineTraceEnd;
private:

	//variables
	float LinetraceStartHeight = 0;
	float LinetraceEndHeight = -900;

	//hover
	float DesiredHoverHeight = 130;
	float HoverForceCoefficient = 2;
	float HoverDamping = 110;

	float HoverInterpSpeed = 20;

	//engine
	float EngineForceCoefficient = 19000;
	float MaxSpeedForClamp = 4000;

	float CurrentThrottle = 0;
	float TargetThrottle = 0;
	float AccelerationSpeed = 0.8f;

	//Boost
	float MaxBoost;
	float CurrentBoost;
	float BoostForceCoefficient = 5000;
	float BoostDrainRate = 32;
	float BoostRechargeRate = 7.5f;
	bool bIsBoosting;

	//rotation
	float SteeringTorqueCoefficient = 9;
	float MaxSteeringAngularVelocity = 120;

	float AlignmentTorqueCoefficient = 1500;
	float DampingCoefficient = 30.0f;
	//brakes
	float BrakingTorqueConstant = 180;
	float BrakeForceCoefficient = 0.5f;

	//bools
	bool steerLeft = false;
	bool steerRight = false;

	//input
	float ThrottleInput;
	float SteeringInput;
	bool bBraking;

	//camera
	float CameraFollowDistance = 200;
	float CameraHeight = 110;
	float CameraInterpSpeedLocation = 50;
	float CameraInterpSpeedRotation = 4;
	float CameraPitchOffset = -2;

	//FOV
	float BaseFOV = 100;
	float MaxFOV = 140; 
	float FOVInterpSpeed = 4;

	//FlameMesh
	float MinFlameScale = 0.01f;
	float MaxFlameScale = 1.0f;
	float ScaleInterpSpeed = 4;

	FVector LastCameraPosition;
	FRotator LastCameraRotation;
	FVector DesiredCameraPosition;
	FRotator DesiredCameraLookAt;

	//void
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void Tick(float DeltaTime) override;
	void CameraMovement(float DeltaTime);
	void UpdateThrottle(float DeltaTime);

	//Movement
	void Boosting(const FInputActionValue& Value);
	void StopBoosting(const FInputActionValue& Value);
	void UpdateBoost(float DeltaTime);
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