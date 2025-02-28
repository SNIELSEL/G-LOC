// Fill out your copyright notice in the Description page of Project Settings.


#include "MainCar.h"
#include "GameFrameWork/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

AMainCar::AMainCar()
{
	SpringArmC = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmC->SetupAttachment(RootComponent);
	SpringArmC->TargetArmLength = 600;
	SpringArmC->SetRelativeLocation(FVector(0, 0, 140));
	SpringArmC->SetRelativeRotation(FRotator(-10, 0, 0));
	CameraC = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraC->SetupAttachment(SpringArmC);
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

void AMainCar::Accelerate(float value)
{

}
void AMainCar::Decelerate(float value)
{

}
void AMainCar::MoveHorizontal(float value)
{

}	
void AMainCar::BrakePressed()
{
	UE_LOG(LogTemp, Warning, TEXT("Brake pressed"));
}
void AMainCar::BrakeReleased()
{
	UE_LOG(LogTemp, Warning, TEXT("Brake released"));
}

//look
void AMainCar::LookVertical(float value)
{

}
void AMainCar::LookHorizontal(float value)
{

}