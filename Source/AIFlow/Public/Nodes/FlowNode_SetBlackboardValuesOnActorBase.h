// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#pragma once

#include "FlowNode_SetBlackboardValues.h"
#include "AIFlowActorBlackboardHelper.h"

#include "FlowNode_SetBlackboardValuesOnActorBase.generated.h"

/**
 * Set blackboard values to the values defined in the Entries array
 */
UCLASS(Abstract)
class AIFLOW_API UFlowNode_SetBlackboardValuesOnActorBase : public UFlowNode_SetBlackboardValues
{
	GENERATED_BODY()

public:

	UFlowNode_SetBlackboardValuesOnActorBase();

protected:

	virtual TArray<UBlackboardComponent*> GetBlackboardComponentsToApplyTo() const override;
	virtual TArray<AActor*> TryResolveActorsForBlackboard() const;

protected:

	// Specify an explicit blackboard asset to write to
	UPROPERTY(EditAnywhere, Category = Configuration, DisplayName = "Expected Blackboard Asset for Actor", meta = (DisplayOrder = 2))
	UBlackboardData* ExpectedBlackboardData = nullptr;

	// Where to search for the blackboard component: on the Actor, their Controller (for Pawns) or both.
	UPROPERTY(EditAnywhere, Category = Configuration, DisplayName = "Blackboard Component Search Rule", meta = (DisplayOrder = 2))
	EActorBlackboardSearchRule SearchRule = EActorBlackboardSearchRule::ActorAndController;

	// TODO (gtaylor) Implement blackboard injection if missing feature
	UPROPERTY(VisibleAnywhere, Category = Configuration, DisplayName = "Blackboard Injection Rule", meta = (DisplayOrder = 2))
	EActorBlackboardInjectRule InjectRule = EActorBlackboardInjectRule::DoNotInjectIfMissing;
};
