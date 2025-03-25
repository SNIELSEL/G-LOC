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
    ConstructorHelpers::FObjectFinder<UStaticMesh> VanskaCarMesh_Asset(TEXT("StaticMesh'/Game/Art/Car/Vanska/VanskaShip.VanskaShip'"));
    ConstructorHelpers::FObjectFinder<UStaticMesh> VanskaFlameMesh_Asset(TEXT("StaticMesh'/Game/Art/Particles/VanskaShipFlames__4_.VanskaShipFlames__4_'"));
    ConstructorHelpers::FObjectFinder<UMaterial> VanskaFlameMat_Asset(TEXT("Material'/Game/Art/Particles/VanskaFlame.VanskaFlame'"));
    ConstructorHelpers::FObjectFinder<UInputMappingContext> InputContext_Asset(TEXT("InputMappingContext'/Game/Scripts/Input/Car_Input_Context.Car_Input_Context'"));
    ConstructorHelpers::FClassFinder<UIngameUI> UI_Asset(TEXT("WidgetBlueprint'/Game/Scripts/UI/IngameUI.IngameUI_C'"));
    ConstructorHelpers::FObjectFinder<UInputAction> PressBrake_Asset(TEXT("InputAction'/Game/Scripts/Input/BrakePress.BrakePress'"));
    ConstructorHelpers::FObjectFinder<UInputAction> ReleaseBrake_Asset(TEXT("InputAction'/Game/Scripts/Input/BrakeRelease.BrakeRelease'"));
    ConstructorHelpers::FObjectFinder<UInputAction> Boost_Asset(TEXT("InputAction'/Game/Scripts/Input/Boost.Boost'"));
    ConstructorHelpers::FObjectFinder<UInputAction> W_Asset(TEXT("InputAction'/Game/Scripts/Input/W.W'"));
    ConstructorHelpers::FObjectFinder<UInputAction> S_Asset(TEXT("InputAction'/Game/Scripts/Input/S.S'"));
    ConstructorHelpers::FObjectFinder<UInputAction> A_Asset(TEXT("InputAction'/Game/Scripts/Input/A.A'"));
    ConstructorHelpers::FObjectFinder<UInputAction> D_Asset(TEXT("InputAction'/Game/Scripts/Input/D.D'"));

    CarMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CarMesh"));
    FlameTrailMeshL = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FlameTrailMeshL"));
    FlameTrailMeshR = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FlameTrailMeshR"));
    CameraC = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    LineTraceParent = CreateDefaultSubobject<USceneComponent>(TEXT("LinetraceParent"));
    CenterLineTrace = CreateDefaultSubobject<USceneComponent>(TEXT("CenterStartScene"));
    CenterLineTraceEnd = CreateDefaultSubobject<USceneComponent>(TEXT("CenterEndLineTrace"));

    CarMesh->SetLinearDamping(3.0f);
    CarMesh->SetAngularDamping(5.0f);
    CarMesh->SetCenterOfMass(FVector(0.0f, 0.0f, -50.0f));
    CarMesh->SetAllUseCCD(true);

    RootComponent = CarMesh;
    CarMesh->SetEnableGravity(false);
    CarMesh->SetSimulatePhysics(true);
    CarMesh->SetStaticMesh(VanskaCarMesh_Asset.Object);

    FlameTrailMeshL->SetupAttachment(CarMesh);
    FlameTrailMeshR->SetupAttachment(CarMesh);

    FlameTrailMeshL->SetRelativeLocation(FVector(-5, -27, -1));
    FlameTrailMeshR->SetRelativeLocation(FVector(-5, 0, -1));

    FlameTrailMeshL->SetStaticMesh(VanskaFlameMesh_Asset.Object);
    FlameTrailMeshR->SetStaticMesh(VanskaFlameMesh_Asset.Object);

    if (VanskaCarMesh_Asset.Object.GetName() == "VanskaShip") 
    {
        FlameTrailMeshL->SetMaterial(0, VanskaFlameMat_Asset.Object);
        FlameTrailMeshR->SetMaterial(0,VanskaFlameMat_Asset.Object);
    }

    CameraC->SetFieldOfView(100);
    CameraC->SetRelativeLocation(FVector(-600, 0, 140));
    CameraC->SetRelativeRotation(FRotator(-6, 0, 0));

    AutoPossessPlayer = EAutoReceiveInput::Player0;

    PlayerHUDClass = UI_Asset.Class;

    InputMapping = InputContext_Asset.Object;
    PressBrake = PressBrake_Asset.Object;
    ReleaseBrake = ReleaseBrake_Asset.Object;
    PressBoost = Boost_Asset.Object;
    MoveForwards = W_Asset.Object;
    MoveBackwards = S_Asset.Object;
    SteerLeftAction = A_Asset.Object;
    SteerRightAction = D_Asset.Object;

    LineTraceParent->SetupAttachment(CarMesh);

    CenterLineTrace->SetupAttachment(LineTraceParent);
    CenterLineTrace->SetWorldLocation(GetActorLocation() + FVector(0, 0, LinetraceStartHeight));

    CenterLineTraceEnd->SetupAttachment(LineTraceParent);
    CenterLineTraceEnd->SetWorldLocation(GetActorLocation() + FVector(0, 0, -1000));

    CurrentBoost = 100;
    MaxBoost = 100;
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
        Input->BindAction(PressBoost, ETriggerEvent::Triggered, this, &AMainCar::Boosting);
        Input->BindAction(PressBoost, ETriggerEvent::Completed, this, &AMainCar::StopBoosting);

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

