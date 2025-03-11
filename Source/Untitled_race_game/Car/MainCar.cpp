// Fill out your copyright notice in the Description page of Project Settings.


#include "MainCar.h"
#include "GameFrameWork/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "DrawDebugHelpers.h"
#include "InputMappingContext.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"

AMainCar::AMainCar()
{
    ConstructorHelpers::FObjectFinder<UStaticMesh> StaticMesh_Asset(TEXT("StaticMesh'/Game/Art/Car/BallCar.BallCar'"));
    ConstructorHelpers::FObjectFinder<UInputMappingContext> InputContext_Asset(TEXT("InputMappingContext'/Game/Scripts/Input/Car_Input_Context.Car_Input_Context'"));
    ConstructorHelpers::FObjectFinder<UInputAction> PressBrake_Asset(TEXT("InputAction'/Game/Scripts/Input/BrakePress.BrakePress'"));
    ConstructorHelpers::FObjectFinder<UInputAction> ReleaseBrake_Asset(TEXT("InputAction'/Game/Scripts/Input/BrakeRelease.BrakeRelease'"));
    ConstructorHelpers::FObjectFinder<UInputAction> W_Asset(TEXT("InputAction'/Game/Scripts/Input/W.W'"));
    ConstructorHelpers::FObjectFinder<UInputAction> S_Asset(TEXT("InputAction'/Game/Scripts/Input/S.S'"));
    ConstructorHelpers::FObjectFinder<UInputAction> AD_Asset(TEXT("InputAction'/Game/Scripts/Input/AD.AD'"));

	CarMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CarMesh"));
	SpringArmC = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	CameraC = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    LineTraceParent = CreateDefaultSubobject<USceneComponent>(TEXT("LinetraceParent"));
    LeftLineTraceStart = CreateDefaultSubobject<USceneComponent>(TEXT("LeftStartScene"));
    RightLineTraceStart = CreateDefaultSubobject<USceneComponent>(TEXT("RightStartScene"));
    FrontLineTraceStart = CreateDefaultSubobject<USceneComponent>(TEXT("FrontStartScene"));
    BackLineTraceStart = CreateDefaultSubobject<USceneComponent>(TEXT("BackStartScene"));
    CenterLineTraceStart = CreateDefaultSubobject<USceneComponent>(TEXT("CenterStartScene"));
    CarMesh->SetLinearDamping(3.0f);

	RootComponent = CarMesh;
	CarMesh->SetEnableGravity(true);
	CarMesh->SetSimulatePhysics(true);
    CarMesh->SetStaticMesh(StaticMesh_Asset.Object);

	SpringArmC->SetupAttachment(CarMesh);
	SpringArmC->TargetArmLength = 600;
	SpringArmC->SetRelativeLocation(FVector(0, 0, 140));
	SpringArmC->SetRelativeRotation(FRotator(-10, 0, 0));
	SpringArmC->bUsePawnControlRotation = false;

	CameraC->SetupAttachment(SpringArmC);
    CameraC->SetFieldOfView(120);

    InputMapping = InputContext_Asset.Object;
    PressBrake = PressBrake_Asset.Object;
    ReleaseBrake = ReleaseBrake_Asset.Object;
    MoveForwards = W_Asset.Object;
    MoveBackwards = S_Asset.Object;
    HorizontalMovement = AD_Asset.Object;

    LineTraceParent->SetupAttachment(CarMesh);
    LeftLineTraceStart->SetupAttachment(LineTraceParent);
    LeftLineTraceStart->SetWorldLocation(GetActorLocation() + FVector(200, -100, LinetraceStartHeight));
    
    RightLineTraceStart->SetupAttachment(LineTraceParent);
    RightLineTraceStart->SetWorldLocation(GetActorLocation() + FVector(200, 100, LinetraceStartHeight));

    FrontLineTraceStart->SetupAttachment(LineTraceParent);
    FrontLineTraceStart->SetWorldLocation(GetActorLocation() + FVector(200, 0, LinetraceStartHeight));

    BackLineTraceStart->SetupAttachment(LineTraceParent);
    BackLineTraceStart->SetWorldLocation(GetActorLocation() + FVector(-200, 0, LinetraceStartHeight));

    CenterLineTraceStart->SetupAttachment(LineTraceParent);
    CenterLineTraceStart->SetWorldLocation(GetActorLocation() + FVector(0, 0, LinetraceStartHeight));
}

