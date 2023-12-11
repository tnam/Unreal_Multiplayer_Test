// Fill out your copyright notice in the Description page of Project Settings.

#include "Machine.h"
#include "Components/BoxComponent.h"

#include "Shape.h"
#include "Data/ShapeData.h"
#include "GameplaySettings.h"

// Sets default values
AMachine::AMachine()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetSimulatePhysics(true);
	RootComponent = Mesh;

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	CollisionBox->SetBoxExtent(FVector(50.f));
	CollisionBox->SetupAttachment(Mesh);

	CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnBeginOverlap);
	CollisionBox->OnComponentEndOverlap.AddDynamic(this, &ThisClass::OnEndOverlap);
}

// Called when the game starts or when spawned
void AMachine::BeginPlay()
{
	Super::BeginPlay();

	InitializeRecipes();
}

void AMachine::InitializeRecipes()
{
	for (const auto& RecipeID : RecipeIDs)
	{
		if (FRecipeData* RecipeData = GetRecipeData(RecipeID))
		{
			CachedRecipes.Add(RecipeData);
		}
	}

	// Sort recipes by number of required ingredients in descending order
	// In case of different recipes requiring the same ingredients, we will choose the one with most ingredients 
	CachedRecipes.Sort([](const FRecipeData& A, const FRecipeData& B) {
		return A.InShapes.Num() > B.InShapes.Num();
		});
}

void AMachine::CheckRecipes()
{
	for (const auto& RecipeData : CachedRecipes)
	{
		if (!IsMissingIngredient(RecipeData))
		{
			ConsumeRecipe(RecipeData);
		}
	}
}

void AMachine::ConsumeRecipe(const FRecipeData* RecipeData)
{
	// Destroy shape ingredients (only the ones required)
	const TMap<FName, int>& InputShapes = RecipeData->InShapes;
	for (const auto& InputShape : InputShapes)
	{
		const FName ShapeName = InputShape.Key;
		const int32 NumOfShapes = InputShape.Value;
		TArray<AShape*> ShapesToDestroy;
		ShapeIngredients.MultiFind(ShapeName, ShapesToDestroy);

		for (int32 i = 0; i < NumOfShapes; ++i)
		{
			if (ShapesToDestroy.IsValidIndex(i))
			{
				ShapeIngredients.Remove(ShapeName, ShapesToDestroy[i]);
				ShapesToDestroy[i]->Destroy();
			}
		}
	}

	SpawnShapeByName(RecipeData->OutShape);
}

bool AMachine::IsMissingIngredient(const FRecipeData* RecipeData) const
{
	for (const auto& Shape : RecipeData->InShapes)
	{
		const FName RecipeShapeName = Shape.Key;
		const int32 RecipeShapeNum = Shape.Value;

		if (!ShapeIngredients.Contains(RecipeShapeName) ||
			ShapeIngredients.Num(RecipeShapeName) < RecipeShapeNum)
		{
			return true;
		}
	}

	return false;
}

void AMachine::SpawnShapeByName(const FName& ShapeName)
{
	const UGameplaySettings* Settings = GetDefault<UGameplaySettings>();
	const TSoftObjectPtr<UDataTable> Shapes = Settings->ShapeDataTable.LoadSynchronous();
	if (Shapes)
	{
		if (FShapeData* ShapeData = Shapes->FindRow<FShapeData>(ShapeName, ""))
		{
			FActorSpawnParameters SpawnParameters;
			const FTransform ShapeTransform = FTransform(FQuat::Identity, GetActorLocation());
	
			const AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(ShapeData->ShapeClass.Get(), ShapeTransform, SpawnParameters);
		}
	}
}

// Called every frame
void AMachine::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

FRecipeData* AMachine::GetRecipeData(const FName& RecipeName) const
{
	FRecipeData* RecipeData = nullptr;

	const UGameplaySettings* Settings = GetDefault<UGameplaySettings>();
	const TSoftObjectPtr<UDataTable> Recipes = Settings->RecipeDataTable.LoadSynchronous();
	if (Recipes)
	{
		RecipeData = Recipes->FindRow<FRecipeData>(RecipeName, "");
	}

	return RecipeData;
}

void AMachine::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AShape* ShapeActor = Cast<AShape>(OtherActor);
	if (!ShapeActor) return;

	const FName ShapeId = ShapeActor->GetShapeID();

	ShapeIngredients.Add(ShapeId, ShapeActor);

	CheckRecipes();
}

void AMachine::OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AShape* ShapeActor = Cast<AShape>(OtherActor);
	if (!ShapeActor) return;

	const FName ShapeId = ShapeActor->GetShapeID();

	ShapeIngredients.Remove(ShapeId, ShapeActor);
}

