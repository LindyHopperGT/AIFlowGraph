// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#include "Nodes/AIFlowNode.h"
#include "FlowAsset.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AIFlowNode)

UBlackboardData* UAIFlowNode::GetBlackboardAsset() const
{
	if (const IFlowBlackboardAssetProvider* BlackboardAssetProvider = Cast<IFlowBlackboardAssetProvider>(GetFlowAsset()))
	{
		return BlackboardAssetProvider->GetBlackboardAsset();
	}

	return nullptr;
}

#if WITH_EDITOR
UBlackboardData* UAIFlowNode::GetBlackboardAssetForPropertyHandle(const TSharedPtr<IPropertyHandle>& PropertyHandle) const
{
	if (const IFlowBlackboardAssetProvider* BlackboardAssetProvider = Cast<IFlowBlackboardAssetProvider>(GetFlowAsset()))
	{
		return BlackboardAssetProvider->GetBlackboardAssetForPropertyHandle(PropertyHandle);
	}

	return nullptr;
}
#endif // WITH_EDITOR

UBlackboardComponent* UAIFlowNode::GetBlackboardComponent() const
{
	IFlowBlackboardInterface* FlowBlackboardInterface = Cast<IFlowBlackboardInterface>(GetFlowAsset());
	if (FlowBlackboardInterface)
	{
		return FlowBlackboardInterface->GetBlackboardComponent();
	}

	return nullptr;
}

