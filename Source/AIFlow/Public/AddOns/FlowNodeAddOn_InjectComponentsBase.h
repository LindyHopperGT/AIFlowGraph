// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#pragma once

#include "AddOns/AIFlowNodeAddOn.h"
#include "Interfaces/FlowPerSpawnedActorInterface.h"
#include "Types/FlowInjectComponentsHelper.h"

#include "GameplayTagContainer.h"

#include "FlowNodeAddOn_InjectComponentsBase.generated.h"

class UActorComponent;
class UFlowInjectComponentsManager;

// Inject a component on a spawned actor
UCLASS(Abstract, Blueprintable, meta = (DisplayName = "Inject Components (base class)"))
class AIFLOW_API UFlowNodeAddOn_InjectComponentsBase
	: public UAIFlowNodeAddOn
	, public IFlowPerSpawnedActorInterface
{
	GENERATED_BODY()

public:

	UFlowNodeAddOn_InjectComponentsBase();

	// IFlowPerSpawnedActorInterface
	void FinishedSpawningActor_Implementation(AActor* SpawnedActor, UFlowNodeBase* SpawningFlowNodeBase) override;
	// --

	// IFlowCoreExecutableInterface
	virtual void DeinitializeInstance() override;
	// --

protected:

	UFUNCTION()
	void OnBeforeActorRemoved(AActor* Actor);

	void EnsureInjectComponentsManager();
	void CleanupInjectComponentsManager();

	// Functions for subclasses to apply additional monitoring to Actors
	virtual void OnStartMonitoringActor(AActor& Actor) { }
	virtual void OnStopMonitoringActor(AActor& Actor) { }

protected:

	// Manager object to inject and remove components from actors
	UPROPERTY()
	TObjectPtr<UFlowInjectComponentsManager> InjectComponentsManager = nullptr;
};