void AMainCar::CameraMovement(float DeltaTime)
{
    FVector CarPosition = GetActorLocation();

    FVector SurfaceTraceStart = CarPosition;
    FVector SurfaceTraceEnd = CarPosition - GetActorUpVector() * 500.0f;

    FHitResult SurfaceHit;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);
    Params.bTraceComplex = true;

    FVector SurfaceNormal = FVector::UpVector;
    if (GetWorld()->LineTraceSingleByChannel(SurfaceHit, SurfaceTraceStart, SurfaceTraceEnd, ECC_Visibility, Params))
    {
        SurfaceNormal = SurfaceHit.ImpactNormal;
    }
    FRotator CarRotationNoRoll = GetActorRotation();
    CarRotationNoRoll.Roll = 0;

    FVector ForwardProjected = FVector::VectorPlaneProject(GetActorForwardVector(), SurfaceNormal).GetSafeNormal();
    FRotator SurfaceAlignedRotation = FRotationMatrix::MakeFromXZ(ForwardProjected, SurfaceNormal).Rotator();

    SurfaceAlignedRotation.Pitch = FMath::ClampAngle(SurfaceAlignedRotation.Pitch + CameraPitchOffset, -80, 80);

    FRotator DesiredCameraRotation = FMath::RInterpTo(CameraC->GetComponentRotation(), SurfaceAlignedRotation, DeltaTime, CameraInterpSpeedRotation);

    CameraC->SetWorldRotation(DesiredCameraRotation);

    FVector CameraOffset = DesiredCameraRotation.RotateVector(FVector(-CameraFollowDistance, 0, CameraHeight));
    FVector DesiredCameraWorldPos = CarPosition + CameraOffset;
    FVector InterpolatedCameraPosition = FMath::VInterpTo(CameraC->GetComponentLocation(), DesiredCameraWorldPos, DeltaTime, CameraInterpSpeedLocation);

    CameraC->SetWorldLocation(InterpolatedCameraPosition);

    FVector CurrentVelocity = CarMesh->GetComponentVelocity();
    float CurrentSpeed = CurrentVelocity.Size();
    float SpeedRatio = FMath::Clamp(CurrentSpeed / MaxSpeedForClamp, 0.0f, 1.0f);
    float DesiredFOV = FMath::Lerp(BaseFOV, MaxFOV, SpeedRatio);
    float CurrentFOV = CameraC->FieldOfView;
    float NewFOV = FMath::FInterpTo(CurrentFOV, DesiredFOV, DeltaTime, FOVInterpSpeed);
    CameraC->SetFieldOfView(NewFOV);
}

