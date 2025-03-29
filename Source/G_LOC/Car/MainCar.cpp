#include "MainCar.h"
#include "GameFrameWork/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "DrawDebugHelpers.h"
#include "InputMappingContext.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"


AMainCar::AMainCar()
{
    CarMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CarMesh"));
    FlameTrailMeshL = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FlameTrailMeshL"));
    FlameTrailMeshR = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FlameTrailMeshR"));
    CameraC = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    LineTraceParent = CreateDefaultSubobject<USceneComponent>(TEXT("LinetraceParent"));
    CenterLineTrace = CreateDefaultSubobject<USceneComponent>(TEXT("CenterStartScene"));
    CenterLineTraceEnd = CreateDefaultSubobject<USceneComponent>(TEXT("CenterEndLineTrace"));

    AutoPossessPlayer = EAutoReceiveInput::Player0;

    RootComponent = CarMesh;

    FlameTrailMeshL->SetupAttachment(CarMesh);
    FlameTrailMeshR->SetupAttachment(CarMesh);

    LineTraceParent->SetupAttachment(CarMesh);

    CenterLineTrace->SetupAttachment(LineTraceParent);
    CenterLineTrace->SetWorldLocation(GetActorLocation() + FVector(0, 0, LinetraceStartHeight));

    CenterLineTraceEnd->SetupAttachment(LineTraceParent);
    CenterLineTraceEnd->SetWorldLocation(GetActorLocation() + FVector(0, 0, LinetraceEndHeight));
}

void AMainCar::BeginPlay()
{
    Super::BeginPlay();

    UInputMappingContext* InputContext_Asset = LoadObject<UInputMappingContext>(nullptr, TEXT("InputMappingContext'/Game/Scripts/Input/Car_Input_Context.Car_Input_Context'"));
    UInputAction* PressBrake_Asset = LoadObject<UInputAction>(nullptr, TEXT("InputAction'/Game/Scripts/Input/BrakePress.BrakePress'"));
    UInputAction* ReleaseBrake_Asset = LoadObject<UInputAction>(nullptr, TEXT("InputAction'/Game/Scripts/Input/BrakeRelease.BrakeRelease'"));
    UInputAction* Boost_Asset = LoadObject<UInputAction>(nullptr, TEXT("InputAction'/Game/Scripts/Input/Boost.Boost'"));
    UInputAction* W_Asset = LoadObject<UInputAction>(nullptr, TEXT("InputAction'/Game/Scripts/Input/W.W'"));
    UInputAction* S_Asset = LoadObject<UInputAction>(nullptr, TEXT("InputAction'/Game/Scripts/Input/S.S'"));
    UInputAction* A_Asset = LoadObject<UInputAction>(nullptr, TEXT("InputAction'/Game/Scripts/Input/A.A'"));
    UInputAction* D_Asset = LoadObject<UInputAction>(nullptr, TEXT("InputAction'/Game/Scripts/Input/D.D'"));


    UTexture2D* emptyBoostBar_Asset = LoadObject<UTexture2D>(nullptr, TEXT("Texture2D'/Game/UI/Images/BoostBarEmpty.BoostBarEmpty'"));
    UTexture2D* BoostBar_Asset = LoadObject<UTexture2D>(nullptr, TEXT("Texture2D'/Game/UI/Images/BoostBar.BoostBar'"));

    TSubclassOf<UIngameUI> UI_Asset = StaticLoadClass(UUserWidget::StaticClass(), nullptr, TEXT("WidgetBlueprint'/Game/Scripts/UI/IngameUI.IngameUI_C'"));

    PlayerHUDClass = UI_Asset;

    InputMapping = InputContext_Asset;
    PressBrake = PressBrake_Asset;
    ReleaseBrake = ReleaseBrake_Asset;
    PressBoost = Boost_Asset;
    MoveForwards = W_Asset;
    MoveBackwards = S_Asset;
    SteerLeftAction = A_Asset;
    SteerRightAction = D_Asset;

    BoostBarFilled = BoostBar_Asset;
    BoostBarEmpty = emptyBoostBar_Asset;

    CarMesh->SetLinearDamping(3.0f);
    CarMesh->SetAngularDamping(5.0f);
    CarMesh->SetCenterOfMass(FVector(0.0f, 0.0f, -50.0f));
    CarMesh->SetAllUseCCD(true);

    CurrentBoost = 100;
    MaxBoost = 100;
    PrimaryActorTick.bCanEverTick = true;

    CarMesh->SetEnableGravity(false);
    CarMesh->SetSimulatePhysics(true);

    CameraC->SetFieldOfView(100);
    CameraC->SetRelativeLocation(FVector(-600, 0, 140));
    CameraC->SetRelativeRotation(FRotator(-6, 0, 0));

    if (IsLocallyControlled() && PlayerHUDClass)
    {
        PlayerHUD = CreateWidget<UIngameUI>(GetWorld(), PlayerHUDClass);
        check(PlayerHUD);
        PlayerHUD->AddToPlayerScreen();
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

    //setting the spedometer slider
    float SpedometerSpeedRatio = FMath::Clamp(CurrentSpeed / (MaxSpeedForClamp * 2), 0.0f, 1.0f);
    PlayerHUD->SetSpedometer(SpedometerSpeedRatio);

    if (SpedometerSpeedRatio > 0.2f)
    {
        CameraFollowDistance = FMath::FInterpTo(CameraFollowDistance, 200.0f, DeltaTime, 2);
    }
    else
    {
        CameraFollowDistance = FMath::FInterpTo(CameraFollowDistance, 300.0f, DeltaTime, 2);
    }

    float CurrentXScaleL = FlameTrailMeshL->GetRelativeScale3D().X;
    float NewXScaleL = FMath::FInterpTo(CurrentXScaleL, DesiredScale, DeltaTime, ScaleInterpSpeed);
    FlameTrailMeshL->SetRelativeScale3D(FVector(NewXScaleL, -1.0f, 1.0f));

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
        float UpDot = FVector::DotProduct(Hit.ImpactNormal, FVector::UpVector);
        float SteepThreshold = 0.7f;
        FVector EffectiveNormal = (UpDot < SteepThreshold) ? FMath::Lerp(Hit.ImpactNormal, FVector::UpVector, 0.5f) : Hit.ImpactNormal;

        FVector DesiredPosition = Hit.ImpactPoint + EffectiveNormal * DesiredHoverHeight;
        FVector NewPosition = FMath::VInterpTo(GetActorLocation(), DesiredPosition, DeltaTime, HoverInterpSpeed);
        SetActorLocation(NewPosition, true);

        FRotator CurrentRotation = GetActorRotation();
        FVector ForwardVector = FVector::VectorPlaneProject(GetActorForwardVector(), EffectiveNormal).GetSafeNormal();
        FRotator DesiredRotation = FRotationMatrix::MakeFromXZ(ForwardVector, EffectiveNormal).Rotator();

        // Incorporate the roll offset based on steering input
        const float MaxRollAngle = 10.0f;
        DesiredRotation.Roll += SteeringInput * MaxRollAngle;

        FRotator NewRotation = FMath::RInterpTo(CurrentRotation, DesiredRotation, DeltaTime, RotationInterpSpeed);
        SetActorRotation(NewRotation);
    }

    //DrawDebugLine(GetWorld(), traceStart, traceEnd, FColor::Blue, false, 0.1f, 0, 1.0f);

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

    if (bIsTimerRunning)
    {
        CurrentLaptimeFloat += DeltaTime;

        PlayerHUD->SetCurrentLaptime(GetFormattedTime());
    }
}

