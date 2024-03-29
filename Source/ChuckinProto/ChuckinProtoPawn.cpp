// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "ChuckinProtoPawn.h"
#include "ChuckinProtoWheelFront.h"
#include "ChuckinProtoWheelRear.h"
#include "ChuckinProtoHud.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "Components/TextRenderComponent.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "WheeledVehicleMovementComponent4W.h"
#include "Engine/SkeletalMesh.h"
#include "Engine/Engine.h"
#include "GameFramework/Controller.h"
#include "UObject/ConstructorHelpers.h"
#include "GameFramework/PlayerController.h"
#include "TimerManager.h"
#include "Public/DrawDebugHelpers.h"
#include "Components/SceneComponent.h"
#include "ChuckinPlayerController.h"
#include "Classes/Kismet/GameplayStatics.h"
#include "ChuckinChickin.h"
#include "Classes/Sound/SoundBase.h" 
#include "Kismet/GameplayStatics.h"
#include "ChuckinHealthComponent.h"
#include "Math/TransformNonVectorized.h"
#include "ChuckinPlayerState.h"
#include "Kismet/KismetMathLibrary.h"
#include "CWheeledVehicleMovementComponent.h"

#ifndef HMD_MODULE_INCLUDED
#define HMD_MODULE_INCLUDED 0
#endif

// Needed for VR Headset
#if HMD_MODULE_INCLUDED
#include "IXRTrackingSystem.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#endif // HMD_MODULE_INCLUDED

const FName AChuckinProtoPawn::LookUpBinding("LookUp");
const FName AChuckinProtoPawn::LookRightBinding("LookRight");
const FName AChuckinProtoPawn::EngineAudioRPM("RPM");

static int32 DebugCarDrawing = 0;
FAutoConsoleVariableRef CVARDebugCarDrawing(
	TEXT("CHUCKIN.DebugCar"),
	DebugCarDrawing,
	TEXT("Draw Debug Lines for Car"),
	ECVF_Cheat);

void AChuckinProtoPawn::FireAt()
{
	if (!ProjectileClass) { return; }

	UGameplayStatics::PlaySoundAtLocation(this, ShootingSoundEffect, GetActorLocation());

	FVector StartLocation = GetMesh()->GetSocketLocation(MuzzleSocketName);
	
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.Instigator = this;
	SpawnParams.Owner = this;
	FRotator CamRotation;

	if (bInCarCameraActive)
	{
		CamRotation = InternalCamera->GetComponentRotation();
	}
	else
	{
		CamRotation = Camera->GetComponentRotation();

		// Only if bInCarCameraActive is FALSE AND the camera is locked, we need to add Pitch to rotation as quick fix for now...
		if (bIsCameraLocked)
		{
			CamRotation.Pitch += 10.f;
		}
	}

	AChuckinChickin* Chicken = Cast<AChuckinChickin>(GetWorld()->SpawnActor<AActor>(ProjectileClass, StartLocation, CamRotation, SpawnParams));

	AChuckinPlayerController* PC = Cast<AChuckinPlayerController>(GetController());
	if (PC)
	{
		PC->LastFireTime = GetWorld()->TimeSeconds;
	}
}


#define LOCTEXT_NAMESPACE "VehiclePawn"

