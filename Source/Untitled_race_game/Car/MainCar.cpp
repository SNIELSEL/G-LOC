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
    ConstructorHelpers::FObjectFinder<UStaticMesh> StaticMesh_Asset(TEXT("StaticMesh'/Game/Art/Car/Vanska/V‰nsk‰Ship.V‰nsk‰Ship'"));
    ConstructorHelpers::FObjectFinder<UInputMappingContext> InputContext_Asset(TEXT("InputMappingContext'/Game/Scripts/Input/Car_Input_Context.Car_Input_Context'"));
    ConstructorHelpers::FObjectFinder<UInputAction> PressBrake_Asset(TEXT("InputAction'/Game/Scripts/Input/BrakePress.BrakePress'"));
    ConstructorHelpers::FObjectFinder<UInputAction> ReleaseBrake_Asset(TEXT("InputAction'/Game/Scripts/Input/BrakeRelease.BrakeRelease'"));
    ConstructorHelpers::FObjectFinder<UInputAction> W_Asset(TEXT("InputAction'/Game/Scripts/Input/W.W'"));
    ConstructorHelpers::FObjectFinder<UInputAction> S_Asset(TEXT("InputAction'/Game/Scripts/Input/S.S'"));
    ConstructorHelpers::FObjectFinder<UInputAction> A_Asset(TEXT("InputAction'/Game/Scripts/Input/A.A'"));
    ConstructorHelpers::FObjectFinder<UInputAction> D_Asset(TEXT("InputAction'/Game/Scripts/Input/D.D'"));

    CarMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CarMesh"));
    CarMesh->SetLinearDamping(3.0f);
    RootComponent = CarMesh;
    CarMesh->SetEnableGravity(true);
    CarMesh->SetSimulatePhysics(true);
    CarMesh->SetStaticMesh(StaticMesh_Asset.Object);

    CameraC = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    CameraC->SetupAttachment(CarMesh);
    CameraC->SetFieldOfView(100);
    CameraC->SetRelativeLocation(FVector(-600, 0, 140));
    CameraC->SetRelativeRotation(FRotator(-6, 0, 0));

    LineTraceParent = CreateDefaultSubobject<USceneComponent>(TEXT("LinetraceParent"));
    LineTraceParent->SetupAttachment(CarMesh);

    LeftLineTrace = CreateDefaultSubobject<USceneComponent>(TEXT("LeftStartScene"));
    LeftLineTrace->SetupAttachment(LineTraceParent);

    RightLineTrace = CreateDefaultSubobject<USceneComponent>(TEXT("RightStartScene"));
    RightLineTrace->SetupAttachment(LineTraceParent);

    FrontLineTrace = CreateDefaultSubobject<USceneComponent>(TEXT("FrontStartScene"));
    FrontLineTrace->SetupAttachment(LineTraceParent);

    BackLineTrace = CreateDefaultSubobject<USceneComponent>(TEXT("BackStartScene"));
    BackLineTrace->SetupAttachment(LineTraceParent);

    CenterLineTrace = CreateDefaultSubobject<USceneComponent>(TEXT("CenterStartScene"));
    CenterLineTrace->SetupAttachment(LineTraceParent);

    LeftWallLineTraceStart = CreateDefaultSubobject<USceneComponent>(TEXT("LeftWallLineTraceStart"));
    LeftWallLineTraceStart->SetupAttachment(LineTraceParent);

    LeftWallLineTraceEnd = CreateDefaultSubobject<USceneComponent>(TEXT("LeftWallLineTraceEnd"));
    LeftWallLineTraceEnd->SetupAttachment(LineTraceParent);

    RightWallLineTraceStart = CreateDefaultSubobject<USceneComponent>(TEXT("RightWallLineTraceStart"));
    RightWallLineTraceStart->SetupAttachment(LineTraceParent);

    RightWallLineTraceEnd = CreateDefaultSubobject<USceneComponent>(TEXT("RightWallLineTraceEnd"));
    RightWallLineTraceEnd->SetupAttachment(LineTraceParent);

    InputMapping = InputContext_Asset.Object;
    PressBrake = PressBrake_Asset.Object;
    ReleaseBrake = ReleaseBrake_Asset.Object;
    MoveForwards = W_Asset.Object;
    MoveBackwards = S_Asset.Object;
    SteerLeftAction = A_Asset.Object;
    SteerRightAction = D_Asset.Object;
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

        Input->BindAction(SteerLeftAction, ETriggerEvent::Started, this, &AMainCar::SteerLeftPressed);
        Input->BindAction(SteerLeftAction, ETriggerEvent::Completed, this, &AMainCar::SteerLeftReleased);

        Input->BindAction(SteerRightAction, ETriggerEvent::Started, this, &AMainCar::SteerRightPressed);
        Input->BindAction(SteerRightAction, ETriggerEvent::Completed, this, &AMainCar::SteerRightReleased);

		Input->BindAction(PressBrake, ETriggerEvent::Triggered, this, &AMainCar::BrakePressed);
		Input->BindAction(PressBrake, ETriggerEvent::Completed, this, &AMainCar::BrakeReleased);

		PlayerInputComponent->BindAxis(FName("LookUD"), this, &AMainCar::LookVertical);
		PlayerInputComponent->BindAxis(FName("LookLR"), this, &AMainCar::LookHorizontal);
	}
}

