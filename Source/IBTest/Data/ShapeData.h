// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameplayTagsManager.h"
#include "ShapeData.generated.h"

 /**  */
USTRUCT(BlueprintType)
struct FShapeData : public FGameplayTagTableRow
{
	GENERATED_USTRUCT_BODY()

public:

	FShapeData() {}

	/** Description of the shape */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Description;

	/** Class of the shape actor */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftClassPtr<class AShape> ShapeClass;
};
