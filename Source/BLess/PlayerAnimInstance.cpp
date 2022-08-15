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
	TIPCharacterYaw(0.f),
	TIPCharacterYawLastFrame(0.f),
	RootYawOffset(0.f),
	// Lean
	CharacterRotation(FRotator::ZeroRotator),
	CharacterRotationLastFrame(FRotator::ZeroRotator),
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
		TIPCharacterYaw = PlayerCharacter->GetActorRotation().Yaw;
		TIPCharacterYawLastFrame = TIPCharacterYaw;

		// Also Reset Curves
		RotationCurve = 0.f;
		RotationCurveLastFrame = 0.f;
	}
	else
	{
		TIPCharacterYawLastFrame = TIPCharacterYaw;
		TIPCharacterYaw = PlayerCharacter->GetActorRotation().Yaw;

		// Delta Between Character Yaw: Current - Last
		const float YawDelta{ TIPCharacterYaw - TIPCharacterYawLastFrame };

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

void UPlayerAnimInstance::Lean(float DeltaTime)
{
	if (!PlayerCharacter) return;

	CharacterRotationLastFrame = CharacterRotation;
	CharacterRotation = PlayerCharacter->GetActorRotation();

	// Gets difference but normalized rotator
	const FRotator Delta{ UKismetMathLibrary::NormalizedDeltaRotator(CharacterRotation, CharacterRotationLastFrame) };

	/** Instead of multiplying DeltaTime as usual we are dividing it
		Reason is CharacterRotation - CharacterRotationLastFrame delta will be larget if we are turning rapidly
		If we mulp. that with DeltaTime it will be smaller so instead we are Dividing it to increase the output
	*/
	// If you use BracedInitialization it will give a conversion error since Delta.Yaw is double
	const float InterpTarget = Delta.Yaw / DeltaTime ;

	const float Interp = FMath::FInterpTo(CharacterYawDelta, InterpTarget, DeltaTime, 6.f);

	// Clap the YawDelta between -90 and +90
	CharacterYawDelta = FMath::Clamp(Interp, -90.f, 90.f);

	// DEBUG ONLY
	TempCharacterYawDelta = CharacterYawDelta;
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
		FRotator DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation);

		FQuat4d AimRotationQ = PlayerCharacter->GetBaseAimRotation().Quaternion();
		FQuat4d MovementRotationQ = PlayerCharacter->GetVelocity().ToOrientationQuat();
		FQuat4d DeltaQ = UKismetMathLibrary::NormalizedDeltaRotator
		(
			MovementRotationQ.Rotator(), AimRotationQ.Rotator()
		).Quaternion();
		
		DeltaQ.Normalize();

		DeltaRotatorQ = FMath::QInterpTo(DeltaRotatorQ, DeltaQ, DeltaTime, 15.f);
		DeltaRotatorQ.Normalize();

		MovementOffsetYaw = DeltaRotatorQ.Rotator().Yaw;

		//MovementOffsetYaw = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation).Yaw;
		//DeltaRotator = FMath::RInterpTo(DeltaRotator, DeltaRot, DeltaTime, 15.f);
		//MovementOffsetYaw = DeltaRotator.Yaw;

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

			FString CharacterYawOffsetMessage = FString::Printf(TEXT("TIP Yaw Delta: %f"), TempYawDiff);
			GEngine->AddOnScreenDebugMessage(3, 2.f, FColor::Cyan, CharacterYawOffsetMessage);

			FString RootYawOffsetMessage = FString::Printf(TEXT("Movement Offset Yaw: %f"), RootYawOffset);
			GEngine->AddOnScreenDebugMessage(4, 2.f, FColor::Magenta, RootYawOffsetMessage);

			FString CharacterYawMessage = FString::Printf(TEXT("Character Yaw: %f"), CharacterRotation.Yaw);
			GEngine->AddOnScreenDebugMessage(5, 2.f, FColor::Turquoise, CharacterYawMessage);

			FString CharacterYawDeltaMessage = FString::Printf(TEXT("Character Yaw Delta: %f"), TempCharacterYawDelta);
			GEngine->AddOnScreenDebugMessage(6, 2.f, FColor::Turquoise, CharacterYawDeltaMessage);

			FString CombatModeMessage = FString::Printf(TEXT("Combat Mode: %s"), bIsInCombat ? TEXT("TRUE") : TEXT("FALSE"));
			GEngine->AddOnScreenDebugMessage(7, 2.f, FColor::Black, CombatModeMessage);

			FString ActorRotationMessage = FString::Printf(TEXT("Actor Rotation: %f"), PlayerCharacter->GetActorRotation().Yaw);
			GEngine->AddOnScreenDebugMessage(8, 2.f, FColor::Silver, ActorRotationMessage);

			FString ControllerAimRotationMessage = FString::Printf(TEXT("Aim Rotation: %f"), PlayerCharacter->GetBaseAimRotation().Yaw);
			GEngine->AddOnScreenDebugMessage(9, 2.f, FColor::Silver, ControllerAimRotationMessage);

			FString ControllerRotationMessage = FString::Printf(TEXT("Control Rotation: %f"), PlayerCharacter->GetControlRotation().Yaw);
			GEngine->AddOnScreenDebugMessage(10, 2.f, FColor::Silver, ControllerRotationMessage);

			FString DeltaTimeMessage = FString::Printf(TEXT("Delta Time: %f"), GetWorld()->GetDeltaSeconds());
			GEngine->AddOnScreenDebugMessage(11, 2.f, FColor::Silver, DeltaTimeMessage);

			FString LastMovementOffsetYawMessage = FString::Printf(TEXT("Last Movement offset Yaw: %f"), LastMovementOffsetYaw);
			GEngine->AddOnScreenDebugMessage(12, 2.f, FColor::Silver, LastMovementOffsetYawMessage);

			FString MovementOffsetYawMessage = FString::Printf(TEXT("Movement offset Yaw: %f"), MovementOffsetYaw);
			GEngine->AddOnScreenDebugMessage(13, 2.f, FColor::Silver, MovementOffsetYawMessage);

		}
		
	}

	// This is called here because PlayerCharacter is being null checked within the function
	TurnInPlace();

	// Call Lean() to update CharacterYawDelta and Interp it
	Lean(DeltaTime);
}
