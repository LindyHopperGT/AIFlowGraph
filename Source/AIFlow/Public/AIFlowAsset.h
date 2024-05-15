// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#pragma once

#include "FlowAsset.h"
#include "Interfaces/FlowBlackboardInterface.h"

#include "BehaviorTree/BlackboardAssetProvider.h"

#include "AIFlowAsset.generated.h"

// Forward Declarations
class UBlackboardData;
class UBlackboardComponent;
class UFlowInjectComponentsManager;

/**
 * Flow Asset subclass to add AI utility (specifically blackboard) capabilities
 */
UCLASS(BlueprintType, DisplayName = "AI Flow Asset")
class AIFLOW_API UAIFlowAsset
	: public UFlowAsset
	, public IBlackboardAssetProvider
	, public IFlowBlackboardInterface
{
	GENERATED_UCLASS_BODY()

public:

	//Begin UFlowAsset
	virtual void InitializeInstance(const TWeakObjectPtr<UObject> InOwner, UFlowAsset* InTemplateAsset) override;
	virtual void DeinitializeInstance() override;
	//End UFlowAsset

	//~Begin IFlowBlackboardInterface
	virtual UBlackboardComponent* GetBlackboardComponent() const override;
	//~End IFlowBlackboardInterface

	//~Begin IBlackboardAssetProvider
	virtual UBlackboardData* GetBlackboardAsset() const override;
	//~End IBlackboardAssetProvider

protected:

	virtual void CreateAndRegisterBlackboardComponent();
	virtual void DestroyAndUnregisterBlackboardComponent();
	virtual void SetKeySelfOnBlackboardComponent(UBlackboardComponent* BlackboardComp) const;

	// Return the BlackboardData to use at runtime
	// (subclasses may want to instance this class For Reasons)
	virtual UBlackboardData* EnsureRuntimeBlackboardData() const { return BlackboardAsset; }

protected:

	// Blackboard asset for this FlowAsset
	UPROPERTY(EditAnywhere, Category = "AI Flow Asset")
	TObjectPtr<UBlackboardData> BlackboardAsset = nullptr;

	// Cached blackboard component (on the owning actor)
	UPROPERTY(Transient)
	TWeakObjectPtr<UBlackboardComponent> BlackboardComponent = nullptr;

	// Manager object to inject and remove blackboard components from the Flow owning Actor
	UPROPERTY(Transient)
	TObjectPtr<UFlowInjectComponentsManager> InjectComponentsManager = nullptr;
};
