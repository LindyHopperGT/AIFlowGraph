// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#pragma once

#include "Nodes/FlowNode_SetBlackboardValues.h"
#include "Types/FlowBlackboardEntry.h"

#include "FlowNode_SetBlackboardValuesOnActor.generated.h"

/**
 * Set blackboard values to the values defined in the Entries array
 * on an actor's blackboard.
 * The actor is found via a blackboard key on the flow graph's blackboard.
 */
UCLASS(DisplayName = "Set Blackboard Values")
class AIFLOW_API UFlowNode_SetBlackboardValuesOnActor : public UFlowNode_SetBlackboardValues
{
	GENERATED_BODY()

public:
	UFlowNode_SetBlackboardValuesOnActor();

	// UObject
#if WITH_EDITOR
	virtual void PostInitProperties() override;
#endif // WITH_EDITOR
	// --

	// UFlowNodeBase
	virtual void UpdateNodeConfigText_Implementation() override;
	// --

	// IFlowBlackboardAssetProvider
#if WITH_EDITOR
	virtual UBlackboardData* GetBlackboardAssetForPropertyHandle(const TSharedPtr<IPropertyHandle>& PropertyHandle) const override;
#endif // WITH_EDITOR
	// --

protected:

	// UFlowNode_SetBlackboardValues
	virtual TArray<AActor*> TryResolveActorsForBlackboard() const override;
	// --

	bool TryAddSpecificActor(TArray<AActor*>& InOutResolvedActors) const;
	bool TryAddBlackboardKeyedActor(TArray<AActor*>& InOutResolvedActors) const;

protected:

	// TODO (update the naming for SpecificActor to array

	// Optional specific actor to use to look for (or inject) the blackboard.
	// If not specified, will use the flow graph's owning actor.
	UPROPERTY(Transient, meta = (DefaultForInputFlowPin, FlowPinType = "Object", DisplayPriority = 1))
	TObjectPtr<AActor> SpecificActor = nullptr;

	// BlackboardKey (in this flow graph's blackboard) 
	// to source the Actors to Set Blackboard Values On (on their blackboards)
	UPROPERTY(EditAnywhere, Category = Configuration, DisplayName = "Key for Actor to Set Blackboard Values On", meta = (DisplayPriority = 1))
	FFlowBlackboardEntry BlackboardActorKey;

	static FName INPIN_SpecificActor;
};
