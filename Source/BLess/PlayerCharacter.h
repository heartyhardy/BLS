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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* FollowCamera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = Movement, meta = (AllowPrivateAccess = "true"))
		float BaseTurnRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = Movement, meta = (AllowPrivateAccess = "true"))
		float BaseLookupRate;


	/** Walk/Run Acceleration Rates and Speed */

	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = Movement, meta = (AllowPrivateAccess = "true"))
		bool bWalkingEnabled;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = Movement, meta = (AllowPrivateAccess = "true"))
		float DefaultMaxAcceleration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = Movement, meta = (AllowPrivateAccess = "true"))
		float DefaultMaxWalkSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = Movement, meta = (AllowPrivateAccess = "true"))
		float WalkAcceleration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = Movement, meta = (AllowPrivateAccess = "true"))
		float WalkSpeed;


	/** Walk Run Rotation Rates */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = Movement, meta = (AllowPrivateAccess = "true"))
		float WalkRotationRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = Movement, meta = (AllowPrivateAccess = "true"))
		float RunRotationRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, category = Movement, meta = (AllowPrivateAccess = "true"))
		bool bIsRunning;


	/** Combat Related */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, category = Combat, meta = (AllowPrivateAccess = "true"))
		bool bIsInCombat;

protected:

	/** Locomotion Related */

	void MoveForward(float Value);
	void MoveRight(float Value);

	void Run();
	void StopRunning();
	
	//Note that Rate is normalized
	void TurnAtRate(float Rate);
	void LookupAtRate(float Rate);

	/** Combat Related */

	void EnterCombatMode();
	void ExitCombatMode();

public:

	// Camera
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	// Movement
	FORCEINLINE bool IsWalkingEnabled() const { return bWalkingEnabled; }
	FORCEINLINE bool IsRunning() const { return bIsRunning; }

	// Combat
	FORCEINLINE bool IsInCombat() const { return bIsInCombat; }

};
