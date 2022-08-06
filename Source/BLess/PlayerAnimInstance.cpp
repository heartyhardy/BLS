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
	// Movement Blendspaces
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


// Turn In Place

void UPlayerAnimInstance::TurnInPlace()
{
	if (!PlayerCharacter) return;
	if (Speed > 0.f)
	{
		// VERY IMPORTANT: Reset values! Otherwise they contain fractions of values which can messup movement!!
		RootYawOffset = 0.f;
		CharacterYaw = PlayerCharacter->GetActorRotation().Yaw;
		CharacterYawLastFrame = CharacterYaw;

		// Also Reset Curves
		RotationCurve = 0.f;
		RotationCurveLastFrame = 0.f;
	}
	else
	{
		CharacterYawLastFrame = CharacterYaw;
		CharacterYaw = PlayerCharacter->GetActorRotation().Yaw;

		// Delta Between Character Yaw: Current - Last
		const float YawDelta{ CharacterYaw - CharacterYawLastFrame };

		// REMOVE LATER: DEBUG PURPOSES ONLY!!!
		TempYawDiff = YawDelta;

		// Desired Rotation offset between Root Bone and Character Rotation
		// Clamped to -180 <-> 180
		RootYawOffset = UKismetMathLibrary::NormalizeAxis(RootYawOffset - YawDelta);

		// Metadata curve returns 1.f if Playing otherwise 0.f
		const float Turning{ GetCurveValue(TEXT("Turning_Meta")) };
		if (Turning > 0.f)
		{
			RotationCurveLastFrame = RotationCurve;
			RotationCurve = GetCurveValue(TEXT("Curve_Rotation"));
			const float DeltaRotation{ RotationCurve - RotationCurveLastFrame };

			// If RootYawOffset > 0 Then we are TURNING LEFT
			// If RootYawOffset < 0 Then we are Turning RIGHT

			// Turning LEFT and RootBone is Turning Towards Right so Subtract the DeltaRotation
			// Turning RIGHT and RootBone is Turning Left so Add the DeltaRotation
			// (Curve returns POSITIVE value(-90.f -> 0.f), DeltaRotation > 0)
			RootYawOffset > 0.f ? RootYawOffset -= DeltaRotation : RootYawOffset += DeltaRotation;

			const float ABSRootYawOffset{ FMath::Abs(RootYawOffset) };
			if (ABSRootYawOffset > 90.f)
			{
				// Ge the excess amount of YawOffset
				const float YawExcess{ ABSRootYawOffset - 90.f };
				RootYawOffset > 0.f ? RootYawOffset -= YawExcess : RootYawOffset += YawExcess;
			}
		}
	}
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

			FString CombatModeMessage = FString::Printf(TEXT("Combat Mode: %s"), bIsInCombat ? TEXT("TRUE") : TEXT("FALSE"));
			GEngine->AddOnScreenDebugMessage(5, 2.f, FColor::Black, CombatModeMessage);

		}
		
	}

	// This is called here because PlayerCharacter is being null checked within the function
	TurnInPlace();
}
