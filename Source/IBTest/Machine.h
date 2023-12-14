// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Data/RecipeData.h"

#include "Machine.generated.h"

class UBoxComponent;
class AShape;
class UNiagaraSystem;
class USoundBase;

UCLASS()
class IBTEST_API AMachine : public AActor
{
	GENERATED_BODY()

private:

	/** Cached recipes sorted by total number of recipes */
	TArray<FRecipeData*> CachedRecipes; 

	/** All shapes ready to be processed by the machine */
	TMultiMap<FName, AShape*> ShapeIngredients; 

	/** The machine can only process recipes when bEnabled is true */
	UPROPERTY(ReplicatedUsing=OnRep_SetEnabled)
	bool bEnabled;

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Machine")
	TObjectPtr<UStaticMeshComponent> BaseMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Machine")
	TObjectPtr<UStaticMeshComponent> BodyMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Machine")
	TObjectPtr<UBoxComponent> CollisionBox;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Machine")
	TObjectPtr<UNiagaraSystem> SpawnEffect;

public:

	/** Recipe id used by this machine */
	UPROPERTY(EditInstanceOnly, Category="Machine")
	TArray<FName> RecipeIDs;
	
public:	
	// Sets default values for this actor's properties
	AMachine();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void InitializeRecipes();

	void CheckRecipes();

	void ConsumeRecipe(const FRecipeData* RecipeData, bool bConsumeIngredients = true);

	bool IsMissingIngredient(const FRecipeData* RecipeData) const;

	void SpawnShapeByName(const FName& ShapeName);

	UFUNCTION(NetMulticast, Unreliable)
	void PlaySpawnEffect();

	// Replication
	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

public:	

	UFUNCTION()
	void OnRep_SetEnabled();

	FRecipeData* GetRecipeData(const FName& RecipeName) const;

	FRecipeData* GetRandomRecipeData() const;

	FORCEINLINE bool IsMachineEnabled() const { return bEnabled; }

	void SetMachineEnabled(bool bEnabled);

	void CompleteRandomRecipe();

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult); 

	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