AChuckinProtoPawn::AChuckinProtoPawn(const FObjectInitializer& ObjectInitializer) 
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UCWheeledVehicleMovementComponent>(VehicleMovementComponentName))
{
	// Car mesh
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> CarMesh(TEXT("/Game/VehicleAdv/Vehicle/Vehicle_SkelMesh.Vehicle_SkelMesh"));
	GetMesh()->SetSkeletalMesh(CarMesh.Object);
	
	static ConstructorHelpers::FClassFinder<UObject> AnimBPClass(TEXT("/Game/VehicleAdv/Vehicle/VehicleAnimationBlueprint"));
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	GetMesh()->SetAnimInstanceClass(AnimBPClass.Class);
	//GetMesh()->SetAngularDamping(50.f);

	// Setup friction materials
	static ConstructorHelpers::FObjectFinder<UPhysicalMaterial> SlipperyMat(TEXT("/Game/VehicleAdv/PhysicsMaterials/Slippery.Slippery"));
	SlipperyMaterial = SlipperyMat.Object;
		
	static ConstructorHelpers::FObjectFinder<UPhysicalMaterial> NonSlipperyMat(TEXT("/Game/VehicleAdv/PhysicsMaterials/NonSlippery.NonSlippery"));
	NonSlipperyMaterial = NonSlipperyMat.Object;

	UWheeledVehicleMovementComponent4W* Vehicle4W = CastChecked<UWheeledVehicleMovementComponent4W>(GetVehicleMovement());

	check(Vehicle4W->WheelSetups.Num() == 4);

	// Wheels/Tyres
	// Setup the wheels
	Vehicle4W->WheelSetups[0].WheelClass = UChuckinProtoWheelFront::StaticClass();
	Vehicle4W->WheelSetups[0].BoneName = FName("PhysWheel_FL");
	Vehicle4W->WheelSetups[0].AdditionalOffset = FVector(0.f, -8.f, 0.f);

	Vehicle4W->WheelSetups[1].WheelClass = UChuckinProtoWheelFront::StaticClass();
	Vehicle4W->WheelSetups[1].BoneName = FName("PhysWheel_FR");
	Vehicle4W->WheelSetups[1].AdditionalOffset = FVector(0.f, 8.f, 0.f);

	Vehicle4W->WheelSetups[2].WheelClass = UChuckinProtoWheelRear::StaticClass();
	Vehicle4W->WheelSetups[2].BoneName = FName("PhysWheel_BL");
	Vehicle4W->WheelSetups[2].AdditionalOffset = FVector(0.f, -8.f, 0.f);

	Vehicle4W->WheelSetups[3].WheelClass = UChuckinProtoWheelRear::StaticClass();
	Vehicle4W->WheelSetups[3].BoneName = FName("PhysWheel_BR");
	Vehicle4W->WheelSetups[3].AdditionalOffset = FVector(0.f, 8.f, 0.f);

	// Adjust the tire loading
	Vehicle4W->MinNormalizedTireLoad = 0.0f;
	Vehicle4W->MinNormalizedTireLoadFiltered = 0.2f;
	Vehicle4W->MaxNormalizedTireLoad = 2.0f;
	Vehicle4W->MaxNormalizedTireLoadFiltered = 2.0f;

	// Engine 
	// Torque setup
	Vehicle4W->MaxEngineRPM = 5700.0f;
	Vehicle4W->EngineSetup.TorqueCurve.GetRichCurve()->Reset();
	Vehicle4W->EngineSetup.TorqueCurve.GetRichCurve()->AddKey(0.0f, 400.0f);
	Vehicle4W->EngineSetup.TorqueCurve.GetRichCurve()->AddKey(1890.0f, 500.0f);
	Vehicle4W->EngineSetup.TorqueCurve.GetRichCurve()->AddKey(5730.0f, 400.0f);
 
	// Adjust the steering 
	Vehicle4W->SteeringCurve.GetRichCurve()->Reset();
	Vehicle4W->SteeringCurve.GetRichCurve()->AddKey(0.0f, 1.0f);
	Vehicle4W->SteeringCurve.GetRichCurve()->AddKey(40.0f, 0.7f);
	Vehicle4W->SteeringCurve.GetRichCurve()->AddKey(120.0f, 0.6f);
			
 	// Transmission	
	// We want 4wd
	Vehicle4W->DifferentialSetup.DifferentialType = EVehicleDifferential4W::LimitedSlip_4W;
	
	// Drive the front wheels a little more than the rear
	Vehicle4W->DifferentialSetup.FrontRearSplit = 0.65;

	// Automatic gearbox
	Vehicle4W->TransmissionSetup.bUseGearAutoBox = true;
	Vehicle4W->TransmissionSetup.GearSwitchTime = 0.15f;
	Vehicle4W->TransmissionSetup.GearAutoBoxLatency = 1.0f;

	// Physics settings
	// Adjust the center of mass - the buggy is quite low
	UPrimitiveComponent* UpdatedPrimitive = Cast<UPrimitiveComponent>(Vehicle4W->UpdatedComponent);
	if (UpdatedPrimitive)
	{
		UpdatedPrimitive->BodyInstance.COMNudge = FVector(8.0f, 0.0f, 0.0f);
	}

	// Set the inertia scale. This controls how the mass of the vehicle is distributed.
	Vehicle4W->InertiaTensorScale = FVector(1.0f, 1.333f, 1.2f);

	// Craete Azimuth Gimbal to attach Spring Arm to
	AzimuthGimbal = CreateDefaultSubobject<USceneComponent>("AzimuthGimbal");
	AzimuthGimbal->SetupAttachment(RootComponent);

	// Create a spring arm component for our chase camera
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetRelativeLocation(FVector(0.0f, 0.0f, 34.0f));
	SpringArm->SetWorldRotation(FRotator(-20.0f, 0.0f, 0.0f));

	//Attach to Azimuth Gimbal instead
	SpringArm->SetupAttachment(AzimuthGimbal);
	SpringArm->TargetArmLength = 125.0f;
	SpringArm->bEnableCameraLag = false;
	SpringArm->bEnableCameraRotationLag = false;
	SpringArm->bInheritPitch = true;
	SpringArm->bInheritYaw = true;
	SpringArm->bInheritRoll = true;

	// Create the chase camera component 
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("ChaseCamera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	Camera->SetRelativeLocation(FVector(-125.0, 0.0f, 0.0f));
	Camera->SetRelativeRotation(FRotator(10.0f, 0.0f, 0.0f));
	Camera->bUsePawnControlRotation = false;
	Camera->FieldOfView = 90.f;

	// Create In-Car camera component 
	InternalCameraOrigin = FVector(-34.0f, -10.0f, 50.0f);
	InternalCameraBase = CreateDefaultSubobject<USceneComponent>(TEXT("InternalCameraBase"));
	InternalCameraBase->SetRelativeLocation(InternalCameraOrigin);
	InternalCameraBase->SetupAttachment(GetMesh());

	InternalCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("InternalCamera"));
	InternalCamera->bUsePawnControlRotation = false;
	InternalCamera->FieldOfView = 90.f;
	InternalCamera->SetupAttachment(InternalCameraBase);

	// In car HUD
	// Create text render component for in car speed display
	InCarSpeed = CreateDefaultSubobject<UTextRenderComponent>(TEXT("IncarSpeed"));
	InCarSpeed->SetRelativeScale3D(FVector(0.1f, 0.1f, 0.1f));
	InCarSpeed->SetRelativeLocation(FVector(35.0f, -6.0f, 20.0f));
	InCarSpeed->SetRelativeRotation(FRotator(0.0f, 180.0f, 0.0f));
	InCarSpeed->SetupAttachment(GetMesh());

	// Create text render component for in car gear display
	InCarGear = CreateDefaultSubobject<UTextRenderComponent>(TEXT("IncarGear"));
	InCarGear->SetRelativeScale3D(FVector(0.1f, 0.1f, 0.1f));
	InCarGear->SetRelativeLocation(FVector(35.0f, 5.0f, 20.0f));
	InCarGear->SetRelativeRotation(FRotator(0.0f, 180.0f, 0.0f));
	InCarGear->SetupAttachment(GetMesh());
	
	// Setup the audio component and allocate it a sound cue
	static ConstructorHelpers::FObjectFinder<USoundCue> SoundCue(TEXT("/Game/VehicleAdv/Sound/Engine_Loop_Cue.Engine_Loop_Cue"));
	EngineSoundComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("EngineSound"));
	EngineSoundComponent->SetSound(SoundCue.Object);
	EngineSoundComponent->SetupAttachment(GetMesh());

	// Colors for the in-car gear display. One for normal one for reverse
	GearDisplayReverseColor = FColor(255, 0, 0, 255);
	GearDisplayColor = FColor(255, 255, 255, 255);

	bIsLowFriction = false;
	bInReverseGear = false;

	// NEW STUFF
	HealthComp = CreateDefaultSubobject<UChuckinHealthComponent>("HealthComp"); 

	//LivesWidgetComp = CreateDefaultSubobject<UWidgetComponent>("LivesWidgetComp");
	//LivesWidgetComp->SetupAttachment(RootComponent);




	// bullets per minute
	RateOfFire = 50.f;
	MuzzleSocketName = "ChickenFire";
	ChickenYawOffset = 0.f;
	ChickenPitchOffset = 0.f;
	LaunchSpeed = 10000.f;
	bIsDead = false;

	// Initialize bIsCameraLocked as false, this will get overriden upon respawns in beginplay as the PlayerController holds a more permanent value
	bIsCameraLocked = false;

	// Capture the initial transform for these components so we can Lock them back here when we press "c" keybind to lock camera
	if (SpringArm)
	{
		DefaultSpringArmTransform = SpringArm->GetRelativeTransform();
	}
	if (AzimuthGimbal)
	{
		DefaultAzimuthTransform = AzimuthGimbal->GetRelativeTransform();
	}

}