FText AMainCar::GetFormattedTime() const
{
    int32 TotalMilliseconds = FMath::FloorToInt(CurrentLaptimeFloat * 1000.f);

    int32 Minutes = TotalMilliseconds / (60 * 1000);
    int32 Seconds = (TotalMilliseconds % (60 * 1000)) / 1000;
    int32 Milliseconds = TotalMilliseconds % 1000;

    FString Formatted = FString::Printf(TEXT("%d.%02d.%03d"), Minutes, Seconds, Milliseconds);
    return FText::FromString(Formatted);
}


void AMainCar::Boosting(const FInputActionValue& Value)
{
    bIsBoosting = (CurrentBoost > 0);
}

void AMainCar::StopBoosting(const FInputActionValue& Value)
{
    bIsBoosting = false;
}

void AMainCar::UpdateBoost(float DeltaTime)
{
    if (bIsBoosting && CurrentBoost > 0.0f && TargetThrottle > 0 && !bIsRecharging)
    {
        PlayerHUD->SetFillImage(BoostBarFilled);

        MaxFOV = 150;
        MaxFlameScale = 1.6f;

        FVector BoostForce = GetActorForwardVector() * BoostForceCoefficient;
        CarMesh->AddForce(BoostForce, NAME_None, true);

        CurrentBoost -= BoostDrainRate * DeltaTime;
        CurrentBoost = FMath::Clamp(CurrentBoost, 0.0f, MaxBoost);

        if (CurrentBoost <= 0.0f)
        {
            PlayerHUD->SetFillImage(BoostBarEmpty);

            bIsRecharging = true;
            bIsBoosting = false;

            MaxFOV = 140;
            MaxFlameScale = 1;
        }
    }
    else
    {
        MaxFOV = 140;
        MaxFlameScale = 1;

        CurrentBoost += BoostRechargeRate * DeltaTime;
        CurrentBoost = FMath::Clamp(CurrentBoost, 0.0f, MaxBoost);

        if (CurrentBoost >= MaxBoost / 2 && bIsRecharging)
        {
            bIsRecharging = false;
            PlayerHUD->SetFillImage(BoostBarFilled);
        }
    }

    if (PlayerHUD && PlayerHUD->IsInViewport())
    {
        PlayerHUD->SetBoost(CurrentBoost, MaxBoost);
    }
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