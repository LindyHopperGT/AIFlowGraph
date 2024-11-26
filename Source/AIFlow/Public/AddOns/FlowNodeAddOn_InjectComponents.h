// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#pragma once

#include "FlowNodeAddOn_InjectComponentsBase.h"

#include "FlowNodeAddOn_InjectComponents.generated.h"

class UActorComponent;
class UFlowInjectComponentsManager;

// Inject a component on a spawned actor
UCLASS(Blueprintable, meta = (DisplayName = "Inject Components"))
class AIFLOW_API UFlowNodeAddOn_InjectComponents : public UFlowNodeAddOn_InjectComponentsBase
{
	GENERATED_BODY()

public:

	UFlowNodeAddOn_InjectComponents();

	// IFlowPerSpawnedActorInterface
	void FinishedSpawningActor_Implementation(AActor* SpawnedActor, UFlowNodeBase* SpawningFlowNodeBase) override;
	// --

	// UFlowNodeBase
	virtual void UpdateNodeConfigText_Implementation() override;
	// --

protected:

	UPROPERTY(EditAnywhere, Category = Configuration, meta = (ShowOnlyInnerProperties))
	FFlowInjectComponentsHelper InjectComponentsHelper;
};
