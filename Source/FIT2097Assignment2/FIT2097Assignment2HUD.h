// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once 

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "FIT2097Assignment2HUD.generated.h"

UCLASS()
class AFIT2097Assignment2HUD : public AHUD
{
	GENERATED_BODY()

public:
	AFIT2097Assignment2HUD();

	/** Primary draw call for the HUD */
	virtual void DrawHUD() override;

private:
	/** Crosshair asset pointer */
	class UTexture2D* CrosshairTex;

};

