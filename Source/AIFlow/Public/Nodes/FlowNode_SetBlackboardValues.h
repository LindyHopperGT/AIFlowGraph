// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#pragma once

#include "AIFlowActorBlackboardHelper.h"
#include "Nodes/AIFlowNode.h"

#include "FlowNode_SetBlackboardValues.generated.h"

// Forward Declarations
class UFlowBlackboardEntryValue;

/**
 * Set blackboard values to the values defined in the Entries array
 */
UCLASS(DisplayName = "Set Blackboard Values", HideCategories = MultipleActorConfiguration)
class AIFLOW_API UFlowNode_SetBlackboardValues : public UAIFlowNode
{
	GENERATED_BODY()

public:

	UFlowNode_SetBlackboardValues();

	// IFlowCoreExecutableInterface
	virtual void ExecuteInput(const FName& PinName) override;
	// --

	// UFlowNodeBase
	virtual void UpdateNodeConfigText_Implementation() override;
	// --

protected:

	virtual TArray<UBlackboardComponent*> GetBlackboardComponentsToApplyTo() const;

protected:

	// Blackboard properties to set on each actor this node is applied to
	UPROPERTY(EditAnywhere, Category = Configuration, meta = (ShowOnlyInnerProperties, DisplayOrder = 3))
	FAIFlowConfigureBlackboardOption EntriesForEveryActor;

	// Method to use when applying the Per-Actor Options to eligible Actors
	UPROPERTY(EditAnywhere, Category = MultipleActorConfiguration, DisplayName = "Per-Actor Options Assignment Method", meta = (DisplayOrder = 4))
	EPerActorOptionsAssignmentMethod PerActorOptionsAssignmentMethod = EPerActorOptionsAssignmentMethod::InOrderWithWrapping;

	// Configured blackboard entry option sets to apply to actors according to the application method
	UPROPERTY(EditAnywhere, Category = MultipleActorConfiguration, DisplayName = "Per-Actor Options", meta = (DisplayOrder = 4))
	TArray<FAIFlowConfigureBlackboardOption> PerActorOptions;

	// Helper struct that shared functionality for manipulating Actor blackboards
	UPROPERTY(EditAnywhere, Category = Configuration, meta = (ShowOnlyInnerProperties, DisplayOrder = 4))
	FAIFlowActorBlackboardHelper ActorBlackboardHelper;
};
