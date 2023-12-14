// Fill out your copyright notice in the Description page of Project Settings.

#include "Machine.h"
#include "Components/BoxComponent.h"

#include "Shape.h"
#include "Data/ShapeData.h"
#include "GameplaySettings.h"
#include "NiagaraFunctionLibrary.h"
#include "Algo/Accumulate.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/DataTableFunctionLibrary.h"
#include "Net/Core/PushModel/PushModel.h"

namespace
{
	int32 GetTotalShapesInRecipe(const FRecipeData& RecipeData)
	{
		TArray<int32> Shapes;
		RecipeData.InShapes.GenerateValueArray(Shapes);
		const int32 TotalShapes = Algo::Accumulate(Shapes, 0);

		return TotalShapes;
	}
}

// Sets default values
AMachine::AMachine()
	: bEnabled(true)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMesh"));
	RootComponent = BaseMesh;

	BodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyMesh"));
	BodyMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BodyMesh->SetupAttachment(BaseMesh);

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	CollisionBox->SetBoxExtent(FVector(50.f));
	CollisionBox->SetupAttachment(BaseMesh);

	CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnBeginOverlap);
	CollisionBox->OnComponentEndOverlap.AddDynamic(this, &ThisClass::OnEndOverlap);

	SetReplicates(true);
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
			// Recipes need at least 2 input shapes to avoid infinite loops
			check(GetTotalShapesInRecipe(*RecipeData) > 1);

			CachedRecipes.Add(RecipeData);
		}
	}

	// Sort recipes by number of required ingredients in descending order
	// In case of different recipes requiring the same ingredients, we will give priority to the one with most ingredients 
	// e.g. Recipe1: {2 cubes}, Recipe2: {2 cubes, 1 sphere} -> choose Recipe2 in this case 
	CachedRecipes.Sort([](const FRecipeData& A, const FRecipeData& B) {
		const int32 TotalAShapes = GetTotalShapesInRecipe(A);
		const int32 TotalBShapes = GetTotalShapesInRecipe(B);

		return TotalAShapes > TotalBShapes;
		});
}

void AMachine::CheckRecipes()
{
	if(!bEnabled) return;

	for (FRecipeData* RecipeData : CachedRecipes)
	{
		if (RecipeData && !IsMissingIngredient(RecipeData))
		{
			ConsumeRecipe(RecipeData);
		}
	}
}

void AMachine::ConsumeRecipe(const FRecipeData* RecipeData, bool bConsumeIngredients /* = true */)
{
	// Destroy shape ingredients (only the ones used by the recipe)
	if (bConsumeIngredients)
	{ 
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
	}

	// Finally spawn the output shape
	SpawnShapeByName(RecipeData->OutShape);

	if (GetLocalRole() == ROLE_Authority && GetNetMode() != NM_DedicatedServer)
	{
		PlaySpawnEffect_Implementation();
	}
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
			SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
			SpawnParameters.Owner = this;
			const FTransform ShapeTransform = FTransform(FQuat::Identity, GetActorLocation());
	
			UClass* ShapeClass = ShapeData->ShapeClass.LoadSynchronous();
			GetWorld()->SpawnActor<AActor>(ShapeClass, ShapeTransform, SpawnParameters);
		}
	}
}

void AMachine::PlaySpawnEffect_Implementation()
{
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), SpawnEffect, GetActorLocation());
}

void AMachine::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Using push model
	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;
	Params.RepNotifyCondition = REPNOTIFY_OnChanged;

	DOREPLIFETIME_WITH_PARAMS_FAST(AMachine, bEnabled, Params);
}

void AMachine::OnRep_SetEnabled()
{
	// Change body material to show if the machine is on or off (green: on, red: off)
	if (BodyMesh)
	{
		BodyMesh->SetCustomPrimitiveDataFloat(0, bEnabled ? 0.f : 1.f);
	}
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

FRecipeData* AMachine::GetRandomRecipeData() const
{
	FRecipeData* RecipeData = nullptr;

	const UGameplaySettings* Settings = GetDefault<UGameplaySettings>();
	const TSoftObjectPtr<UDataTable> Recipes = Settings->RecipeDataTable.LoadSynchronous();
	if (Recipes)
	{
		TArray<FName> RecipeList;
		UDataTableFunctionLibrary::GetDataTableRowNames(Recipes.Get(), RecipeList);

		const int32 RecipeIdx = FMath::RandRange(0, RecipeList.Num() - 1);
		RecipeData = Recipes->FindRow<FRecipeData>(RecipeList[RecipeIdx], "");
	}

	return RecipeData;
}

void AMachine::SetMachineEnabled(bool bMachineEnabled)
{
	if (bEnabled == bMachineEnabled) return;

	bEnabled = bMachineEnabled;
	MARK_PROPERTY_DIRTY_FROM_NAME(AMachine, bEnabled, this);

	if (bEnabled)
	{
		CheckRecipes();
	}
}

void AMachine::CompleteRandomRecipe()
{
	if(!bEnabled) return;

	FRecipeData* RecipeData = GetRandomRecipeData();
	if (RecipeData)
	{
		ConsumeRecipe(RecipeData, false);
		PlaySpawnEffect();
	}
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

