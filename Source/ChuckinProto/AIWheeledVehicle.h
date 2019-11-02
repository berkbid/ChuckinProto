// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WheeledVehicle.h"
#include "AIWheeledVehicle.generated.h"

class UPhysicalMaterial;
class UAudioComponent;

UCLASS()
class CHUCKINPROTO_API AAIWheeledVehicle : public AWheeledVehicle
{
	GENERATED_BODY()
	


	/** Audio component for the engine sound */
	UPROPERTY(Category = Display, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UAudioComponent* EngineSoundComponent;

public:
	AAIWheeledVehicle(const FObjectInitializer& ObjectInitializer);

	// Begin Actor interface
	virtual void Tick(float Delta) override;

	/** Update the physics material used by the vehicle mesh */
	void UpdatePhysicsMaterial();

	/** Returns EngineSoundComponent subobject **/
	FORCEINLINE UAudioComponent* GetEngineSoundComponent() const { return EngineSoundComponent; }

	/** Are we in reverse gear */
	UPROPERTY(Category = Camera, VisibleDefaultsOnly, BlueprintReadOnly)
	bool bInReverseGear;

	static const FName EngineAudioRPM;

private:
	/* Are we on a 'slippery' surface */
	bool bIsLowFriction;

	/** Slippery Material instance */
	UPhysicalMaterial* SlipperyMaterial;

	/** Non Slippery Material instance */
	UPhysicalMaterial* NonSlipperyMaterial;
};
