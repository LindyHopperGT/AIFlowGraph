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

	// TODO (gtaylor) Implement blackboard injection if missing feature
	UPROPERTY(VisibleAnywhere, Category = Configuration, DisplayName = "Blackboard Injection Rule", meta = (DisplayPriority = 3))
	EActorBlackboardInjectRule InjectRule = EActorBlackboardInjectRule::DoNotInjectIfMissing;
};
