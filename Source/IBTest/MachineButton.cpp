// Fill out your copyright notice in the Description page of Project Settings.

#include "MachineButton.h"
#include "Machine.h"

// Sets default values
AMachineButton::AMachineButton()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	ButtonMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ButtonMesh"));
	RootComponent = ButtonMesh;
}

bool AMachineButton::CanInteract_Implementation()
{
	if (MachineRef)
	{
		return MachineRef->IsMachineEnabled();
	}

	return false;
}

void AMachineButton::Interact1_Implementation()
{
	if (MachineRef)
	{
		const bool bMachineEnabled = MachineRef->IsMachineEnabled();
		MachineRef->CompleteRandomRecipe();
	}
}

void AMachineButton::Interact2_Implementation()
{
	if (MachineRef)
	{
		const bool bMachineEnabled = MachineRef->IsMachineEnabled();
		MachineRef->SetMachineEnabled(!bMachineEnabled);

		// Called for listen server
		MachineRef->OnRep_SetEnabled();	
	}
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

