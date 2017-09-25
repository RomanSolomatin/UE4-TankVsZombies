// Fill out your copyright notice in the Description page of Project Settings.

#include "Turret.h"
#include "PaperSpriteComponent.h"
#include "Tank.h"
#include "Missile.h"
#include "TanksStatics.h"


// Sets default values
ATurret::ATurret()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = TurretDirection = CreateDefaultSubobject<UArrowComponent>(TEXT("TurretDirection"));

	TurretSprite = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("TurretSprite"));
	TurretSprite->SetupAttachment(TurretDirection);

	YawSpeed = 180.0f;
}

// Called when the game starts or when spawned
void ATurret::BeginPlay()
{
	Super::BeginPlay();
	
	check(GetParentComponent());
	Tank = Cast<ATank>(GetParentComponent()->GetOwner());
	check(Tank);
}

// Called every frame
void ATurret::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	check(TurretDirection);
	if (Tank)
	{
		if (APlayerController* PC = Cast<APlayerController>(Tank->GetController()))
		{
			FVector2D AimLocation;
			if (PC->GetMousePosition(AimLocation.X, AimLocation.Y))
			{
				UE_LOG(LogTemp, Warning, TEXT("Aim Location: ( %f , %f )"), AimLocation.X, AimLocation.Y);
				FVector2D TurretLocation = FVector2D::ZeroVector;
				UGameplayStatics::ProjectWorldToScreen(PC, TurretDirection->GetComponentLocation(), TurretLocation);
				UE_LOG(LogTemp, Warning, TEXT("Turret Location: ( %f , %f )"), TurretLocation.X, TurretLocation.Y);

				float DesiredYaw;
				if (UTanksStatics::FindLookAtAngle2D(TurretLocation, AimLocation, DesiredYaw))
				{
					DesiredYaw += 90.0f;
					UE_LOG(LogTemp, Warning, TEXT("Desired Yaw: ( %f )"), DesiredYaw);
					FRotator CurrentRotation = TurretDirection->GetComponentRotation();
					float DeltaYaw = UTanksStatics::FindDeltaAngleDegrees(CurrentRotation.Yaw, DesiredYaw);
					UE_LOG(LogTemp, Warning, TEXT("Delta Yaw: ( %f )"), DeltaYaw);
					float MaxDeltaYawThisFrame = YawSpeed * DeltaTime;
					if (MaxDeltaYawThisFrame > FMath::Abs(DeltaYaw))
					{
						// We can get there on this frame, so just set it
						CurrentRotation.Yaw = DesiredYaw;
					}
					else
					{
						CurrentRotation.Yaw += (MaxDeltaYawThisFrame * FMath::Sign(DeltaYaw));
					}
					TurretDirection->SetWorldRotation(CurrentRotation);
				}
			}

			// Handle Input
			const FTankInput& CurrentInput = Tank->GetCurrentInput();
			if (CurrentInput.bPrimaryFire && Projectile)
			{
				if (UWorld* World = GetWorld())
				{
					if (AMissile* NewProjectile = Cast<AMissile>(World->SpawnActor(Projectile)))
					{
						FVector Loc = TurretSprite->GetSocketLocation("Muzzle");
						FRotator Rot = TurretDirection->GetComponentRotation();
						NewProjectile->SetActorLocation(Loc);
						NewProjectile->SetActorRotation(Rot);
					}
				}
			}
		}
	}
}

