// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "PlayerAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class BLESS_API UPlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:

	UPlayerAnimInstance();

	UFUNCTION(BlueprintCallable)
	void UpdateAnimationProperties(float DeltaTime);

	virtual void NativeInitializeAnimation() override;
	
private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = Player, meta = (AllowPrivateAccess = "true"))
		class APlayerCharacter* PlayerCharacter;

	/** Movement Related */

	// Character Speed
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = Movement, meta = (AllowPrivateAccess = "true"))
		float Speed;

	// Is Character Airborne
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = Movement, meta = (AllowPrivateAccess = "true"))
		bool bIsInAir;

	// Is Acceleration > 0
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = Movement, meta = (AllowPrivateAccess = "true"))
		bool bIsAccelerating;

	// Difference between AimRotation and MovementRotation: Offset YAW used for Strafing
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = Movement, meta = (AllowPrivateAccess = "true"))
		float MovementOffsetYaw;

	// Difference between AimRotation and MovementRotation: Offset YAW used for Strafing
	// When movement is stopped MovementOffsetYaw is invalid, so we cache it
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = Movement, meta = (AllowPrivateAccess = "true"))
		float LastMovementOffsetYaw;

	/** Turn In Place Related */

	// Yaw Offset between Character Yaw and Root Bone Yaw
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = "Turn In Place", meta = (AllowPrivateAccess = "true"))
		float RootYawOffset;

	// TIPCharacterYaw and TIPCharacterYawLast Frame are only updated when STANDING STILL!
	// Yaw of the character for This Frame
	float TIPCharacterYaw;

	// Yaw of the character in the Previous Frame
	float TIPCharacterYawLastFrame;

	// Rotation Curve in Turn-in-place animations
	float RotationCurve;
	float RotationCurveLastFrame;


	/** Leaning and Global use of Character Yaw and Yaw Delta */
	// Character Yaw this frame
	FRotator CharacterRotation;

	// Character Yaw Last Frame
	FRotator CharacterRotationLastFrame;

	// Used for Leaning in Running Blendspaces
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = "Lean", meta = (AllowPrivateAccess = "true"))
		float CharacterYawDelta;


	/** Combat Related */

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = Combat, meta = (AllowPrivateAccess = "true"))
		bool bIsInCombat;


	/** DEBUG RELATED */
	// REMOVE LATER: DEBUG ONLY
	float TempYawDiff;
	float TempCharacterYawDelta;

protected:

	// Handle Turn-in-place variables
	void TurnInPlace();

	// Handle calculations for Leaning while running
	void Lean(float DeltaTime);

};