void AMainCar::BeginPlay()
{
    Super::BeginPlay();

    if (IsLocallyControlled() && PlayerHUDClass)
    {
        PlayerHUD = CreateWidget<UIngameUI>(GetWorld(), PlayerHUDClass);
        check(PlayerHUD);
        PlayerHUD->AddToPlayerScreen();

        if (PlayerHUD && PlayerHUD->IsInViewport())
        {
            PlayerHUD->SetBoost(CurrentBoost, MaxBoost);
        }
    }
}

void AMainCar::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (PlayerHUD)
    {
        PlayerHUD->RemoveFromParent();
        PlayerHUD = nullptr;
    }

    Super::EndPlay(EndPlayReason);
}

void AMainCar::UpdateThrottle(float DeltaTime)
{
    CurrentThrottle = FMath::FInterpTo(CurrentThrottle, TargetThrottle, DeltaTime, AccelerationSpeed);

    FVector CurrentVelocity = CarMesh->GetComponentVelocity();
    FVector ForwardVector = FVector::VectorPlaneProject(GetActorForwardVector(), GetActorUpVector()).GetSafeNormal();
    if (ForwardVector.IsNearlyZero())
        ForwardVector = GetActorForwardVector();

    float ForwardSpeed = FVector::DotProduct(CurrentVelocity, ForwardVector);

    FVector AppliedForce = FVector::ZeroVector;

    if (FMath::Abs(CurrentThrottle) > KINDA_SMALL_NUMBER)
    {
        if (CurrentThrottle > 0.f)
        {
            AppliedForce = ForwardVector * CurrentThrottle * EngineForceCoefficient;
        }
        else if (CurrentThrottle < 0.f)
        {
            if (ForwardSpeed > 10.f)
            {
                AppliedForce = -ForwardVector * BrakeForceCoefficient * FMath::Abs(CurrentThrottle);
            }
            else
            {
                AppliedForce = ForwardVector * CurrentThrottle * (EngineForceCoefficient * 0.8f);
            }
        }

        if (!AppliedForce.ContainsNaN() && !AppliedForce.IsNearlyZero())
            CarMesh->AddForce(AppliedForce, NAME_None, true);
    }

    CarMesh->SetLinearDamping(CurrentThrottle < 0.f ? 8.0f : 3.0f);

    //set flame mesh scale based on speed
    float CurrentSpeed = CurrentVelocity.Size();
    float SpeedRatio = FMath::Clamp(CurrentSpeed / MaxSpeedForClamp, 0.0f, 1.0f);
    float DesiredScale = FMath::Lerp(MinFlameScale, MaxFlameScale, SpeedRatio);

    float CurrentXScaleL = FlameTrailMeshL->GetRelativeScale3D().X;
    float NewXScaleL = FMath::FInterpTo(CurrentXScaleL, DesiredScale, DeltaTime, ScaleInterpSpeed);
    FlameTrailMeshL->SetRelativeScale3D(FVector(NewXScaleL, 1.0f, 1.0f));

    float CurrentXScaleR = FlameTrailMeshR->GetRelativeScale3D().X;
    float NewXScaleR = FMath::FInterpTo(CurrentXScaleR, DesiredScale, DeltaTime, ScaleInterpSpeed);
    FlameTrailMeshR->SetRelativeScale3D(FVector(NewXScaleR, 1.0f, 1.0f));
}

