// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "Shape.generated.h"

UCLASS()
class IBTEST_API AShape : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AShape();

	FORCEINLINE FName GetShapeID() { return ShapeID; }

public:

	// Unique String ID of this shape
	UPROPERTY(EditAnywhere, Category = "Shape")
	FName ShapeID;

private:

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> MeshComponent;
};
