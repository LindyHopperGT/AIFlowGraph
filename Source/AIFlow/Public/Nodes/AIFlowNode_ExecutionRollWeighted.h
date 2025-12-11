// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#pragma once

#include "AIFlowNode.h"
#include "AIFlowNode_ExecutionRollWeighted.generated.h"

USTRUCT(BlueprintType, meta = (ShowOnlyInnerProperties))
struct FAIFlowNode_RollWeightedOption
{
	GENERATED_BODY()

public:

	FAIFlowNode_RollWeightedOption() = default;
	FAIFlowNode_RollWeightedOption(const FName InOutputName, int32 InWeight);
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RollWeightedOption")
	FName OutputName;
	
	// Weight for this option (higher is better, zero weight will not spawn at all)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = 0, UIMin = 0), Category = "RollWeightedOption")
	int32 Weight = 1;
};

/**
 * Executes a weighted roll
 */
UCLASS(NotBlueprintable, meta = (DisplayName = "Roll Weighted", Keywords = "random"))
class AIFLOW_API UAIFlowNode_ExecutionRollWeighted final : public UAIFlowNode
{
	GENERATED_UCLASS_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "RollWeighted", meta = (TitleProperty = "{OutputName} {Weight}"))
	TArray<FAIFlowNode_RollWeightedOption> OutputPinOptions;

	// Random stream to use for all random numbers
	UPROPERTY(Transient)
	FRandomStream RandomStream;
	
	UPROPERTY(Transient)
	int32 TotalWeight = 0;

public:
	virtual void OnActivate() override;
	virtual void ExecuteInput(const FName& PinName) override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;

	// IFlowContextPinSupplierInterface
	virtual bool SupportsContextPins() const override { return true; }
	virtual TArray<FFlowPin> GetContextOutputs() const override;
	// --
#endif

private:
	int32 GetWeightedRandomChoice();
	void CalculateTotalWeight();
};
