// Fill out your copyright notice in the Description page of Project Settings.


#include "Shape.h"

// Sets default values
AShape::AShape()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh Component"));
	MeshComponent->SetCollisionProfileName(UCollisionProfile::BlockAll_ProfileName);
	RootComponent = MeshComponent;

	SetReplicates(true);
	SetReplicateMovement(true);
}
