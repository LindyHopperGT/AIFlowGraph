// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#pragma once

#include "AddOns/AIFlowNodeAddOn.h"

#include "FlowNodeAddOn_SetBlackboardValues.generated.h"

// Forward Declarations
class UFlowBlackboardEntryValue;

/**
 * Set blackboard values to the set values
 */
UCLASS(DisplayName = "Set Blackboard Values")
class AIFLOW_API UFlowNodeAddOn_SetBlackboardValues : public UAIFlowNodeAddOn
{
	GENERATED_BODY()

public:
	UFlowNodeAddOn_SetBlackboardValues();

	//~Begin UMKTFlowNodeAddOn
	virtual EFlowAddOnAcceptResult AcceptFlowNodeAddOnParent_Implementation(const UFlowNodeBase* ParentTemplate) const override;
	//~End UMKTFlowNodeAddOn

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