void AChuckinProtoPawn::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// set up gameplay key bindings
	check(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AChuckinProtoPawn::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AChuckinProtoPawn::MoveRight);
	PlayerInputComponent->BindAxis(LookUpBinding);
	PlayerInputComponent->BindAxis(LookRightBinding);
	PlayerInputComponent->BindAxis("LookUpNew", this, &AChuckinProtoPawn::LookUpNew);
	PlayerInputComponent->BindAxis("LookRightNew", this, &AChuckinProtoPawn::LookRightNew);

	PlayerInputComponent->BindAction("Handbrake", IE_Pressed, this, &AChuckinProtoPawn::OnHandbrakePressed);
	PlayerInputComponent->BindAction("Handbrake", IE_Released, this, &AChuckinProtoPawn::OnHandbrakeReleased);
	PlayerInputComponent->BindAction("SwitchCamera", IE_Pressed, this, &AChuckinProtoPawn::OnToggleCamera);

	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &AChuckinProtoPawn::OnResetVR); 

	PlayerInputComponent->BindAction("FlipCar", IE_Pressed, this, &AChuckinProtoPawn::FlipCar);
	PlayerInputComponent->BindAction("LockCamera", IE_Pressed, this, &AChuckinProtoPawn::LockCamera);
	
	

}


