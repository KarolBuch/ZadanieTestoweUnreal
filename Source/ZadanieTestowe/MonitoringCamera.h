// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MonitoringCamera.generated.h"

class AZadanieTestoweCharacter;

UCLASS()
class ZADANIETESTOWE_API AMonitoringCamera : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMonitoringCamera();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void FocusOnPlayer();
private:
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* CameraMesh;

	UPROPERTY(VisibleAnywhere)
	class USceneComponent* ProjectileSpawnPoint;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class AProjectile> ProjectileClass;

	UPROPERTY(VisibleAnywhere)
	class AZadanieTestoweCharacter* PlayerCharacter;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	
	void Fire();

	FTimerHandle FireRateTimerHandle;

	float FireRate = 1.5f;

	bool bIsFocued;

};
