// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "ZadanieTestoweCharacter.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/DamageType.h"

// Sets default values
AProjectile::AProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = ProjectileMesh;

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Movement"));
	
	ProjectileMovement->ProjectileGravityScale = 0,3;

}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	ProjectileMesh->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);

	start = GetActorLocation();
	
}

// Called every frame
void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OterComp, FVector NormalImpulse, const FHitResult& Hit)
{
	
	FPointDamageEvent DamageEvent(10.f, Hit, GetActorRotation().Vector(), nullptr);
	AActor* MyOwner = GetOwner();
	if (!MyOwner) return;
	AController* MyOwnerInstigator = MyOwner->GetInstigatorController();

	if (OtherActor && OtherActor != this && OtherActor != MyOwner)
	{
		OtherActor->TakeDamage(10.f, DamageEvent, MyOwnerInstigator, this);
	}

	if (ProjectileMovement->bShouldBounce)
	{
		numberOfBounce++;
		if (bIsLineTraceButtonPressed)
		{
			DrawDebugLine(GetWorld(), start, Hit.Location, FColor::Green, true);
			start = Hit.Location;
		}
		if (numberOfBounce == 3)
			{
				Destroy();
			}
	}
	else
	{
		Destroy();
	}	
}

