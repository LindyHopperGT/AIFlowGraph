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

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void FinishedSpawningActor(AActor* SpawnedActor);
	virtual void FinishedSpawningActor_Implementation(AActor* SpawnedActor) { }

	static bool ImplementsInterfaceSafe(const UFlowNodeAddOn* AddOnTemplate);
};

