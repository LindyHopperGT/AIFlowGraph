// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#pragma once

#include "BehaviorTree/BlackboardAssetProvider.h"
#include "Nodes/FlowNode.h"

#include "Interfaces/FlowBlackboardInterface.h"

#include "AIFlowNode.generated.h"

/**
 * A base flow node class for Flow Nodes that access Blackboards
 */
UCLASS(Abstract, Blueprintable, BlueprintType, DisplayName = "AI Flow Node")
class AIFLOW_API UAIFlowNode
	: public UFlowNode
	, public IBlackboardAssetProvider
	, public IFlowBlackboardInterface
{
	GENERATED_BODY()

public:

	//~Begin IFlowBlackboardInterface
	virtual UBlackboardComponent* GetBlackboardComponent() const override;
	//~End IFlowBlackboardInterface

	//~Begin IBlackboardAssetProvider
	virtual UBlackboardData* GetBlackboardAsset() const override;
	//~End IBlackboardAssetProvider
};
