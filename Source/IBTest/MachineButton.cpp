// Fill out your copyright notice in the Description page of Project Settings.


#include "MachineButton.h"

// Sets default values
AMachineButton::AMachineButton()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	ButtonMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ButtonMesh"));
	RootComponent = ButtonMesh;
}

void AMachineButton::Interact1_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("Interact 1"));
}

void AMachineButton::Interact2_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("Interact 2"));
}

// Called when the game starts or when spawned
void AMachineButton::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMachineButton::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