void AMainCar::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    //checks if there even is a car mesh and if its simulating physics if not then dont execute void to prevent engine crash
    if (!CarMesh || !CarMesh->IsSimulatingPhysics() || !FlameTrailMeshL || !FlameTrailMeshR)
    {
        return;
    }

    UpdateBoost(DeltaTime);
    CameraMovement(DeltaTime);
    UpdateThrottle(DeltaTime);

    FVector traceStart = CenterLineTrace->GetComponentLocation();
    FVector traceEnd = CenterLineTraceEnd->GetComponentLocation();
    FHitResult Hit;

    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);
    Params.bTraceComplex = true;

    FCollisionObjectQueryParams ObjectQueryParams;
    ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldStatic);

    if (GetWorld()->LineTraceSingleByObjectType(Hit, traceStart, traceEnd, ObjectQueryParams, Params))
    {
        FVector DesiredPosition = Hit.ImpactPoint + Hit.ImpactNormal * DesiredHoverHeight;

        FVector NewPosition = FMath::VInterpTo(GetActorLocation(), DesiredPosition, DeltaTime, HoverInterpSpeed);
        SetActorLocation(NewPosition, true);

        FRotator CurrentRotation = GetActorRotation();
        FVector ForwardVector = FVector::VectorPlaneProject(GetActorForwardVector(), Hit.ImpactNormal).GetSafeNormal();
        FRotator DesiredRotation = FRotationMatrix::MakeFromXZ(ForwardVector, Hit.ImpactNormal).Rotator();

        FRotator NewRotation = FMath::RInterpTo(CurrentRotation, DesiredRotation, DeltaTime, 8.0f);
        SetActorRotation(NewRotation);
    }

    DrawDebugLine(GetWorld(), traceStart, traceEnd, FColor::Blue, false, 0.1f, 0, 1.0f);

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
            FVector SteeringTorque = GetActorUpVector() * SteeringInput * SteeringTorqueCoefficient;
            CarMesh->AddTorqueInRadians(SteeringTorque, NAME_None, true);
        }
    }

    if (currentAngularVelocity.Size() > 0.1f && FMath::Abs(SteeringInput) < 0.1f)
    {
        FVector brakingTorque = -currentAngularVelocity.GetSafeNormal() * BrakingTorqueConstant;
        CarMesh->AddTorqueInDegrees(brakingTorque, NAME_None, true);
    }

    if (bBraking)
    {
        FVector CurrentVelocity = CarMesh->GetComponentVelocity();
        float CurrentSpeed = CurrentVelocity.Size();

        if (CurrentSpeed > KINDA_SMALL_NUMBER)
        {
            FVector BrakeDirection = -CurrentVelocity.GetSafeNormal();

            float BrakeStrength = FMath::Clamp(CurrentSpeed / 500.f, 0.f, 1.f);
            FVector BrakeForce = BrakeDirection * BrakeForceCoefficient * BrakeStrength;

            CarMesh->AddForce(BrakeForce, NAME_None, true);
            CarMesh->SetLinearDamping(10.0f);
        }
    }
    else
    {
        CarMesh->SetLinearDamping(CurrentThrottle < 0.f ? 8.0f : 3.0f);
    }
}

void AMainCar::Boosting(const FInputActionValue& Value)
{
    bIsBoosting = (CurrentBoost > 0);
    MaxFOV = 150;
}

void AMainCar::StopBoosting(const FInputActionValue& Value)
{
    bIsBoosting = false;
    MaxFOV = 140;
}

void AMainCar::UpdateBoost(float DeltaTime)
{
    if (bIsBoosting && CurrentBoost > 0 && TargetThrottle > 0)
    {
        FVector BoostForce = GetActorForwardVector() * BoostForceCoefficient;
        CarMesh->AddForce(BoostForce, NAME_None, true);

        CurrentBoost -= BoostDrainRate * DeltaTime;
        CurrentBoost = FMath::Clamp(CurrentBoost, 0.0f, MaxBoost);

        if (CurrentBoost <= 0.0f)
        {
            bIsBoosting = false;
        }
    }
    else
    {
        CurrentBoost += BoostRechargeRate * DeltaTime;
        CurrentBoost = FMath::Clamp(CurrentBoost, 0.0f, MaxBoost);
    }

    if (PlayerHUD && PlayerHUD->IsInViewport())
    {
        PlayerHUD->SetBoost(CurrentBoost, MaxBoost);
    }

    PlayerHUD->SetBoost(CurrentBoost, MaxBoost);
}

void AMainCar::Accelerate(const FInputActionValue& Value)
{
    TargetThrottle = FMath::Clamp(Value.Get<float>(), 0.0f, 1.0f);
}

void AMainCar::StopAccelerate(const FInputActionValue& Value)
{
    TargetThrottle = 0.0f;
}

void AMainCar::Decelerate(const FInputActionValue& Value)
{
    TargetThrottle = FMath::Clamp(Value.Get<float>(), -1.0f, 0.0f);
}

void AMainCar::StopDecelerate(const FInputActionValue& Value)
{
    TargetThrottle = 0.0f;
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