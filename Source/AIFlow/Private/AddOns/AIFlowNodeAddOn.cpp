// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#include "AddOns/AIFlowNodeAddOn.h"
#include "FlowAsset.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AIFlowNodeAddOn)

UBlackboardComponent* UAIFlowNodeAddOn::GetBlackboardComponent() const
{
	IFlowBlackboardInterface* FlowBlackboardInterface = Cast<IFlowBlackboardInterface>(GetFlowAsset());
	if (FlowBlackboardInterface)
	{
		return FlowBlackboardInterface->GetBlackboardComponent();
	}

	return nullptr;
}


UBlackboardData* UAIFlowNodeAddOn::GetBlackboardAsset() const
{
	const IBlackboardAssetProvider* BlackboardAssetProvider = Cast<IBlackboardAssetProvider>(GetFlowAsset());
	if (BlackboardAssetProvider)
	{
		return BlackboardAssetProvider->GetBlackboardAsset();
	}

	return nullptr;
}
