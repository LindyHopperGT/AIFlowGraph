// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#include "Nodes/AIFlowNode.h"
#include "FlowAsset.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AIFlowNode)

UBlackboardData* UAIFlowNode::GetBlackboardAsset() const
{
	const IBlackboardAssetProvider* BlackboardAssetProvider = Cast<IBlackboardAssetProvider>(GetFlowAsset());
	if (BlackboardAssetProvider)
	{
		return BlackboardAssetProvider->GetBlackboardAsset();
	}

	return nullptr;
}

UBlackboardComponent* UAIFlowNode::GetBlackboardComponent() const
{
	IFlowBlackboardInterface* FlowBlackboardInterface = Cast<IFlowBlackboardInterface>(GetFlowAsset());
	if (FlowBlackboardInterface)
	{
		return FlowBlackboardInterface->GetBlackboardComponent();
	}

	return nullptr;
}

