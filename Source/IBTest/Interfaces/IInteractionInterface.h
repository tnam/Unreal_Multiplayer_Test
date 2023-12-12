// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/Interface.h"
#include "IInteractionInterface.generated.h"

UINTERFACE(BlueprintType)
class IBTEST_API UInteractionInterface : public UInterface
{
	GENERATED_BODY()
};

class IBTEST_API IInteractionInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interaction")
	void Interact1();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interaction")
	void Interact2();
};
