// Fill out your copyright notice in the Description page of Project Settings.


#include "MainCar.h"
#include "GameFrameWork/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "ChaosVehicleMovementComponent.h"
#include "DrawDebugHelpers.h"

AMainCar::AMainCar()
{
	SpringArmC = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmC->SetupAttachment(RootComponent);
	SpringArmC->TargetArmLength = 600;
	SpringArmC->SetRelativeLocation(FVector(0, 0, 140));
	SpringArmC->SetRelativeRotation(FRotator(-10, 0, 0));
	SpringArmC->bUsePawnControlRotation = true;
	CameraC = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraC->SetupAttachment(SpringArmC);
	GetMesh()->SetEnableGravity(false);
}

void AMainCar::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(FName("MoveForward"), this, &AMainCar::Accelerate);
	PlayerInputComponent->BindAxis(FName("MoveBackward"), this, &AMainCar::Decelerate);
	PlayerInputComponent->BindAxis(FName("MoveLR"), this, &AMainCar::MoveHorizontal);
	PlayerInputComponent->BindAxis(FName("LookUD"), this, &AMainCar::LookVertical);
	PlayerInputComponent->BindAxis(FName("LookLR"), this, &AMainCar::LookHorizontal);

	PlayerInputComponent->BindAction(FName("Brake"), EInputEvent::IE_Pressed, this, &AMainCar::BrakePressed);
	PlayerInputComponent->BindAction(FName("Brake"), EInputEvent::IE_Released, this, &AMainCar::BrakeReleased);
}

void AMainCar::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector actorLocation = GetActorLocation();
	FVector End = actorLocation - FVector(0, 0, 1000);

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	if (GetWorld()->LineTraceSingleByChannel(Hit, actorLocation, End, ECC_Visibility, Params))
	{
		float DesiredHoverHeight = 4.f;
		FVector TargetLocation = GetActorLocation();
		TargetLocation.Z = Hit.ImpactPoint.Z + DesiredHoverHeight;

		//move gameobject smoothly to new location like a lerp
		FVector NewLocation = FMath::VInterpTo(GetActorLocation(), TargetLocation, DeltaTime, 5.f);
		SetActorLocation(NewLocation, false, nullptr, ETeleportType::TeleportPhysics);

	}
	DrawDebugLine(GetWorld(), actorLocation, End, FColor::Green, false, 2.0f, 0, 1.0f);

	//front raycast Info
	FVector LocalForwardOffset(250, 0, 0);
	FVector WorldForwardOffset = GetActorRotation().RotateVector(LocalForwardOffset);
	FVector actorFrontLocation = GetActorLocation() + WorldForwardOffset;
	FVector FrontEnd = actorFrontLocation - FVector(0, 0, 1000);
	FHitResult FrontHit;

	//back raycast Info
	FVector LocalBackwardOffset(-200, 0, 0);
	FVector WorldBackwardOffset = GetActorRotation().RotateVector(LocalBackwardOffset);
	FVector actorBackLocation = GetActorLocation() + WorldBackwardOffset;
	FVector BackEnd = actorBackLocation - FVector(0, 0, 1000);
	FHitResult BackHit;

	//if front and back hit
	if (GetWorld()->LineTraceSingleByChannel(BackHit, actorBackLocation, BackEnd, ECC_Visibility, Params) && GetWorld()->LineTraceSingleByChannel(FrontHit, actorFrontLocation, FrontEnd, ECC_Visibility, Params))
	{
		double deltaX = FrontHit.ImpactPoint.X - BackHit.ImpactPoint.X;
		double deltaY = FrontHit.ImpactPoint.X - BackHit.ImpactPoint.Y;

		double angleRadians = std::atan2(deltaY, deltaX);
		double angleDegrees = angleRadians * (180.0 / PI);

		if (angleDegrees < 0) {
			angleDegrees += 360;
		}
		else
		{
			FRotator TargetRotation = GetActorRotation();
			TargetRotation.Pitch = angleDegrees;

			//move gameobject smoothly to new location like a lerp
			FRotator NewRotation = FMath::RInterpTo(GetActorRotation(), TargetRotation, DeltaTime, 5.f);
			SetActorRotation(NewRotation);

		}

		UE_LOG(LogTemp, Log, TEXT("slope in degrees: %f"), angleDegrees);
		//UE_LOG(LogTemp, Log, TEXT("Front Z coordinate: %f and Back Z coordinate: %f"), FrontHit.ImpactPoint.Z, BackHit.ImpactPoint.Z);
	}
	DrawDebugLine(GetWorld(), actorBackLocation, BackEnd, FColor::Green, false, 2.0f, 0, 1.0f);
	DrawDebugLine(GetWorld(), actorFrontLocation, FrontEnd, FColor::Green, false, 2.0f, 0, 1.0f);
}

void AMainCar::Accelerate(float value)
{
	GetVehicleMovementComponent()->SetThrottleInput(value);
}
void AMainCar::Decelerate(float value)
{
	GetVehicleMovementComponent()->SetBrakeInput(value);
}
void AMainCar::MoveHorizontal(float value)
{
	GetVehicleMovementComponent()->SetSteeringInput(value);
}	
void AMainCar::BrakePressed()
{
	GetVehicleMovementComponent()->SetHandbrakeInput(true);
}
void AMainCar::BrakeReleased()
{
	GetVehicleMovementComponent()->SetHandbrakeInput(false);
}

//look
void AMainCar::LookVertical(float value)
{
	AddControllerPitchInput(value* -1);
}
void AMainCar::LookHorizontal(float value)
{
	AddControllerYawInput(value);
}