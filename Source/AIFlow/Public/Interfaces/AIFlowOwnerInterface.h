// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#pragma once

#include "UObject/Interface.h"

#include "AIFlowOwnerInterface.generated.h"

// Forward Declarations
class UAIFlowAsset;

UINTERFACE(MinimalAPI, BlueprintType, DisplayName = "AI Flow Owner Interface")
class UAIFlowOwnerInterface : public UInterface
{
	GENERATED_BODY()
};

class AIFLOW_API IAIFlowOwnerInterface
{
	GENERATED_BODY()

public:

	// Called at the end of InitializeInstance for a new AIFlowAsset
	UFUNCTION(BlueprintNativeEvent, Category = "FlowAsset")
	void PostFlowAssetInitializeInstance(UAIFlowAsset* AIFlowAsset);
	virtual void PostFlowAssetInitializeInstance_Implementation(UAIFlowAsset* AIFlowAsset) { }
};
