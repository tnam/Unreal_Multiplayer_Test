// Copyright Epic Games, Inc. All Rights Reserved.

#include "IBTestCharacter.h"
#include "IBTestProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Engine/LocalPlayer.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "Interfaces/IInteractionInterface.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// AIBTestCharacter

AIBTestCharacter::AIBTestCharacter()
{
	// Character doesnt have a rifle at start
	bHasRifle = false;
	
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
		
	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-10.f, 0.f, 60.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	//Mesh1P->SetRelativeRotation(FRotator(0.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-30.f, 0.f, -150.f));

	PhysicsHandleComponent = CreateDefaultSubobject<UPhysicsHandleComponent>(TEXT("PhysicsHandle"));
}

void AIBTestCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

}

void AIBTestCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (PhysicsHandleComponent && PhysicsHandleComponent->GetGrabbedComponent())
	{
		FVector StartLocation;
		FVector EndLocation; 
		GetPlayerInteractionRange(StartLocation, EndLocation);
		PhysicsHandleComponent->SetTargetLocation(EndLocation);
	}
}

//////////////////////////////////////////////////////////////////////////// Input

void AIBTestCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AIBTestCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AIBTestCharacter::Look);

		// Interact 1
		EnhancedInputComponent->BindAction(Interact1Action, ETriggerEvent::Triggered, this, &AIBTestCharacter::Interact1);

		// Interact 2 
		EnhancedInputComponent->BindAction(Interact2Action, ETriggerEvent::Triggered, this, &AIBTestCharacter::Interact2);

		// Grabbing
		EnhancedInputComponent->BindAction(GrabAction, ETriggerEvent::Started, this, &AIBTestCharacter::BeginGrab);
		EnhancedInputComponent->BindAction(GrabAction, ETriggerEvent::Completed, this, &AIBTestCharacter::EndGrab);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AIBTestCharacter::Server_Interact1_Implementation(const FHitResult& HitResult)
{
	AActor* HitActor = HitResult.GetActor();

	if (HitActor &&
		HitActor->GetClass()->ImplementsInterface(UInteractionInterface::StaticClass()))
	{
		IInteractionInterface::Execute_Interact1(HitActor);
	}
}

bool AIBTestCharacter::Server_Interact1_Validate(const FHitResult& HitResult)
{
	return true;
}

void AIBTestCharacter::Server_Interact2_Implementation(const FHitResult& HitResult)
{
	AActor* HitActor = HitResult.GetActor();

	if (HitActor &&
		HitActor->GetClass()->ImplementsInterface(UInteractionInterface::StaticClass()))
	{
		IInteractionInterface::Execute_Interact2(HitActor);
	}
}

bool AIBTestCharacter::Server_Interact2_Validate(const FHitResult& HitResult)
{
	return true;
}

void AIBTestCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add movement 
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
		AddMovementInput(GetActorRightVector(), MovementVector.X);
	}
}

void AIBTestCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AIBTestCharacter::Interact1(const FInputActionValue& Value)
{
	FHitResult HitResult = PlayerTrace();    
	AActor* HitActor = HitResult.GetActor();

	if (HitActor &&
		HitActor->GetClass()->ImplementsInterface(UInteractionInterface::StaticClass()))
	{
		const bool bCanInteract = IInteractionInterface::Execute_CanInteract(HitActor);
		if (bCanInteract)
		{
			if (GetLocalRole() != ROLE_Authority)
			{
				Server_Interact1(HitResult);
			}
			else
			{
				Server_Interact1_Implementation(HitResult);
			}
		}
		else
		{
			PlayErrorSFX();
		}
	}
}

void AIBTestCharacter::Interact2(const FInputActionValue& Value)
{
	FHitResult HitResult = PlayerTrace();    
	if (GetLocalRole() != ROLE_Authority)
	{
		Server_Interact2(HitResult);
	}
	else
	{
		Server_Interact2_Implementation(HitResult);
	}
}

void AIBTestCharacter::BeginGrab(const FInputActionValue& Value)
{
	FHitResult HitResult = PlayerTrace();    
	AActor* HitActor = HitResult.GetActor();
	UPrimitiveComponent* HitComponent = HitResult.GetComponent();

	if (HitActor && HitComponent)
	{
		PhysicsHandleComponent->GrabComponentAtLocation
		(
			HitComponent,
			NAME_None,
			HitComponent->GetComponentLocation()
		);
	}
}

void AIBTestCharacter::EndGrab(const FInputActionValue& Value)
{
	if (PhysicsHandleComponent && PhysicsHandleComponent->GetGrabbedComponent())
	{
		PhysicsHandleComponent->ReleaseComponent();
	}
}

void AIBTestCharacter::PlayErrorSFX()
{
	if (ErrorSFX)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), ErrorSFX, GetActorLocation());	
	}
}

FHitResult AIBTestCharacter::PlayerTrace() 
{
	FHitResult HitResult;    
    FCollisionQueryParams QueryParams(FName(TEXT("PlayerTrace")), false, GetOwner());    

	FVector StartLocation;
	FVector EndLocation; 
	GetPlayerInteractionRange(StartLocation, EndLocation);

	//GetWorld()->LineTraceSingleByObjectType
	GetWorld()->LineTraceSingleByChannel
    (
        HitResult,
        StartLocation,
        EndLocation,                      
		ECollisionChannel::ECC_Visibility,
        QueryParams
    );

	return HitResult;
}

void AIBTestCharacter::GetPlayerInteractionRange(FVector& StartLocation, FVector& EndLocation)
{
	if(!FirstPersonCameraComponent) return;

	const float Range = 400.f;
	StartLocation = FirstPersonCameraComponent->GetComponentLocation();
	EndLocation = StartLocation + FirstPersonCameraComponent->GetForwardVector() * Range;
}

void AIBTestCharacter::SetHasRifle(bool bNewHasRifle)
{
	bHasRifle = bNewHasRifle;
}

bool AIBTestCharacter::GetHasRifle()
{
	return bHasRifle;
}