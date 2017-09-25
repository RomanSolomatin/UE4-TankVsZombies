// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Missile.generated.h"

UCLASS()
class TANKSVSZOMBIES_API AMissile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMissile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Speed at which the projectile travels
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
		float Speed;

protected:
	// What to do when the projectile explodes. The base version just destroys the projectile
	UFUNCTION(BlueprintNativeEvent, Category = "Projectile")
		void Explode();
	virtual void Explode_Implementation();
	
};
