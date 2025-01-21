// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#pragma once

#include "Nodes/FlowNode_SetBlackboardValuesOnActorBase.h"
#include "Types/FlowBlackboardEntry.h"

#include "FlowNode_SetBlackboardValuesOnActorByKey.generated.h"

/**
 * Set blackboard values to the values defined in the Entries array
 * on an actor's blackboard.
 * The actor is found via a blackboard key on the flow graph's blackboard.
 */
UCLASS(DisplayName = "Set Blackboard Values (on Actor by Key)", HideCategories = MultipleActorConfiguration)
class AIFLOW_API UFlowNode_SetBlackboardValuesOnActorByKey : public UFlowNode_SetBlackboardValuesOnActorBase
{
	GENERATED_BODY()

public:
	UFlowNode_SetBlackboardValuesOnActorByKey();

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

	TArray<AActor*> TryResolveActorsForBlackboard() const override;

protected:

	// BlackboardKey (in this flow graph's blackboard) 
	// to source the Actors to Set Blackboard Values On (on their blackboards)
	UPROPERTY(EditAnywhere, Category = Configuration, DisplayName = "Key for Actor to Set Blackboard Values On", meta = (DisplayPriority = 1))
	FFlowBlackboardEntry BlackboardActorKey;
};
