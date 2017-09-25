// Fill out your copyright notice in the Description page of Project Settings.

#include "Tank.h"
#include "PaperSpriteComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/InputComponent.h"
#include "TanksStatics.h"

void FTankInput::Sanitize()
{
	MovementInput = RawMovementInput.ClampAxes(-1.0f, 1.0f);
	MovementInput.GetSafeNormal();
	RawMovementInput.Set(0.0f, 0.0f);
}

void FTankInput::MoveX(float AxisValue)
{
	RawMovementInput.X += AxisValue;
}

void FTankInput::MoveY(float AxisValue)
{
	RawMovementInput.Y += AxisValue;
}

void FTankInput::PrimaryFire(bool bPressed)
{
	bPrimaryFire = bPressed;
}

void FTankInput::SecondaryFire(bool bPressed)
{
	bSecondaryFire = bPressed;
}

// Sets default values
ATank::ATank()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	if (!RootComponent) {
		RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("TankBase"));
	}

	TankDirection = CreateDefaultSubobject<UArrowComponent>(TEXT("TankDirection"));
	TankDirection->SetupAttachment(RootComponent);

	TankSprite = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("TankSprite"));
	TankSprite->SetupAttachment(TankDirection);

	ChildTurret = CreateDefaultSubobject<UChildActorComponent>(TEXT("Turret"));
	ChildTurret->SetupAttachment(TankDirection);

	USpringArmComponent* SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->TargetArmLength = 500.0f;
	SpringArm->CameraLagSpeed = 2.0f;
	SpringArm->bEnableCameraLag = true;
	SpringArm->bEnableCameraRotationLag = false;
	SpringArm->bUsePawnControlRotation = false;
	SpringArm->bDoCollisionTest = false;
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->SetWorldRotation(FRotator(-90.0f, 0.0f, 0.0f));

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->bUsePawnControlRotation = false;
	CameraComponent->ProjectionMode = ECameraProjectionMode::Orthographic;
	CameraComponent->OrthoWidth = 1024.0f;
	CameraComponent->AspectRatio = 4.0f / 3.0f;
	CameraComponent->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	CameraComponent->SetWorldRotation(FRotator(-90.0f, -90.0f, 0.0f));

	MoveSpeed = 100.0f;
	YawSpeed = 180.0f;
}

// Called when the game starts or when spawned
void ATank::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATank::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TankInput.Sanitize();

	// Move the tank
	{
		FVector DesiredMovementDirection = FVector(TankInput.MovementInput.X, TankInput.MovementInput.Y, 0.0f);
		if (!DesiredMovementDirection.IsNearlyZero())
		{
			// Rotate the tank note that we rotate the tankdirection component, not the root component
			FRotator MovementAngle = DesiredMovementDirection.Rotation();
			float DeltaYaw = UTanksStatics::FindDeltaAngleDegrees(TankDirection->GetComponentRotation().Yaw, MovementAngle.Yaw);
			bool bReverse = false;
			if (DeltaYaw != 0.0f)
			{
				float AdjustedDeltaYaw = DeltaYaw;

				if (AdjustedDeltaYaw < -90.0f)
				{
					AdjustedDeltaYaw += 180.0f;
					bReverse = true;
				}
				else if (AdjustedDeltaYaw > 90.0f)
				{
					AdjustedDeltaYaw -= 180.0f;
					bReverse = true;
				}

				// Turn toward the desired angle. Stop if we can get there in one frame
				float MaxYawThisFrame = YawSpeed * DeltaTime;
				if (MaxYawThisFrame >= FMath::Abs(AdjustedDeltaYaw))
				{
					if (bReverse)
					{
						// Move backward
						FRotator FacingAngle = MovementAngle;
						FacingAngle.Yaw = MovementAngle.Yaw + 180.0f;
						TankDirection->SetWorldRotation(FacingAngle);
					}
					else
					{
						TankDirection->SetWorldRotation(MovementAngle);
					}
				}
				else
				{
					// Cant reach our desired angle this fram, rotate part way.
					TankDirection->AddLocalRotation(FRotator(0.0f, FMath::Sign(AdjustedDeltaYaw) * MaxYawThisFrame, 0.0f));
				}
			}
			// Move the tank
			{
				FVector MovementDirection = TankDirection->GetForwardVector() * (bReverse ? -1.0f : 1.0f);
				FVector Pos = GetActorLocation();
				Pos.X += MovementDirection.X * MoveSpeed * DeltaTime;
				Pos.Y += MovementDirection.Y * MoveSpeed * DeltaTime;
				SetActorLocation(Pos);
			}
		}
	}
}

// Called to bind functionality to input
void ATank::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	InputComponent->BindAxis("MoveX", this, &ATank::MoveX);
	InputComponent->BindAxis("MoveY", this, &ATank::MoveY);
	InputComponent->BindAction("PrimaryFire", EInputEvent::IE_Pressed, this, &ATank::PrimaryFirePressed);
	InputComponent->BindAction("PrimaryFire", EInputEvent::IE_Released, this, &ATank::PrimaryFireReleased);
	InputComponent->BindAction("SecondaryFire", EInputEvent::IE_Pressed, this, &ATank::SecondaryFirePressed);
	InputComponent->BindAction("SecondaryFire", EInputEvent::IE_Released, this, &ATank::SecondaryFireReleased);
}

void ATank::MoveX(float AxisValue)
{
	TankInput.MoveX(AxisValue);
}

void ATank::MoveY(float AxisValue)
{
	TankInput.MoveY(AxisValue);
}

void ATank::PrimaryFirePressed()
{
	TankInput.PrimaryFire(true);
}

void ATank::PrimaryFireReleased()
{
	TankInput.PrimaryFire(false);
}

void ATank::SecondaryFirePressed()
{
	TankInput.SecondaryFire(true);
}

void ATank::SecondaryFireReleased()
{
	TankInput.SecondaryFire(false);
}

