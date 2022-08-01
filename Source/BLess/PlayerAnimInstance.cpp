// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAnimInstance.h"
#include "PlayerCharacter.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"

UPlayerAnimInstance::UPlayerAnimInstance() :
	// Movement
	Speed(0.f),
	bIsInAir(false),
	bIsAccelerating(false),
	bIsRunning(false),
	MovementOffsetYaw(0.f),
	LastMovementOffsetYaw(0.f),
	//Turn In Place
	CharacterYaw(0.f),
	CharacterYawLastFrame(0.f),
	RootYawOffset(0.f),
	// Combat
	bIsInCombat(false)
{

}

void UPlayerAnimInstance::NativeInitializeAnimation()
{
	// Initialize Player Character
	PlayerCharacter = Cast<APlayerCharacter>(TryGetPawnOwner());
}

void UPlayerAnimInstance::TurnInPlace()
{
	if (!PlayerCharacter) return;
	if (Speed > 0.f) return;

	CharacterYawLastFrame = CharacterYaw;
	CharacterYaw = PlayerCharacter->GetActorRotation().Yaw;

	// Delta Between Character Yaw: Current - Last
	const float YawDelta{ CharacterYaw - CharacterYawLastFrame };
	
	// REMOVE LATER: DEBUG PURPOSES ONLY!!!
	TempYawDiff = YawDelta;

	// Desired Rotation offset between Root Bone and Character Rotation
	RootYawOffset -= YawDelta;
}

void UPlayerAnimInstance::UpdateAnimationProperties(float DeltaTime)
{
	// If at any given frame Character is null, try to reinitialize
	if (!PlayerCharacter)
	{
		PlayerCharacter = Cast<APlayerCharacter>(TryGetPawnOwner());
	}

	if (PlayerCharacter)
	{
		/** Movement Related */

		// Get the Lateral Speed
		FVector Velocity{ PlayerCharacter->GetVelocity() };
		Velocity.Z = 0;

		Speed = Velocity.Size();

		// Is Character Airborne
		bIsInAir = PlayerCharacter->GetCharacterMovement()->IsFalling();

		// Is Character Accelerating
		if (PlayerCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0)
		{
			bIsAccelerating = true;
		}
		else
		{
			bIsAccelerating = false;
		}

		// Is Walking Enabled?
		bWalkingEnabled = PlayerCharacter->IsWalkingEnabled();

		// Is Character Running?
		bIsRunning = PlayerCharacter->IsRunning();

		// Get the difference between Aim Rotation and Movement Direction Rotation
		FRotator AimRotation = PlayerCharacter->GetBaseAimRotation();
		FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(PlayerCharacter->GetVelocity());
		MovementOffsetYaw = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation).Yaw;

		// Cache Last MovementOffsetYaw
		if (PlayerCharacter->GetVelocity().Size() > 0.f)
		{
			LastMovementOffsetYaw = MovementOffsetYaw;
		}


		/** Combat Related */

		// Is Player In Combat Mode?
		bIsInCombat = PlayerCharacter->IsInCombat();
		

		/** Debug */
		if (GEngine)
		{
			FString AimRotationMessage = FString::Printf(TEXT("Base Aim Rotation: %f"), AimRotation.Yaw);
			GEngine->AddOnScreenDebugMessage(0, 2.f, FColor::Red, AimRotationMessage);

			FString MovementRotationMessage = FString::Printf(TEXT("Movement Rotation: %f"), MovementRotation.Yaw);
			GEngine->AddOnScreenDebugMessage(1, 2.f, FColor::Blue, MovementRotationMessage);

			FString MovementRotationYawMessage = FString::Printf(TEXT("Movement Offset Yaw: %f"), MovementOffsetYaw);
			GEngine->AddOnScreenDebugMessage(2, 2.f, FColor::Green, MovementRotationYawMessage);

			FString CharacterYawOffsetMessage = FString::Printf(TEXT("Character Yaw Delta: %f"), TempYawDiff);
			GEngine->AddOnScreenDebugMessage(3, 2.f, FColor::Cyan, CharacterYawOffsetMessage);

			FString RootYawOffsetMessage = FString::Printf(TEXT("Movement Offset Yaw: %f"), RootYawOffset);
			GEngine->AddOnScreenDebugMessage(4, 2.f, FColor::Magenta, RootYawOffsetMessage);
		}
		
	}

	// This is called here because PlayerCharacter is being null checked within the function
	TurnInPlace();
}
