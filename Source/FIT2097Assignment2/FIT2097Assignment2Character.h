// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Blueprint/UserWidget.h"
#include "Components/TimelineComponent.h"
#include "Net/UnrealNetwork.h"
#include "FIT2097Assignment2Character.generated.h"


class UInputComponent;

UCLASS(config=Game)
class AFIT2097Assignment2Character : public ACharacter
{
	 GENERATED_BODY()

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	class USkeletalMeshComponent* Mesh1P;

	/** Gun mesh: 1st person view (seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USkeletalMeshComponent* FP_Gun;

	/** Location on gun mesh where projectiles should spawn. */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USceneComponent* FP_MuzzleLocation;

	/** Gun mesh: VR view (attached to the VR controller directly, no arm, just the actual gun) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USkeletalMeshComponent* VR_Gun;

	/** Location on VR gun mesh where projectiles should spawn. */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USceneComponent* VR_MuzzleLocation;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FirstPersonCameraComponent;

	/** Motion controller (right hand) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UMotionControllerComponent* R_MotionController;

	/** Motion controller (left hand) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UMotionControllerComponent* L_MotionController;


public:
	AFIT2097Assignment2Character();

protected:
	virtual void BeginPlay();

	virtual void Tick(float DeltaTime) override;

public:
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	/** Gun muzzle's offset from the characters location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	FVector GunOffset;

	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category=Projectile)
	TSubclassOf<class AFIT2097Assignment2Projectile> ProjectileClass;

	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	class USoundBase* FireSound;

	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class UAnimMontage* FireAnimation;

	/** Whether to use motion controller location for aiming. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	uint32 bUsingMotionControllers : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
		float Health;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
		float CurrentHealth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
		float HealthPrecentage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
		FString Dead;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
		float Stamina;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
		float CurrentStamina;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
		float StaminaPrecentage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Joy ")
		float Joy;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Joy ")
		float CurrentJoy;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Joy ")
		float JoyPrecentage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterMovement ")
		float CurrentSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterMovement ")
		float BaseSpeed;

	UFUNCTION(BlueprintPure, Category = "CharacterMovement ")
		float GetSpeed();
	
	UFUNCTION(BlueprintPure, Category = "Health")
		float GetHealth();

	UFUNCTION(BlueprintPure, Category = "Health")
		FText GetHealthIntText();

	UFUNCTION(BlueprintCallable, Category = "Health")
		void UpdateHealth(float Healthadd);

	UFUNCTION(BlueprintCallable, Category = "Health")
		void CheckIsDead();

	UFUNCTION(BlueprintPure, Category = "Stamina")
		float GetStamina();

	//UFUNCTION(BlueprintPure, Category = "Stamina")
	///	FText GetStaminaIntText();

	UFUNCTION(BlueprintCallable, Category = "Stamina")
		void UpdateStamina(float Staminaadd);

	UFUNCTION(BlueprintPure, Category = "Joy")
		float GetJoy();

	//UFUNCTION(BlueprintPure, Category = "Joy")
	//	FText GetJoyIntText();

	UFUNCTION(BlueprintCallable, Category = "Joy")
		void UpdateJoy(float Joyadd);


	UFUNCTION()
		void GetPosionDamage();
	UFUNCTION()
		void GetJoyDamage();
	UFUNCTION()
		void GetStaminaDamage();
	UFUNCTION()
		void IncreaseSpeed();

	FTimerHandle HealthDamageTimerHandle;
	FTimerHandle JoyDamageTimerHandle;
	FTimerHandle StaminaDamageTimerHandle;
	FTimerHandle SpeedIncreaseTimerHandle;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item ")
	FString ItemName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item ")
	FString PickUpNotice;

	bool CanBeDamage;

	UFUNCTION(BlueprintCallable, Category = "Item")
		FString GetItemName();
	UFUNCTION(BlueprintCallable, Category = "Item")
		FString GetPickUpText();
	
	UPROPERTY(EditAnywhere, Category = "Health", Replicated)
		class AHealthPack* healthpack;

	UPROPERTY(EditAnywhere, Category = "Food", Replicated)
		class AFoodPack* foodpack;

	UPROPERTY(EditAnywhere, Category = "Super", Replicated)
		class ASuperPack* superpack;

	UPROPERTY(EditAnywhere, Category = "Speed", Replicated)
		class ASpeedPickUp* speedpack;


	UFUNCTION(BlueprintPure)
		FString MyRole();

	//UFUNCTION(Server, Reliable, WithValidation)
	void PerformRayTrace();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item ")
	bool isTrace;
	

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>&OutLifetimeProps) const;



protected:
	
	/** Fires a projectile. */
	void OnFire();

	/** Resets HMD orientation and position in VR. */
	void OnResetVR();

	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles stafing movement, left and right */
	void MoveRight(float Val);

	//Pick Up Action
	//UFUNCTION(Server, Reliable, WithValidation)
	void PickUp();

	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	struct TouchData
	{
		TouchData() { bIsPressed = false;Location=FVector::ZeroVector;}
		bool bIsPressed;
		ETouchIndex::Type FingerIndex;
		FVector Location;
		bool bMoved;
	};
	void BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location);
	void EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location);
	void TouchUpdate(const ETouchIndex::Type FingerIndex, const FVector Location);
	TouchData	TouchItem;
	
protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

	/* 
	 * Configures input for touchscreen devices if there is a valid touch interface for doing so 
	 *
	 * @param	InputComponent	The input component pointer to bind controls to
	 * @returns true if touch controls were enabled.
	 */
	bool EnableTouchscreenMovement(UInputComponent* InputComponent);

public:
	/** Returns Mesh1P subobject **/
	FORCEINLINE class USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

private:


};
