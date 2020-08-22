// Copyright Epic Games, Inc. All Rights Reserved.

#include "StealthProtoCharacter.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"

//////////////////////////////////////////////////////////////////////////
// AStealthProtoCharacter

AStealthProtoCharacter::AStealthProtoCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	SneakSlowFactor = 0.3f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	DefaultMovementSpeed = GetCharacterMovement()->MaxWalkSpeed;
	DefaultVerticalSpeed = GetCharacterMovement()->JumpZVelocity;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)

	PrimaryActorTick.bCanEverTick = true;

	DetectedTextRender = CreateDefaultSubobject<UTextRenderComponent>(TEXT("DetectedTextRender"));
	DetectedTextRender->SetText(TEXT("Detected!"));
	DetectedTextRender->SetHorizontalAlignment(EHorizTextAligment::EHTA_Center);
	DetectedTextRender->SetRelativeLocation(FVector(300.0f, 0, GetDefaultHalfHeight()));
	DetectedTextRender->SetRelativeRotation(FRotator(0, 180.0f, 0));
	DetectedTextRender->SetTextRenderColor(FColor(52, 220, 239));
	DetectedTextRender->SetupAttachment(CameraBoom);
}

//////////////////////////////////////////////////////////////////////////
// Input

void AStealthProtoCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAction("Sneak", IE_Pressed, this, &AStealthProtoCharacter::Sneak);
	PlayerInputComponent->BindAction("Sneak", IE_Released, this, &AStealthProtoCharacter::StopSneak);

	PlayerInputComponent->BindAxis("MoveForward", this, &AStealthProtoCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AStealthProtoCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AStealthProtoCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AStealthProtoCharacter::LookUpAtRate);

}

void AStealthProtoCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AStealthProtoCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AStealthProtoCharacter::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AStealthProtoCharacter::MoveRight(float Value)
{
	if ( (Controller != NULL) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

void AStealthProtoCharacter::Sneak()
{
	ToggleSneak(true);
}

void AStealthProtoCharacter::StopSneak()
{
	ToggleSneak(false);
}

void AStealthProtoCharacter::ToggleSneak(bool active)
{
	if (active)
	{
		GetCharacterMovement()->MaxWalkSpeed = DefaultMovementSpeed * SneakSlowFactor;
		GetCharacterMovement()->JumpZVelocity = DefaultVerticalSpeed * SneakSlowFactor;
	}
	else 
	{
		GetCharacterMovement()->MaxWalkSpeed = DefaultMovementSpeed;
		GetCharacterMovement()->JumpZVelocity = DefaultVerticalSpeed;
	}
}

// Called every frame.
void AStealthProtoCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	float soundDistance = GetSoundDistance();
	if (soundDistance > 0.0f)
	{
		DrawDebugSphere(GetWorld(), GetActorLocation(), soundDistance, 200, FColor(52, 220, 239));
	}

	DetectedTextRender->SetHiddenInGame(DetectedAt + DeltaSeconds < UGameplayStatics::GetRealTimeSeconds(GetWorld()));
}

float AStealthProtoCharacter::GetSoundDistance()
{
	if (GetCharacterMovement()->IsMovingOnGround())
	{
		FVector currentVelocity = GetVelocity();
		return currentVelocity.Size();
	}
	return 0.0f;
}

void AStealthProtoCharacter::DetectedFor(float seconds)
{
	// Update the time when player was last detected and how long will it last
	DetectedAt = UGameplayStatics::GetRealTimeSeconds(GetWorld()) + seconds;
}
