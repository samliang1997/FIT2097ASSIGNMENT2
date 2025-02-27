// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "FIT2097Assignment2Character.h"
#include "FIT2097Assignment2Projectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Engine.h"
#include "MotionControllerComponent.h"
#include "XRMotionControllerBase.h" // for FXRMotionControllerBase::RightHandSourceId
#include "TimerManager.h"
#include "DrawDebugHelpers.h"
#include "HealthPack.h"
#include "FoodPack.h"
#include "SuperPack.h"
#include "SpeedPickUp.h"
#include "Net/UnrealNetwork.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// AFIT2097Assignment2Character


void AFIT2097Assignment2Character::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>&OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AFIT2097Assignment2Character, healthpack);
	DOREPLIFETIME(AFIT2097Assignment2Character, foodpack);
	DOREPLIFETIME(AFIT2097Assignment2Character, superpack);
	DOREPLIFETIME(AFIT2097Assignment2Character, speedpack);

}


AFIT2097Assignment2Character::AFIT2097Assignment2Character()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->RelativeLocation = FVector(-39.56f, 1.75f, 64.f); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->RelativeRotation = FRotator(1.9f, -19.19f, 5.2f);
	Mesh1P->RelativeLocation = FVector(-0.5f, -4.4f, -155.7f);

	// Create a gun mesh component
	FP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	FP_Gun->SetOnlyOwnerSee(true);			// only the owning player will see this mesh
	FP_Gun->bCastDynamicShadow = false;
	FP_Gun->CastShadow = false;
	// FP_Gun->SetupAttachment(Mesh1P, TEXT("GripPoint"));
	FP_Gun->SetupAttachment(RootComponent);

	FP_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	FP_MuzzleLocation->SetupAttachment(FP_Gun);
	FP_MuzzleLocation->SetRelativeLocation(FVector(0.2f, 48.4f, -10.6f));

	// Default offset from the character location for projectiles to spawn
	GunOffset = FVector(100.0f, 0.0f, 10.0f);

	// Note: The ProjectileClass and the skeletal mesh/anim blueprints for Mesh1P, FP_Gun, and VR_Gun 
	// are set in the derived blueprint asset named MyCharacter to avoid direct content references in C++.

	// Create VR Controllers.
	R_MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("R_MotionController"));
	R_MotionController->MotionSource = FXRMotionControllerBase::RightHandSourceId;
	R_MotionController->SetupAttachment(RootComponent);
	L_MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("L_MotionController"));
	L_MotionController->SetupAttachment(RootComponent);

	// Create a gun and attach it to the right-hand VR controller.
	// Create a gun mesh component
	VR_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("VR_Gun"));
	VR_Gun->SetOnlyOwnerSee(true);			// only the owning player will see this mesh
	VR_Gun->bCastDynamicShadow = false;
	VR_Gun->CastShadow = false;
	VR_Gun->SetupAttachment(R_MotionController);
	VR_Gun->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));

	VR_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("VR_MuzzleLocation"));
	VR_MuzzleLocation->SetupAttachment(VR_Gun);
	VR_MuzzleLocation->SetRelativeLocation(FVector(0.000004, 53.999992, 10.000000));
	VR_MuzzleLocation->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));		// Counteract the rotation of the VR gun model.

	// Uncomment the following line to turn motion controllers on by default:
	//bUsingMotionControllers = true;
}

void AFIT2097Assignment2Character::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	Health = 50.0f;
	CurrentHealth = Health;
	HealthPrecentage = 1.0f;
	bCanBeDamaged = true;

	Stamina = 50.0f;
	CurrentStamina = Stamina;
	StaminaPrecentage = 1.0f;

	Joy = 50.0f;
	CurrentJoy = Joy;
	JoyPrecentage = 1.0f;

	BaseSpeed = 800.0f;
	CurrentSpeed = BaseSpeed;
	GetCharacterMovement()->MaxWalkSpeed = BaseSpeed;

	CanBeDamage = false;
	ItemName = NULL;
	PickUpNotice = NULL;
	Dead = NULL;
	isTrace = false;

	//Attach gun mesh component to Skeleton, doing it here because the skeleton is not yet created in the constructor
	FP_Gun->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));

	// Show or hide the two versions of the gun based on whether or not we're using motion controllers.
	if (bUsingMotionControllers)
	{
		VR_Gun->SetHiddenInGame(false, true);
		Mesh1P->SetHiddenInGame(true, true);
	}
	else
	{
		VR_Gun->SetHiddenInGame(true, true);
		Mesh1P->SetHiddenInGame(false, true);
	}

	GetWorldTimerManager().SetTimer(HealthDamageTimerHandle, this, &AFIT2097Assignment2Character::GetPosionDamage, 5.0f, true);
	GetWorldTimerManager().SetTimer(JoyDamageTimerHandle, this, &AFIT2097Assignment2Character::GetJoyDamage, 8.0f, true);
}


