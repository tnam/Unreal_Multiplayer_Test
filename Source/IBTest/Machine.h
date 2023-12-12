// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Data/RecipeData.h"

#include "Machine.generated.h"

class UBoxComponent;
class AShape;
class UNiagaraSystem;

UCLASS()
class IBTEST_API AMachine : public AActor
{
	GENERATED_BODY()

private:

	TMultiMap<FName, AShape*> ShapeIngredients; 

	TArray<FRecipeData*> CachedRecipes; 

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UBoxComponent> CollisionBox;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UNiagaraSystem> SpawnEffect;

public:

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

	void ConsumeRecipe(const FRecipeData* RecipeData);

	bool IsMissingIngredient(const FRecipeData* RecipeData) const;

	void SpawnShapeByName(const FName& ShapeName);

	void PlaySpawnEffect();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	FRecipeData* GetRecipeData(const FName& RecipeName) const;

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult); 

	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