void AMainCar::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer())) 
		{
			subsystem->AddMappingContext(InputMapping, 0);
		}
	}

	if (UEnhancedInputComponent* Input = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) 
	{

        Input->BindAction(MoveForwards, ETriggerEvent::Triggered, this, &AMainCar::Accelerate);
        Input->BindAction(MoveForwards, ETriggerEvent::Completed, this, &AMainCar::StopAccelerate);

        Input->BindAction(MoveBackwards, ETriggerEvent::Triggered, this, &AMainCar::Decelerate);
        Input->BindAction(MoveBackwards, ETriggerEvent::Completed, this, &AMainCar::StopDecelerate);

        Input->BindAction(HorizontalMovement, ETriggerEvent::Triggered, this, &AMainCar::MoveHorizontal);
        Input->BindAction(HorizontalMovement, ETriggerEvent::Completed, this, &AMainCar::StopSteering);
		Input->BindAction(PressBrake, ETriggerEvent::Triggered, this, &AMainCar::BrakePressed);
		Input->BindAction(PressBrake, ETriggerEvent::Completed, this, &AMainCar::BrakeReleased);

		PlayerInputComponent->BindAxis(FName("LookUD"), this, &AMainCar::LookVertical);
		PlayerInputComponent->BindAxis(FName("LookLR"), this, &AMainCar::LookHorizontal);
	}
}

