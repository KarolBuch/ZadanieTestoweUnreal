// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

class UProjectileMovementComponent;

UCLASS()
class ZADANIETESTOWE_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectile();


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, Category = "Movement")
	UProjectileMovementComponent* ProjectileMovement;
	bool bIsPlayer;

	UPROPERTY(VisibleAnywhere)
	class AZadanieTestoweCharacter* PlayerCharacter;

	FVector start;
	bool bIsLineTraceButtonPressed;
		

private:
	UPROPERTY(EditDefaultsOnly)
	UStaticMeshComponent* ProjectileMesh;
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OterComp, FVector NormalImpulse, const FHitResult& Hit);
	

	int numberOfBounce = 0;

};
