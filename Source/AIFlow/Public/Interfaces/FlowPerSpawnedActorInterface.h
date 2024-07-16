// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors
 
#pragma once

#include "UObject/Interface.h"

#include "FlowPerSpawnedActorInterface.generated.h"

class AActor;
class UFlowNodeAddOn;

UINTERFACE(MinimalAPI, Blueprintable, DisplayName = "Per-Spawned Actor Interface")
class UFlowPerSpawnedActorInterface : public UInterface
{
	GENERATED_BODY()
};

class AIFLOW_API IFlowPerSpawnedActorInterface
{
	GENERATED_BODY()

public:

	// SpawningFlowNodeBase is the Node or AddOn that did the spawning or is most relevant to the spawning;
	// this is often (but not always) the FlowNode itself.
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "FlowNode")
	void FinishedSpawningActor(AActor* SpawnedActor, UFlowNodeBase* SpawningNodeOrAddOn);
	virtual void FinishedSpawningActor_Implementation(AActor* SpawnedActor, UFlowNodeBase* SpawningNodeOrAddOn) { }

	static bool ImplementsInterfaceSafe(const UFlowNodeAddOn* AddOnTemplate);
};

