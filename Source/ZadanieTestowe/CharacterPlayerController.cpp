// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterPlayerController.h"
#include "Blueprint/UserWidget.h"

void ACharacterPlayerController::BeginPlay()
{
	Super::BeginPlay();

	//Do��czanie celownika i licznika punkt�w do widoku kamery
	HUD = CreateWidget(this, HUDClass);
	if (HUD != nullptr)
	{
		HUD->AddToViewport();
	}
}



void ACharacterPlayerController::GameHasEnded(class AActor* EndGameFocus, bool bIsWinner)
{
	Super::GameHasEnded(EndGameFocus, bIsWinner);
	
	//wy�wietla sie kiedy si� przegra
	UUserWidget* LoseScreen = CreateWidget(this, LoseScreenClass);

	if (LoseScreen != nullptr)
	{
		LoseScreen->AddToViewport();
		HUD->RemoveFromViewport();
		
	}
}