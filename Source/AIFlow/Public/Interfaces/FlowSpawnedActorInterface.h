// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors
 
#pragma once

#include "UObject/Interface.h"

#include "FlowSpawnedActorInterface.generated.h"

// Interface for actors that were spawned by a Flow Node/AddOn
// May be implemented by the Actor that is spawned and/or its Components, to receive the FinishedSpawningActor call.
UINTERFACE(MinimalAPI, Blueprintable, DisplayName = "Flow-Spawned Actor Interface")
class UFlowSpawnedActorInterface : public UInterface
{
	GENERATED_BODY()
};

class AIFLOW_API IFlowSpawnedActorInterface
{
	GENERATED_BODY()

public:

	// Function called for Actors spawned by Flow nodes, after all of Flow-based post-spawn setup has completed.
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "FlowNode")
	void FinishedSpawningActorFromFlow(AActor* SpawnedActor, UFlowNodeBase* SpawningNodeOrAddOn);
	virtual void FinishedSpawningActorFromFlow_Implementation(AActor* SpawnedActor, UFlowNodeBase* SpawningNodeOrAddOn) { }

	// Function called by the flow node or addon that spawned an actor to dispatch the FinishedSpawningActorFromFlow function to
	// the SpawnedActor (if it implements this interface) and/or its Components (if they implement this interface).
	static void DispatchFinishedSpawningActorFromFlowToActorAndComponents(AActor* SpawnedActor, UFlowNodeBase* SpawningNodeOrAddOn);

	static bool ImplementsInterfaceSafe(const UObject* Object);
};

