// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "GameplaySettings.generated.h"

class UDataTable;

UCLASS(Config=Game, defaultconfig, meta = (DisplayName="Gameplay Settings"))
class IBTEST_API UGameplaySettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, Config, Category = "Machine")
	TSoftObjectPtr<UDataTable> RecipeDataTable;

	UPROPERTY(EditAnywhere, Config, Category = "Machine")
	TSoftObjectPtr<UDataTable> ShapeDataTable;
};
