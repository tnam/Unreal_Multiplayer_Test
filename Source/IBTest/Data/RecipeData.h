// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "RecipeData.generated.h"

 /**  */
USTRUCT(BlueprintType)
struct FRecipeData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:

	FRecipeData() {}

	/** Display name of the recipe */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Name;

	/** List of shapes required for the recipe */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FName, int32> InShapes; 

	/** End result of the recipe */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName OutShape; 
};
