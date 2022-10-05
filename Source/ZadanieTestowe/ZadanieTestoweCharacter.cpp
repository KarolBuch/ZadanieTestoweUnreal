// Copyright Epic Games, Inc. All Rights Reserved.

#include "ZadanieTestoweCharacter.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "DrawDebugHelpers.h"
#include "MonitoringCamera.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Projectile.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "ZadanieTestoweGameMode.h"

//////////////////////////////////////////////////////////////////////////
// AZadanieTestoweCharacter

AZadanieTestoweCharacter::AZadanieTestoweCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;
	

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;


	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetMesh());
	
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller
	//
	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	bIsFPCamera = false;


	//Create Projectile Spawn Point
	ProjectileSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("Spawn Point"));
	ProjectileSpawnPoint->SetupAttachment(FollowCamera);

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)
}


void AZadanieTestoweCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bIsFPCamera) return;
	//Mechanika patrzenia na kamerê
	FVector StartLocation = FollowCamera->GetComponentLocation();
	FVector EndLocation = StartLocation + FollowCamera->GetComponentRotation().Vector() * MaxSightRange;

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	FHitResult Hit;

	bool bSuccess = GetWorld()->LineTraceSingleByChannel (Hit, StartLocation, EndLocation, ECollisionChannel::ECC_GameTraceChannel1, Params);

	if (bSuccess)
	{
		AActor* HitActor = Hit.GetActor();
		if (HitActor != nullptr)
		{
			if (AMonitoringCamera* MonitoringCamera = Cast<AMonitoringCamera>(HitActor))
			{
				MonitoringCamera->FocusOnPlayer();
			}
		}
	}
	
}

// Input

void AZadanieTestoweCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AZadanieTestoweCharacter::Fire);
	PlayerInputComponent->BindAction("ChangeCamera", IE_Pressed, this, &AZadanieTestoweCharacter::ChangeCamera);
	PlayerInputComponent->BindAction("BulletOption", IE_Pressed, this, &AZadanieTestoweCharacter::BulletOption);
	PlayerInputComponent->BindAction("BulletOption", IE_Released, this, &AZadanieTestoweCharacter::BulletOptionReleased);


	PlayerInputComponent->BindAxis("MoveForward", this, &AZadanieTestoweCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AZadanieTestoweCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AZadanieTestoweCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AZadanieTestoweCharacter::LookUpAtRate);
	PlayerInputComponent->BindAxis("ZoomCamera", this, &AZadanieTestoweCharacter::ZoomCamera);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &AZadanieTestoweCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &AZadanieTestoweCharacter::TouchStopped);

	// VR headset functionality
	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &AZadanieTestoweCharacter::OnResetVR);
}


void AZadanieTestoweCharacter::OnResetVR()
{
	// If ZadanieTestowe is added to a project via 'Add Feature' in the Unreal Editor the dependency on HeadMountedDisplay in ZadanieTestowe.Build.cs is not automatically propagated
	// and a linker error will result.
	// You will need to either:
	//		Add "HeadMountedDisplay" to [YourProject].Build.cs PublicDependencyModuleNames in order to build successfully (appropriate if supporting VR).
	// or:
	//		Comment or delete the call to ResetOrientationAndPosition below (appropriate if not supporting VR)
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();

}

void AZadanieTestoweCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
		Jump();
}

void AZadanieTestoweCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
		StopJumping();
	
		
}

void AZadanieTestoweCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AZadanieTestoweCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

// przybli¿anie kamery scrollem
void AZadanieTestoweCharacter::ZoomCamera(float Rate)
{
	if (!bIsFPCamera || Rate == 0.f) return;

	float NewTargetArmLen = CameraBoom->TargetArmLength + Rate * ZoomStep;

	CameraBoom->TargetArmLength = FMath::Clamp(NewTargetArmLen, MinZoomLen, MaxZoomLen);

}

void AZadanieTestoweCharacter::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AZadanieTestoweCharacter::MoveRight(float Value)
{
	if ( (Controller != nullptr) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}
// zamiana kamery z FPS na TPS i na odwrót
void AZadanieTestoweCharacter::ChangeCamera()
{
	if (bIsFPCamera == false)
	{
		bUseControllerRotationYaw = true;
		bIsFPCamera = true;
		CameraBoom->TargetArmLength = 0;
		CameraBoom->SocketOffset = FVector::ZeroVector;
		GetMesh()->HideBoneByName(TEXT("head"), EPhysBodyOp::PBO_None);
		GetMesh()->HideBoneByName(TEXT("neck_01"), EPhysBodyOp::PBO_None);
		ProjectileSpawnPoint->SetRelativeLocation(FVector(50.f,0.f,0.f));
	}
	else
	{
		bUseControllerRotationYaw = false;
		bIsFPCamera = false;
		CameraBoom->TargetArmLength = 300;
		CameraBoom->SocketOffset = FVector(0.f, 30.f, 0.f);
		GetMesh()->UnHideBoneByName(TEXT("head"));
		GetMesh()->UnHideBoneByName(TEXT("neck_01"));
		ProjectileSpawnPoint->SetRelativeLocation(FVector(371.f, 0.f, 0.f));
	}
}
// mechanika strzelania
void AZadanieTestoweCharacter::Fire()
{
	FVector Location = ProjectileSpawnPoint->GetComponentLocation();
	FRotator Rotation = ProjectileSpawnPoint->GetComponentRotation();
	
	if (bIsFPCamera)
	{
		Projectile = GetWorld()->SpawnActor<AProjectile>(FPProjectileClass, Location, Rotation);
		PointsAmount = 5;
		if (Projectile)
		{
			Projectile->SetOwner(this);
			if (bIsBulletOption)
			{
				APlayerController* Player = UGameplayStatics::GetPlayerController(this, 0);		
				Player->SetViewTarget(Projectile);
			}
		}
	}
	else
	{
		Projectile = GetWorld()->SpawnActor<AProjectile>(ProjectileClass, Location, Rotation);
		Projectile->SetOwner(this);
		if (bIsBulletOption)
		{
			Projectile->bIsLineTraceButtonPressed = true;
		}
		PointsAmount = 1;

	}
}
// czy pocisk ma zostawiaæ trasê w TPS albo bullet cam w FPS
void AZadanieTestoweCharacter::BulletOption()
{
	bIsBulletOption = true;
}

void AZadanieTestoweCharacter::BulletOptionReleased()
{
	bIsBulletOption = false;
	FlushPersistentDebugLines(GetWorld());
	if (Projectile)
	{
		Projectile->bIsLineTraceButtonPressed = false;
	}
}

float AZadanieTestoweCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	AZadanieTestoweGameMode* GameMode = GetWorld()->GetAuthGameMode<AZadanieTestoweGameMode>();
	if (GameMode != nullptr)
	{
		GameMode->PawnKilled(this);
	}
	DetachFromControllerPendingDestroy();
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	return 0.f;
}

int AZadanieTestoweCharacter::GetScore() const
{
	return PlayerScore;
}