void AChuckinProtoPawn::MoveForward(float Val)
{
	GetVehicleMovementComponent()->SetThrottleInput(Val);

}

void AChuckinProtoPawn::MoveRight(float Val)
{
	GetVehicleMovementComponent()->SetSteeringInput(Val);
}

void AChuckinProtoPawn::OnHandbrakePressed()
{
	GetVehicleMovementComponent()->SetHandbrakeInput(true);
}

void AChuckinProtoPawn::OnHandbrakeReleased()
{
	GetVehicleMovementComponent()->SetHandbrakeInput(false);
}

void AChuckinProtoPawn::OnToggleCamera()
{
	// TODO: CHANGE camera to use for camera rotation to the INSIDE camera.
	EnableIncarView(!bInCarCameraActive);
}

void AChuckinProtoPawn::EnableIncarView(const bool bState)
{
	if (bState != bInCarCameraActive)
	{
		bInCarCameraActive = bState;
		
		if (bState == true)
		{
			OnResetVR();
			Camera->Deactivate();
			InternalCamera->Activate();
		}
		else
		{
			InternalCamera->Deactivate();
			Camera->Activate();
		}
		
		InCarSpeed->SetVisibility(bInCarCameraActive);
		InCarGear->SetVisibility(bInCarCameraActive);
	}
}

void AChuckinProtoPawn::Tick(float Delta)
{
	Super::Tick(Delta);
	//if (bIsDead) { return; }
	// Setup the flag to say we are in reverse gear
	bInReverseGear = GetVehicleMovement()->GetCurrentGear() < 0;
	
	// Update phsyics material
	UpdatePhysicsMaterial();

	// Update the strings used in the hud (incar and onscreen)
	UpdateHUDStrings();

	// Set the string in the incar hud
	SetupInCarHUD();

	bool bHMDActive = false;
#if HMD_MODULE_INCLUDED
	if ((GEngine->XRSystem.IsValid() == true ) && ( (GEngine->XRSystem->IsHeadTrackingAllowed() == true) || (GEngine->IsStereoscopic3D() == true)))
	{
		bHMDActive = true;
	}
#endif // HMD_MODULE_INCLUDED
	if( bHMDActive == false )
	{
		if ( (InputComponent) && (bInCarCameraActive == true ))
		{
			FRotator HeadRotation = InternalCamera->RelativeRotation;
			HeadRotation.Pitch += InputComponent->GetAxisValue(LookUpBinding);
			HeadRotation.Yaw += InputComponent->GetAxisValue(LookRightBinding);
			InternalCamera->RelativeRotation = HeadRotation;
		}
	}	

	// Pass the engine RPM to the sound component
	float RPMToAudioScale = 2500.0f / GetVehicleMovement()->GetEngineMaxRotationSpeed();
	EngineSoundComponent->SetFloatParameter(EngineAudioRPM, GetVehicleMovement()->GetEngineRotationSpeed()*RPMToAudioScale);
}

