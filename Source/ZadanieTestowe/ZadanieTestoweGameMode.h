// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ZadanieTestoweGameMode.generated.h"

UCLASS(minimalapi)
class AZadanieTestoweGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AZadanieTestoweGameMode();

	virtual void PawnKilled(APawn* PawnKilled);

};



