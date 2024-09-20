// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#include "AddOns/AIFlowNodeAddOn.h"
#include "FlowAsset.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AIFlowNodeAddOn)

UBlackboardComponent* UAIFlowNodeAddOn::GetBlackboardComponent() const
{
	if (IFlowBlackboardInterface* FlowBlackboardInterface = Cast<IFlowBlackboardInterface>(GetFlowAsset()))
	{
		return FlowBlackboardInterface->GetBlackboardComponent();
	}

	return nullptr;
}

UBlackboardData* UAIFlowNodeAddOn::GetBlackboardAsset() const
{
	if (const IFlowBlackboardAssetProvider* BlackboardAssetProvider = Cast<IFlowBlackboardAssetProvider>(GetFlowAsset()))
	{
		return BlackboardAssetProvider->GetBlackboardAsset();
	}

	return nullptr;
}

#if WITH_EDITOR
UBlackboardData* UAIFlowNodeAddOn::GetBlackboardAssetForPropertyHandle(const TSharedPtr<IPropertyHandle>& PropertyHandle) const
{
	if (const IFlowBlackboardAssetProvider* BlackboardAssetProvider = Cast<IFlowBlackboardAssetProvider>(GetFlowAsset()))
	{
		return BlackboardAssetProvider->GetBlackboardAssetForPropertyHandle(PropertyHandle);
	}

	return nullptr;
}
#endif // WITH_EDITOR
