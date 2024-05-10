// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#pragma once

#include "Nodes/AIFlowNode.h"

#include "FlowNode_SetBlackboardValues.generated.h"

// Forward Declarations
class UFlowBlackboardEntryValue;

/**
 * Set blackboard values to the values defined in the Entries array
 */
UCLASS(DisplayName = "Set Blackboard Values")
class AIFLOW_API UFlowNode_SetBlackboardValues : public UAIFlowNode
{
	GENERATED_BODY()

public:
	UFlowNode_SetBlackboardValues();

	//~Begin IFlowCoreExecutableInterface
	virtual void ExecuteInput(const FName& PinName) override;
	//~End IFlowCoreExecutableInterface

	//~Begin UFlowNodeBase
	virtual void UpdateNodeConfigText_Implementation() override;
	//~End UFlowNodeBase

protected:

	// Entries to set on the blackboard
	UPROPERTY(EditAnywhere, Instanced, Category = Configuration)
	TArray<UFlowBlackboardEntryValue*> Entries;
};
