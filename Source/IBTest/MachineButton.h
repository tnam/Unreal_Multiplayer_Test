// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/IInteractionInterface.h"

#include "MachineButton.generated.h"

class AMachine;

UCLASS()
class IBTEST_API AMachineButton : public AActor, public IInteractionInterface
{
	GENERATED_BODY()

private:

	UPROPERTY(EditInstanceOnly)
	AMachine* MachineRef;

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> ButtonMesh;
	
public:	
	// Sets default values for this actor's properties
	AMachineButton();

	// IInteractionInterface Begin
	bool CanInteract_Implementation() override;		
	void Interact1_Implementation() override;		
	void Interact2_Implementation() override;
	// IInteractionInterface End

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