//////////////////////////////////////////////////////////////////////////
// Input

void AFIT2097Assignment2Character::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// Bind fire event
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AFIT2097Assignment2Character::OnFire);
	
	//Pick up event
	PlayerInputComponent->BindAction("PickUp", IE_Pressed, this, &AFIT2097Assignment2Character::PickUp);

	// Enable touchscreen input
	EnableTouchscreenMovement(PlayerInputComponent);

	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &AFIT2097Assignment2Character::OnResetVR);

	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &AFIT2097Assignment2Character::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AFIT2097Assignment2Character::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AFIT2097Assignment2Character::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AFIT2097Assignment2Character::LookUpAtRate);
}

void AFIT2097Assignment2Character::OnFire()
{
	//PerformRayTrace();

	// try and fire a projectile
	/*
	if (ProjectileClass != NULL)
	{
		UWorld* const World = GetWorld();
		if (World != NULL)
		{
			if (bUsingMotionControllers)
			{
				const FRotator SpawnRotation = VR_MuzzleLocation->GetComponentRotation();
				const FVector SpawnLocation = VR_MuzzleLocation->GetComponentLocation();
				World->SpawnActor<AFIT2097Assignment2Projectile>(ProjectileClass, SpawnLocation, SpawnRotation);
			}
			else
			{
				const FRotator SpawnRotation = GetControlRotation();
				// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
				const FVector SpawnLocation = ((FP_MuzzleLocation != nullptr) ? FP_MuzzleLocation->GetComponentLocation() : GetActorLocation()) + SpawnRotation.RotateVector(GunOffset);

				//Set Spawn Collision Handling Override
				FActorSpawnParameters ActorSpawnParams;
				ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

				// spawn the projectile at the muzzle
				World->SpawnActor<AFIT2097Assignment2Projectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
			}
		}
	}
	*/


	// try and play the sound if specified
	if (FireSound != NULL)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	}

	// try and play a firing animation if specified
	if (FireAnimation != NULL)
	{
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();
		if (AnimInstance != NULL)
		{
			AnimInstance->Montage_Play(FireAnimation, 1.f);
		}
	}

}

void AFIT2097Assignment2Character::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void AFIT2097Assignment2Character::BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == true)
	{
		return;
	}

	if ((FingerIndex == TouchItem.FingerIndex) && (TouchItem.bMoved == false))
	{
		OnFire();
	}
	TouchItem.bIsPressed = true;
	TouchItem.FingerIndex = FingerIndex;
	TouchItem.Location = Location;
	TouchItem.bMoved = false;
}

void AFIT2097Assignment2Character::EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == false)
	{
		return;
	}
	TouchItem.bIsPressed = false;
}

//Commenting this section out to be consistent with FPS BP template.
//This allows the user to turn without using the right virtual joystick

//void AFIT2097Assignment2Character::TouchUpdate(const ETouchIndex::Type FingerIndex, const FVector Location)
//{
//	if ((TouchItem.bIsPressed == true) && (TouchItem.FingerIndex == FingerIndex))
//	{
//		if (TouchItem.bIsPressed)
//		{
//			if (GetWorld() != nullptr)
//			{
//				UGameViewportClient* ViewportClient = GetWorld()->GetGameViewport();
//				if (ViewportClient != nullptr)
//				{
//					FVector MoveDelta = Location - TouchItem.Location;
//					FVector2D ScreenSize;
//					ViewportClient->GetViewportSize(ScreenSize);
//					FVector2D ScaledDelta = FVector2D(MoveDelta.X, MoveDelta.Y) / ScreenSize;
//					if (FMath::Abs(ScaledDelta.X) >= 4.0 / ScreenSize.X)
//					{
//						TouchItem.bMoved = true;
//						float Value = ScaledDelta.X * BaseTurnRate;
//						AddControllerYawInput(Value);
//					}
//					if (FMath::Abs(ScaledDelta.Y) >= 4.0 / ScreenSize.Y)
//					{
//						TouchItem.bMoved = true;
//						float Value = ScaledDelta.Y * BaseTurnRate;
//						AddControllerPitchInput(Value);
//					}
//					TouchItem.Location = Location;
//				}
//				TouchItem.Location = Location;
//			}
//		}
//	}
//}