void AMainCar::BeginPlay()
{
    Super::BeginPlay();

    LineTraceParent->AttachToComponent(CarMesh, FAttachmentTransformRules::KeepRelativeTransform);

    LeftLineTrace->SetWorldLocation(GetActorLocation() + FVector(50, -50, LinetraceStartHeight));
    RightLineTrace->SetWorldLocation(GetActorLocation() + FVector(50, 50, LinetraceStartHeight));
    FrontLineTrace->SetWorldLocation(GetActorLocation() + FVector(200, 0, LinetraceStartHeight));
    BackLineTrace->SetWorldLocation(GetActorLocation() + FVector(-100, 0, LinetraceStartHeight));
    CenterLineTrace->SetWorldLocation(GetActorLocation() + FVector(0, 0, LinetraceStartHeight));

    LeftWallLineTraceStart->SetWorldLocation(GetActorLocation() + FVector(0, -100, LinetraceStartHeight));
    LeftWallLineTraceEnd->SetWorldLocation(GetActorLocation() + FVector(0, -LinetraceEndHeight, LinetraceStartHeight));
    RightWallLineTraceStart->SetWorldLocation(GetActorLocation() + FVector(0, 100, LinetraceStartHeight));
    RightWallLineTraceEnd->SetWorldLocation(GetActorLocation() + FVector(0, LinetraceEndHeight, LinetraceStartHeight));

    HoverPreviousForce = FVector::ZeroVector; // initialize hover smoothing force
}