void AMainCar::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    FVector traceStart = CenterLineTraceStart->GetComponentLocation();
    FVector traceEnd = CenterLineTraceStart->GetComponentLocation() - FVector(0, 0, 1000);

    FHitResult Hit;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);

    FCollisionObjectQueryParams ObjectQueryParams;
    ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldStatic);

    if (GetWorld()->LineTraceSingleByObjectType(Hit, traceStart, traceEnd, ObjectQueryParams, Params))
    {
        float currentHeight = CenterLineTraceStart->GetComponentLocation().Z - Hit.ImpactPoint.Z;
        float heightError = DesiredHoverHeight - currentHeight;
        float upwardForce = heightError * HoverForceCoefficient;
        float verticalVelocity = CarMesh->GetComponentVelocity().Z;
        upwardForce -= verticalVelocity * HoverDamping;
        CarMesh->AddForce(FVector(0, 0, upwardForce), NAME_None, true);
    }

    //Movement
    if (ThrottleInput > 0.f)
    {
        FVector forwardForce = GetActorForwardVector() * ThrottleInput * EngineForceCoefficient;
        CarMesh->AddForce(forwardForce, NAME_None, true);
    }
    else if (ThrottleInput < 0.f)
    {
        FVector reverseForce = GetActorForwardVector() * ThrottleInput * EngineForceCoefficient;
        CarMesh->AddForce(reverseForce, NAME_None, true);
    }
    
    //steering
    if (FMath::Abs(SteeringInput) > 0.1f)
    {
        FVector torque = GetActorUpVector() * SteeringInput * SteeringTorqueCoefficient;
        CarMesh->AddTorqueInDegrees(torque, NAME_None, true);
    }

    if (bBraking)
    {
        FVector currentVelocity = CarMesh->GetComponentVelocity();
        FVector brakeForce = -currentVelocity * BrakeForceCoefficient;
        CarMesh->AddForce(brakeForce, NAME_None, true);
    }

    FVector actorFrontLocation = FrontLineTraceStart->GetComponentLocation();
    FVector FrontEnd = FrontLineTraceStart->GetComponentLocation() - FVector(0, 0, 1000);
    FHitResult FrontHit;

    FVector actorBackLocation = BackLineTraceStart->GetComponentLocation();
    FVector BackEnd = BackLineTraceStart->GetComponentLocation() - FVector(0, 0, 1000);
    FHitResult BackHit;

    Params = FCollisionQueryParams();
    Params.AddIgnoredActor(this);

    if (GetWorld()->LineTraceSingleByChannel(BackHit, actorBackLocation, BackEnd, ECC_Visibility, Params) &&
        GetWorld()->LineTraceSingleByChannel(FrontHit, actorFrontLocation, FrontEnd, ECC_Visibility, Params))
    {
        double deltaX = FrontHit.ImpactPoint.X - BackHit.ImpactPoint.X;
        double deltaY = FrontHit.ImpactPoint.Y - BackHit.ImpactPoint.Y;
        double deltaZ = FrontHit.ImpactPoint.Z - BackHit.ImpactPoint.Z;

        double horizontalDistance = std::sqrt(deltaX * deltaX + deltaY * deltaY);
        double angleRadians = std::atan2(deltaZ, horizontalDistance);
        double angleDegrees = angleRadians * (180.0 / PI);

        FRotator TargetRotation = GetActorRotation();
        TargetRotation.Pitch = angleDegrees;

        FRotator NewRotation = FMath::RInterpTo(GetActorRotation(), TargetRotation, DeltaTime, 5.f);
        SetActorRotation(NewRotation, ETeleportType::TeleportPhysics);
    }

    FVector actorLeftLocation = LeftLineTraceStart->GetComponentLocation();
    FVector LeftEnd = LeftLineTraceStart->GetComponentLocation() - FVector(0, 0, 1000);
    FHitResult LeftHit;

    FVector actorRightLocation = RightLineTraceStart->GetComponentLocation();
    FVector RightEnd = RightLineTraceStart->GetComponentLocation() - FVector(0, 0, 1000);
    FHitResult RightHit;

    if (GetWorld()->LineTraceSingleByChannel(LeftHit, actorLeftLocation, LeftEnd, ECC_Visibility, Params) &&
        GetWorld()->LineTraceSingleByChannel(RightHit, actorRightLocation, RightEnd, ECC_Visibility, Params))
    {
        double deltaZ = LeftHit.ImpactPoint.Z - RightHit.ImpactPoint.Z;
        double horizontalDistance = FVector::Dist2D(LeftHit.ImpactPoint, RightHit.ImpactPoint);
        double rollRadians = FMath::Atan2(deltaZ, horizontalDistance);
        double rollDegrees = FMath::RadiansToDegrees(rollRadians);

        FRotator CurrentRotation = GetActorRotation();
        FRotator TargetRotation = CurrentRotation;
        TargetRotation.Roll = rollDegrees;

        FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, 5.f);
        SetActorRotation(NewRotation, ETeleportType::TeleportPhysics);
    }

    DrawDebugLine(GetWorld(), actorLeftLocation, LeftEnd, FColor::Emerald, false, 2.0f, 0, 1.0f);
    DrawDebugLine(GetWorld(), actorRightLocation, RightEnd, FColor::Emerald, false, 2.0f, 0, 1.0f);

    DrawDebugLine(GetWorld(), traceStart, traceEnd, FColor::Blue, false, 0.1f, 0, 1.0f);
    
    DrawDebugLine(GetWorld(), actorBackLocation, BackEnd, FColor::Green, false, 2.0f, 0, 1.0f);
    DrawDebugLine(GetWorld(), actorFrontLocation, FrontEnd, FColor::Green, false, 2.0f, 0, 1.0f);
}


void AMainCar::Accelerate(const FInputActionValue& Value)
{
    float inputValue = Value.Get<float>();
    ThrottleInput = FMath::Clamp(inputValue, 0.0f, 1.0f);
}

void AMainCar::StopAccelerate(const FInputActionValue& Value)
{
    ThrottleInput = 0.0f;
}

void AMainCar::Decelerate(const FInputActionValue& Value)
{
    float inputValue = Value.Get<float>();
    ThrottleInput = FMath::Clamp(inputValue, -1.0f, 0.0f);
    UE_LOG(LogTemp, Warning, TEXT("Input Value: %f"), inputValue);
}

void AMainCar::StopDecelerate(const FInputActionValue& Value)
{
    ThrottleInput = 0.0f;
}

void AMainCar::MoveHorizontal(const FInputActionValue& Value)
{
	float inputValue = Value.Get<float>();
	SteeringInput = FMath::Clamp(inputValue, -1.0f, 1.0f);
}

void AMainCar::StopSteering(const FInputActionValue& Value)
{
    SteeringInput = 0.0f;
}

void AMainCar::BrakePressed()
{
	bBraking = true;
}

void AMainCar::BrakeReleased()
{
	bBraking = false;
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