// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

// Sets default values
APlayerCharacter::APlayerCharacter() :
	// Turn Rate
	BaseTurnRate(45.f),
	BaseLookupRate(45.f),
	// Combat
	bIsInCombat(false)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Camera Boom: Pulls in towards Character if collides
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("Camera Boom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.f;
	CameraBoom->bUsePawnControlRotation = true;

	// Follow Camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Follow Camera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false; // We dont want camera to rotate to pawn

	// Disable Controller Rotation for the Character. Camara will still do
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	// Rotate the character to the Movement instead of Controller Rotation
	// Character Moves in the Direction of the Input
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator{ 0.f, 160.f, 0.f };

	// Set Jump Velocity and Air Control
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = .2f;
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();	
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Move Forward in the controller Foward(x) direction
void APlayerCharacter::MoveForward(float Value)
{
	if (Controller && Value != 0.f)
	{
		const FRotator ControlRotation = Controller->GetControlRotation();
		const FRotator YawRotation{ 0.f, ControlRotation.Yaw, 0.f };
		const FVector FowardDirection{ FRotationMatrix{YawRotation}.GetUnitAxis(EAxis::X) };

		AddMovementInput(FowardDirection, Value);
	}
}

// Move Right in the controller Right(y) direction
void APlayerCharacter::MoveRight(float Value)
{
	if (Controller && Value != 0.f)
	{
		const FRotator ControlRotation{ Controller->GetControlRotation() };
		const FRotator YawRotation{ 0.f, ControlRotation.Yaw, 0.f };
		const FVector RightDirection{ FRotationMatrix{YawRotation}.GetUnitAxis(EAxis::Y) };

		AddMovementInput(RightDirection, Value);
	}
}

// Turn Rate
void APlayerCharacter::TurnAtRate(float Rate)
{
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

// LookUp Rate
void APlayerCharacter::LookupAtRate(float Rate)
{
	AddControllerPitchInput(Rate * BaseLookupRate * GetWorld()->GetDeltaSeconds());
}

// TEMP: Enter the COMBAT mode
void APlayerCharacter::EnterCombatMode()
{
	// Needs Lerping with a Curve
	if (!bIsInCombat)
	{
		bIsInCombat = true;
		bUseControllerRotationYaw = true;
	}
}

// TEMP: Exit the COMBAT Mode
void APlayerCharacter::ExitCombatMode()
{

	// EXPERIMENTAL CODE
	// Exit Combat Mode when In IDLE
	FVector Velocity{ GetVelocity() };
	Velocity.Z = 0;

	float Speed = Velocity.Size();

	// Needs Lerping with a Curve
	if (bIsInCombat && Speed == 0.f)
	{
		bIsInCombat = false;
		bUseControllerRotationYaw = false;
	}
}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	check(PlayerInputComponent);

	/** Locomotion */
	PlayerInputComponent->BindAxis("MoveForward", this, &ThisClass::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ThisClass::MoveRight);

	PlayerInputComponent->BindAxis("TurnRate", this, &ThisClass::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ThisClass::LookupAtRate);

	PlayerInputComponent->BindAxis("TurnRate", this, &ThisClass::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ThisClass::LookupAtRate);

	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

	PlayerInputComponent->BindAction("Jump", EInputEvent::IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", EInputEvent::IE_Released, this, &ACharacter::StopJumping);


	/** Combat */
	PlayerInputComponent->BindAction("CombatMode", EInputEvent::IE_Pressed, this, &ThisClass::EnterCombatMode);
	PlayerInputComponent->BindAction("CombatMode", EInputEvent::IE_Released, this, &ThisClass::ExitCombatMode);

}



