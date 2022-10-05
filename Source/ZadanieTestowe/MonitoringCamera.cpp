// Fill out your copyright notice in the Description page of Project Settings.


#include "MonitoringCamera.h"
#include "ZadanieTestoweCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "ZadanieTestoweGameMode.h"
#include "Projectile.h"
#include "TimerManager.h"

// Sets default values
AMonitoringCamera::AMonitoringCamera()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CameraMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = CameraMesh;

	ProjectileSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("Spawn Point"));
	ProjectileSpawnPoint->SetupAttachment(CameraMesh);


}

// Called when the game starts or when spawned
void AMonitoringCamera::BeginPlay()
{
	Super::BeginPlay();

	PlayerCharacter = Cast<AZadanieTestoweCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));

	GetWorldTimerManager().SetTimer(FireRateTimerHandle, this, &AMonitoringCamera::Fire, FireRate, true);
	
	
}

// Called every frame
void AMonitoringCamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	bIsFocued = false;

}

void AMonitoringCamera::Fire()
{
	if (bIsFocued)
	{
	FVector Location = ProjectileSpawnPoint->GetComponentLocation();
	FRotator Rotation = ProjectileSpawnPoint->GetComponentRotation();
	AProjectile* Projectile = GetWorld()->SpawnActor<AProjectile>(ProjectileClass, Location, Rotation);
	Projectile->SetOwner(this);
	}
}

void AMonitoringCamera::FocusOnPlayer()
{
	bIsFocued = true;
	FVector ToTarget = PlayerCharacter->GetActorLocation() - RootComponent->GetComponentLocation();
	FRotator LookAtRotation = FRotator(ToTarget.Rotation());
	
	SetActorRelativeRotation(LookAtRotation);
}

float AMonitoringCamera::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	PlayerCharacter->PlayerScore += PlayerCharacter->PointsAmount;

	
	return 0.f;
}