void AMainCar::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    FVector CarPosition = GetActorLocation();
    FRotator CarRotation = GetActorRotation();

    FVector CameraOffset = FVector(-CameraFollowDistance, 0, CameraHeight);
    FVector DesiredCameraWorldPos = CarPosition + CarRotation.RotateVector(CameraOffset);

    FVector CurrentCameraPosition = CameraC->GetComponentLocation();
    FVector InterpolatedCameraPosition = FMath::VInterpTo(CurrentCameraPosition, DesiredCameraWorldPos, DeltaTime, CameraInterpSpeedLocation);
    CameraC->SetWorldLocation(InterpolatedCameraPosition);

    FVector LookAtTarget = CarPosition + FVector(0, 0, CameraHeight / 2.0f);
    FRotator DesiredLookRotation = (LookAtTarget - InterpolatedCameraPosition).Rotation();
    FRotator CurrentCameraRotation = CameraC->GetComponentRotation();
    FRotator InterpolatedCameraRotation = FMath::RInterpTo(CurrentCameraRotation, DesiredLookRotation, DeltaTime, CameraInterpSpeedRotation);
    CameraC->SetWorldRotation(InterpolatedCameraRotation);

    FVector traceStart = CenterLineTrace->GetComponentLocation();
    FVector traceEnd = CenterLineTrace->GetComponentLocation() - FVector(0, 0, 1000);
    FHitResult Hit;

    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);

    FCollisionObjectQueryParams ObjectQueryParams;
    ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldStatic);

    if (GetWorld()->LineTraceSingleByObjectType(Hit, traceStart, traceEnd, ObjectQueryParams, Params))
    {
        float currentHeight = CenterLineTrace->GetComponentLocation().Z - Hit.ImpactPoint.Z;
        float heightError = DesiredHoverHeight - currentHeight;
        float upwardForce = heightError * HoverForceCoefficient;
        float verticalVelocity = CarMesh->GetComponentVelocity().Z;
        upwardForce -= verticalVelocity * HoverDamping;
        CarMesh->AddForce(FVector(0, 0, upwardForce), NAME_None, true);
    }

    FVector leftWallTraceStart = LeftWallLineTraceStart->GetComponentLocation();
    FVector leftWallTraceEnd = LeftWallLineTraceEnd->GetComponentLocation();

    FVector rightWallTraceStart = RightWallLineTraceStart->GetComponentLocation();
    FVector rightWallTraceEnd = RightWallLineTraceEnd->GetComponentLocation();
    FHitResult LeftRightWallHit;

    if (GetWorld()->LineTraceSingleByObjectType(Hit, leftWallTraceStart, leftWallTraceEnd, ObjectQueryParams, Params))
    {

    }

    FVector actorFrontLocation = FrontLineTrace->GetComponentLocation();
    FVector FrontEnd = FrontLineTrace->GetComponentLocation() - FVector(0, 0, 1000);
    FHitResult FrontHit;

    FVector actorBackLocation = BackLineTrace->GetComponentLocation();
    FVector BackEnd = BackLineTrace->GetComponentLocation() - FVector(0, 0, 1000);
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

        FRotator NewRotation = FMath::RInterpTo(GetActorRotation(), TargetRotation, DeltaTime, RotateSpeed);
        SetActorRotation(NewRotation, ETeleportType::TeleportPhysics);
    }

    FVector actorLeftLocation = LeftLineTrace->GetComponentLocation();
    FVector LeftEnd = LeftLineTrace->GetComponentLocation() - FVector(0, 0, 1000);
    FHitResult LeftHit;

    FVector actorRightLocation = RightLineTrace->GetComponentLocation();
    FVector RightEnd = RightLineTrace->GetComponentLocation() - FVector(0, 0, 1000);
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

        FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, RotateSpeed);
        SetActorRotation(NewRotation, ETeleportType::TeleportPhysics);
    }

    //DrawDebugLine(GetWorld(), actorLeftLocation, LeftEnd, FColor::Emerald, false, 2.0f, 0, 1.0f);
    //DrawDebugLine(GetWorld(), actorRightLocation, RightEnd, FColor::Emerald, false, 2.0f, 0, 1.0f);

    DrawDebugLine(GetWorld(), traceStart, traceEnd, FColor::Blue, false, 0.1f, 0, 1.0f);

    //DrawDebugLine(GetWorld(), actorBackLocation, BackEnd, FColor::Green, false, 2.0f, 0, 1.0f);
    //DrawDebugLine(GetWorld(), actorFrontLocation, FrontEnd, FColor::Green, false, 2.0f, 0, 1.0f);

    //DrawDebugLine(GetWorld(), leftWallTraceStart, leftWallTraceEnd, FColor::Red, false, 2.0f, 0, 1.0f);
    //DrawDebugLine(GetWorld(), rightWallTraceStart, rightWallTraceEnd, FColor::Red, false, 2.0f, 0, 1.0f);

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
    
    if (steerLeft && !steerRight)
    {
        SteeringInput = -1.0f;
    }
    else if (steerRight && !steerLeft)
    {
        SteeringInput = 1.0f;
    }
    else
    {
        SteeringInput = 0.0f;
    }

    FVector currentAngularVelocity = CarMesh->GetPhysicsAngularVelocityInDegrees();

    if (FMath::Abs(SteeringInput) > 0.1f)
    {
        if (FMath::Abs(currentAngularVelocity.Z) < MaxSteeringAngularVelocity)
        {
            FVector torque = GetActorUpVector() * SteeringInput * SteeringTorqueCoefficient;
            CarMesh->AddTorqueInDegrees(torque, NAME_None, true);
        }
    }
    else if (currentAngularVelocity.Size() > 0.1f)
    {
        FVector brakingTorque = -currentAngularVelocity.GetSafeNormal() * BrakingTorqueConstant;
        CarMesh->AddTorqueInDegrees(brakingTorque, NAME_None, true);
    }

    float CurrentRoll = GetActorRotation().Roll;
    float TargetRollAngle = FMath::Clamp(CarMesh->GetPhysicsAngularVelocityInDegrees().Z, -MaxSteeringAngularVelocity, MaxSteeringAngularVelocity) / MaxSteeringAngularVelocity * MaxRollAngle;

    float InterpSpeed = FMath::Abs(TargetRollAngle) > FMath::Abs(CurrentRoll) ? RollInterpSpeed : RollInterpSpeed * 0.5f;

    float SmoothRoll = FMath::FInterpTo(CurrentRoll, TargetRollAngle, DeltaTime, InterpSpeed);

    FRotator NewRotation = GetActorRotation();
    NewRotation.Roll = SmoothRoll;

    SetActorRotation(NewRotation, ETeleportType::TeleportPhysics);

    if (bBraking)
    {
        FVector currentVelocity = CarMesh->GetComponentVelocity();
        FVector brakeForce = -currentVelocity * BrakeForceCoefficient;
        CarMesh->AddForce(brakeForce, NAME_None, true);
    }
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
}

void AMainCar::StopDecelerate(const FInputActionValue& Value)
{
    ThrottleInput = 0.0f;
}

void AMainCar::SteerLeftPressed()
{
    steerLeft = true;
}

void AMainCar::SteerLeftReleased()
{
    steerLeft = false;
}

void AMainCar::SteerRightPressed()
{
    steerRight = true;
}

void AMainCar::SteerRightReleased()
{
    steerRight = false;
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