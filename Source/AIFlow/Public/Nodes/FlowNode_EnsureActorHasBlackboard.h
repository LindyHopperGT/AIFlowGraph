// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#pragma once

#include "AIFlowNode.h"
#include "AIFlowActorBlackboardHelper.h"

#include "FlowNode_EnsureActorHasBlackboard.generated.h"

class UBlackboardComponent;

/**
 * Ensure an Actor has a blackboard.
 * Can optionally specify the actor and the blackboard.
 * The desired blackboard component will be injected if it cannot be found on the actor
 */
UCLASS(Blueprintable, meta = (DisplayName = "Ensure Actor Has Blackboard"))
class AIFLOW_API UFlowNode_EnsureActorHasBlackboard : public UAIFlowNode
{
	GENERATED_BODY()

public:

	UFlowNode_EnsureActorHasBlackboard();

	// IFlowCoreExecutableInterface
	virtual void ExecuteInput(const FName& PinName) override;
	virtual void DeinitializeInstance() override;
	// --

protected:

	void EnsureInjectComponentsManager();
	void CleanupInjectComponentsManager();

	UBlackboardComponent* EnsureActorHasBlackboard(AActor& ResolvedActor);

	UFUNCTION()
	void OnBeforeActorRemoved(AActor* RemovedActor);

	// Functions for subclasses to apply additional monitoring to Actors
	virtual void OnStartMonitoringActor(AActor& Actor) { }
	virtual void OnStopMonitoringActor(AActor& Actor) { }

protected:

	// Optional specific actor to use to look for (or inject) the blackboard.
	// If not specified, will use the flow graph's owning actor.
	UPROPERTY(Transient, meta = (DefaultForInputFlowPin, FlowPinType = "Object", DisplayPriority = 2))
	TObjectPtr<AActor> SpecificActor = nullptr;

	// Specific blackboard to use (optional, defaults to the flow asset's blackboard)
	UPROPERTY(EditAnywhere, Category = Configuration, DisplayName = "Specific Blackboard", meta = (DisplayPriority = 2))
	TObjectPtr<UBlackboardData> SpecificBlackboardAsset = nullptr;

	// Injection rule (generally inject onto the Actor, but occasionally you may wish to inject onto the actor's controller)
	UPROPERTY(EditAnywhere, AdvancedDisplay, Category = Configuration, DisplayName = "Blackboard Injection Rule", meta = (DisplayPriority = 3))
	EActorBlackboardInjectRule InjectRule = EActorBlackboardInjectRule::InjectOntoActorIfMissing;

	// Search rule to use to find the Blackboard
	UPROPERTY(EditAnywhere, Category = Configuration, DisplayName = "Blackboard Search Rule", meta = (DisplayPriority = 2))
	EActorBlackboardSearchRule BlackboardSearchRule = EActorBlackboardSearchRule::ActorOnly;

	// Specific blackboard component subclass to use (optional, defaults to the flow asset's blackboard defined component class)
	UPROPERTY(EditAnywhere, AdvancedDisplay, Category = Configuration, DisplayName = "Specific Blackboard Component", meta = (DisplayPriority = 2))
	TSubclassOf<UBlackboardComponent> SpecificBlackboardComponentClass = nullptr;

	// Manager object to inject and remove components from actors
	UPROPERTY()
	TObjectPtr<UFlowInjectComponentsManager> InjectComponentsManager = nullptr;

public:

	static const FName OUTPIN_Success;
	static const FName OUTPIN_Failed;
};