void AFIT2097Assignment2Character::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
		CanBeDamage = true;
		GetStaminaDamage();
		
	}
}

void AFIT2097Assignment2Character::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
		CanBeDamage = true;
		GetStaminaDamage();
		
	}
}

void AFIT2097Assignment2Character::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AFIT2097Assignment2Character::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

bool AFIT2097Assignment2Character::EnableTouchscreenMovement(class UInputComponent* PlayerInputComponent)
{
	if (FPlatformMisc::SupportsTouchInput() || GetDefault<UInputSettings>()->bUseMouseForTouch)
	{
		PlayerInputComponent->BindTouch(EInputEvent::IE_Pressed, this, &AFIT2097Assignment2Character::BeginTouch);
		PlayerInputComponent->BindTouch(EInputEvent::IE_Released, this, &AFIT2097Assignment2Character::EndTouch);

		//Commenting this out to be more consistent with FPS BP template.
		//PlayerInputComponent->BindTouch(EInputEvent::IE_Repeat, this, &AFIT2097Assignment2Character::TouchUpdate);
		return true;

	}
	
	return false;
}

void AFIT2097Assignment2Character::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FHitResult OutHit;

	FVector Start = FP_Gun->GetComponentLocation();
	FVector ForwardVector = FirstPersonCameraComponent->GetForwardVector();
	FVector End = (Start + (ForwardVector* 1000.0f));

	FCollisionQueryParams CollisionParams;

	DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 1, 0, 1);

	bool isHit = GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECC_Visibility, CollisionParams);

	if (isHit)
	{
		if (OutHit.bBlockingHit)
		{

			//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, FString::Printf(TEXT("You are hitting: %s"), *OutHit.GetActor()->GetName()));	
			if (OutHit.GetActor()->GetClass()->IsChildOf(AHealthPack::StaticClass()))
			{
				PickUpNotice = "Press E to pick up";
				ItemName = "HealthPack";
				isTrace = true;
			}
			else if (OutHit.GetActor()->GetClass()->IsChildOf(AFoodPack::StaticClass()))
			{	
				PickUpNotice = "Press E to pick up";
				ItemName = "FoodPack";
				isTrace = true;
			}

			else if (OutHit.GetActor()->GetClass()->IsChildOf(ASuperPack::StaticClass()))
			{
				PickUpNotice = "Press E to pick up";
				ItemName = "SuperPack";
				isTrace = true;
			}


			else if (OutHit.GetActor()->GetClass()->IsChildOf(ASpeedPickUp::StaticClass()))
			{
				PickUpNotice = "Press E to pick up";
				ItemName = "SpeedPack";
				isTrace = true;
			}
		}
	}
	else
	{
		PickUpNotice = NULL;
		ItemName = NULL;
		isTrace = false;
	}


}

FString AFIT2097Assignment2Character::MyRole()
{
	if (Role == ROLE_Authority)
	{
		return TEXT("Server");
	}
	else
	{
		return TEXT("Client");
	}
}

float AFIT2097Assignment2Character::GetHealth()
{
	//HealthPrecentage = CurrentHealth / 100.0f;
	return HealthPrecentage;
}

float AFIT2097Assignment2Character::GetStamina()
{
	//StaminaPrecentage = CurrentStamina / 100.0f;
	return StaminaPrecentage;
}

float AFIT2097Assignment2Character::GetJoy()
{
	//JoyPrecentage = CurrentJoy / 100.0f;
	return JoyPrecentage;
}

void AFIT2097Assignment2Character::UpdateHealth(float Healthadd)
{

	CurrentHealth = FMath::Clamp(CurrentHealth += Healthadd, 0.0f, 100.0f);
	HealthPrecentage = CurrentHealth / 100.0f;
}

void AFIT2097Assignment2Character::UpdateStamina(float Staminaadd)
{

	CurrentStamina = FMath::Clamp(CurrentStamina += Staminaadd, 0.0f, 100.0f);
	StaminaPrecentage = CurrentStamina / 100.0f;
}

void AFIT2097Assignment2Character::UpdateJoy(float Joyadd)
{

	CurrentJoy = FMath::Clamp(CurrentJoy += Joyadd, 0.0f, 100.0f);
	JoyPrecentage = CurrentJoy / 100.0f;
}

void AFIT2097Assignment2Character::IncreaseSpeed()
{
	if (Role == ROLE_Authority)
	{
		GetCharacterMovement()->MaxWalkSpeed = CurrentSpeed * 10.0f;
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = CurrentSpeed * 10.0f;
	}
}


float AFIT2097Assignment2Character::GetSpeed()
{
	return CurrentSpeed;
}

