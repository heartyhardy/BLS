// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlayerCharacter.generated.h"

UCLASS()
class BLESS_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:

	/** Camera, Lookup and Turn Rates */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* FollowCamera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = Movement, meta = (AllowPrivateAccess = "true"))
		float BaseTurnRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = Movement, meta = (AllowPrivateAccess = "true"))
		float BaseLookupRate;


	/**
		Orienting Character to Combat Mode
			At the end of the lerping, bUseControllerRotationYaw will be turned ON
			and bOrientRotationToMovement will be Turned OFF
	*/
	FQuat4d ActorRotation;
	FQuat4d AimRotation;
	FQuat4d CurrentRotation;

	float AimRotationYaw;
	float TurnLerpSpeed;
	float TurnLerpAlpha;
	bool bLerpingToCombat;


	/** Combat Related */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, category = Combat, meta = (AllowPrivateAccess = "true"))
		bool bIsInCombat;

protected:

	/** Locomotion Related */

	void MoveForward(float Value);
	void MoveRight(float Value);
	
	//Note that Rate is normalized
	void TurnAtRate(float Rate);
	void LookupAtRate(float Rate);

	// Note: Mouse Turn is Disabled while Entering Combat mode
	void MouseXTurn(float Rate);


	/** Combat Related */

	void EnterCombatMode();
	void ExitCombatMode();

	void LerpToAimRotation(float DeltaTime);

public:

	// Camera
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	// Combat
	FORCEINLINE bool IsInCombat() const { return bIsInCombat; }

};
