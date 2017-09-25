// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/ArrowComponent.h"
#include "Camera/CameraComponent.h"
#include "Tank.generated.h"

// This struct covers all possible tank input schemes
// What the inputs do vary by tank, but the same inputs will always exits
USTRUCT(BlueprintType)
struct FTankInput
{
	GENERATED_USTRUCT_BODY()

public:
	// Sanitized movement input, ready to be used by game logic
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tank Input")
		FVector2D MovementInput;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tank Input")
		uint32 bPrimaryFire : 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tank Input")
		uint32 bSecondaryFire : 1;

	void Sanitize();
	void MoveX(float AxisValue);
	void MoveY(float AxisValue);
	void PrimaryFire(bool bPressed);
	void SecondaryFire(bool bPressed);

private:
	// Private because its internal, raw data. Game code should never see this.
	FVector2D RawMovementInput;
};

UCLASS()
class TANKSVSZOMBIES_API ATank : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ATank();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable, Category = "Tanks")
		const FTankInput& GetCurrentInput() { return TankInput; }

private:
	void MoveX(float AxisValue);
	void MoveY(float AxisValue);
	void PrimaryFirePressed();
	void PrimaryFireReleased();
	void SecondaryFirePressed();
	void SecondaryFireReleased();

private:
	// Helpful debug tool - which way is the tank facing
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tank", meta = (AllowPrivateAccess = "true"))
		UArrowComponent* TankDirection;
	
	// Sprite for the tank body
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tank", meta = (AllowPrivateAccess = "true"))
		class UPaperSpriteComponent* TankSprite;

	// The actor used as the turret
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tank", meta = (AllowPrivateAccess = "true"))
		UChildActorComponent* ChildTurret;

	// Our in-game camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tank", meta = (AllowPrivateAccess = "true"))
		UCameraComponent* CameraComponent;

protected:
	// Our input structure
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tank Input")
		FTankInput TankInput;

	// Maximum turn rate (degrees/second) of the tank
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tank", meta = (ClampMin = "0.0"))
		float YawSpeed;

	// Maximum movement rate (units/second) of the tank
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tank", meta = (ClampMin = "0.0"))
		float MoveSpeed;
};