FText AFIT2097Assignment2Character::GetHealthIntText()
{
	int32 HP = FMath::RoundHalfFromZero(HealthPrecentage * 100);
	FString HPS = FString::FromInt(HP);
	FString HealthHUD = HPS + FString(TEXT("%"));
	FText HPText = FText::FromString(HealthHUD);
	return HPText;
}


void AFIT2097Assignment2Character::GetPosionDamage()
{
	
	if (CurrentHealth <= 0.0f)
		{
			CheckIsDead();
		}
		else
		{
			UpdateHealth(-1.0f);
		}
		CheckIsDead();
}

void AFIT2097Assignment2Character::GetJoyDamage()
{

	if (CurrentJoy <= 0.0f)
		{
			if (CurrentJoy == 0.0f)
			{
				GetCharacterMovement()->MaxWalkSpeed = CurrentSpeed * 0.8f;
			}
		}
		else
		{
			GetCharacterMovement()->MaxWalkSpeed = BaseSpeed;
			UpdateJoy(-2.0f);
		}
	
}

void AFIT2097Assignment2Character::GetStaminaDamage()
{
	
	if (CurrentStamina <= 0.0f)
		{
			if (CurrentStamina == 0.0f)
			{

				GetCharacterMovement()->MaxWalkSpeed = CurrentSpeed * 0.2f;
			}
		}
		else if (CanBeDamage == true)
		{
			GetCharacterMovement()->MaxWalkSpeed = BaseSpeed;
			UpdateStamina(-0.01f);
			CanBeDamage = false;
		}
	
}

void AFIT2097Assignment2Character::PickUp()
{
	if (Role == ROLE_Authority)
	{
		PerformRayTrace();
	}
	else
	{
		PerformRayTrace();
	}
	
}

FString AFIT2097Assignment2Character::GetItemName()
{

	return ItemName;
}

FString AFIT2097Assignment2Character::GetPickUpText()
{

	return PickUpNotice;
}


void AFIT2097Assignment2Character::PerformRayTrace()
{

	FHitResult OutHit;

	FVector Start = FP_Gun->GetComponentLocation();
	FVector ForwardVector = FirstPersonCameraComponent->GetForwardVector();
	FVector End = (Start + (ForwardVector* 1000.0f));

	FCollisionQueryParams CollisionParams;

	DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 1, 0, 1);

	bool isHit = GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECC_Visibility, CollisionParams);

	if (isHit)
		{
			if (OutHit.bBlockingHit)
			{
				GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, FString::Printf(TEXT("You are hitting: %s"), *OutHit.GetActor()->GetName()));
				if (OutHit.GetActor()->GetClass()->IsChildOf(AHealthPack::StaticClass()))
				{
					healthpack = Cast<AHealthPack>(OutHit.GetActor());
					UpdateHealth(20.0f);
					UpdateJoy(20.0f);
					healthpack->Destroy();
				}

				else if (OutHit.GetActor()->GetClass()->IsChildOf(AFoodPack::StaticClass()))
				{
					foodpack = Cast<AFoodPack>(OutHit.GetActor());
					UpdateStamina(20.0f);
					UpdateJoy(20.0f);
					foodpack->Destroy();
				}

				else if (OutHit.GetActor()->GetClass()->IsChildOf(ASuperPack::StaticClass()))
				{
					superpack = Cast<ASuperPack>(OutHit.GetActor());
					UpdateStamina(50.0f);
					UpdateJoy(50.0f);
					UpdateHealth(50.0f);
					superpack->Destroy();
				}

				else if (OutHit.GetActor()->GetClass()->IsChildOf(ASpeedPickUp::StaticClass()))
				{
					speedpack = Cast<ASpeedPickUp>(OutHit.GetActor());
					GetWorldTimerManager().SetTimer(SpeedIncreaseTimerHandle, this, &AFIT2097Assignment2Character::IncreaseSpeed, 30.0f, false);
					speedpack->Destroy();
				}
			}
		}
		else
		{
			healthpack = NULL;
			foodpack = NULL;
			superpack = NULL;
			speedpack = NULL;
		}
	
	


}


void AFIT2097Assignment2Character::CheckIsDead()
{
	if (Role == ROLE_Authority) 
	{
		if (CurrentHealth == 0)
		{
			Dead = "You Dead !!!";
		}
		else
		{
			Dead = NULL;
		}
	}
	else
	{
		if (CurrentHealth == 0)
		{
			Dead = "You Dead !!!";
		}
		else
		{
			Dead = NULL;
		}
	}
}