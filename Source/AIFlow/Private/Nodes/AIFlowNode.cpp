// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#include "Nodes/AIFlowNode.h"
#include "AIFlowAsset.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AIFlowNode)

UBlackboardData* UAIFlowNode::GetBlackboardAsset() const
{
	if (const IFlowBlackboardAssetProvider* BlackboardAssetProvider = Cast<IFlowBlackboardAssetProvider>(GetFlowAsset()))
	{
		return BlackboardAssetProvider->GetBlackboardAsset();
	}

	return nullptr;
}

int32 UAIFlowNode::GetRandomSeed() const
{
	const int32 SuperRandomSeed = Super::GetRandomSeed();

	if (const UAIFlowAsset* AIFlowAsset = Cast<UAIFlowAsset>(GetFlowAsset()))
	{
		return HashCombine(AIFlowAsset->GetRandomSeed(), SuperRandomSeed);
	}

	return SuperRandomSeed;
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