void AChuckinProtoPawn::BeginPlay()
{
	Super::BeginPlay();

	bool bWantInCar = false;
	// First disable both speed/gear displays 
	bInCarCameraActive = false;
	InCarSpeed->SetVisibility(bInCarCameraActive);
	InCarGear->SetVisibility(bInCarCameraActive);

	// Enable in car view if HMD is attached
#if HMD_MODULE_INCLUDED
	bWantInCar = UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled();
#endif // HMD_MODULE_INCLUDED

	EnableIncarView(bWantInCar);
	// Start an engine sound playing
	EngineSoundComponent->Play();

	TimeBetweenShots = 60.f / RateOfFire;

	// This should be moved to a method that fires once this pawn is possessed. We can only find playercontroller reference with the UGamePlayStatics method.
	// Using the self->GetController() won't work at BeginPlay()... But this workaround below does.
	// Get this boolean value from player controller since player controller will have a persistent value when pawn dies and respawns
	AChuckinPlayerController* PC = Cast<AChuckinPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (PC)
	{
		bIsCameraLocked = PC->bShouldCameraBeLocked;
	}


}

void AChuckinProtoPawn::OnResetVR()
{
#if HMD_MODULE_INCLUDED
	if (GEngine->XRSystem.IsValid())
	{
		GEngine->XRSystem->ResetOrientationAndPosition();
		InternalCamera->SetRelativeLocation(InternalCameraOrigin);
		GetController()->SetControlRotation(FRotator());
	}
#endif // HMD_MODULE_INCLUDED
}

void AChuckinProtoPawn::FlipCar()
{
	FRotator NewRotation(0);
	NewRotation.Yaw = GetActorRotation().Yaw;

	SetActorRotation(NewRotation, ETeleportType::TeleportPhysics);
}

void AChuckinProtoPawn::LockCamera()
{
	// This boolean will stop the mouse inputs from being processed for camera movement
	bIsCameraLocked = !bIsCameraLocked;

	// Set up a persistent boolean value in the player controller for if player dies and respawns
	AChuckinPlayerController* PC = Cast<AChuckinPlayerController>(GetController());
	if (PC)
	{

		PC->bShouldCameraBeLocked = bIsCameraLocked;
	}

	// If we are now locking the camera, let's set the relative transform for the azimuth and spring arm to the default values we captured in constructor
	if (bIsCameraLocked)
	{
		if (AzimuthGimbal)
		{
			AzimuthGimbal->SetRelativeTransform(DefaultAzimuthTransform);
		}
		if (SpringArm)
		{
			SpringArm->SetRelativeTransform(DefaultSpringArmTransform);
		}
	}

}

void AChuckinProtoPawn::UpdateHUDStrings()
{
	float KPH = FMath::Abs(GetVehicleMovement()->GetForwardSpeed()) * 0.036f;
	int32 KPH_int = FMath::FloorToInt(KPH);
	int32 Gear = GetVehicleMovement()->GetCurrentGear();

	// Using FText because this is display text that should be localizable
	SpeedDisplayString = FText::Format(LOCTEXT("SpeedFormat", "{0} km/h"), FText::AsNumber(KPH_int));


	if (bInReverseGear == true)
	{
		GearDisplayString = FText(LOCTEXT("ReverseGear", "R"));
	}
	else
	{
		GearDisplayString = (Gear == 0) ? LOCTEXT("N", "N") : FText::AsNumber(Gear);
	}

}

void AChuckinProtoPawn::SetupInCarHUD()
{
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if ((PlayerController != nullptr) && (InCarSpeed != nullptr) && (InCarGear != nullptr))
	{
		// Setup the text render component strings
		InCarSpeed->SetText(SpeedDisplayString);
		InCarGear->SetText(GearDisplayString);
		
		if (bInReverseGear == false)
		{
			InCarGear->SetTextRenderColor(GearDisplayColor);
		}
		else
		{
			InCarGear->SetTextRenderColor(GearDisplayReverseColor);
		}
	}
}

void AChuckinProtoPawn::UpdatePhysicsMaterial()
{
	if (GetActorUpVector().Z < 0)
	{
		if (bIsLowFriction == true)
		{
			GetMesh()->SetPhysMaterialOverride(NonSlipperyMaterial);
			bIsLowFriction = false;
		}
		else
		{
			GetMesh()->SetPhysMaterialOverride(SlipperyMaterial);
			bIsLowFriction = true;
		}
	}
}

void AChuckinProtoPawn::LookUpNew(float Val)
{
	if (bIsCameraLocked) { return; }

	if (SpringArm)
	{
		SpringArm->AddLocalRotation(FRotator(Val, 0.f, 0.f));
	}
}

void AChuckinProtoPawn::LookRightNew(float Val)
{
	if (bIsCameraLocked) { return; }

	if (AzimuthGimbal)
	{
		AzimuthGimbal->AddLocalRotation(FRotator(0.f, Val, 0.f));
	}

}

#undef LOCTEXT_NAMESPACE
