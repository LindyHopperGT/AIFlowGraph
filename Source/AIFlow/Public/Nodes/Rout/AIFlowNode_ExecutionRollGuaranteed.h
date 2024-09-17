// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#pragma once

#include "Nodes/FlowNode.h"
#include "AIFlowNode_ExecutionRollGuaranteed.generated.h"

/**
 * Executes a random roll that is eventually guaranteed
 * Our odds of the Guaranteed Output increase by 1 / MaximumAttempts every failed attempt
 * MaximumAttempts(4) - odds for success would be 25%, 50%, 75%, 100% on each subsequent roll
 */
UCLASS(NotBlueprintable, meta = (DisplayName = "Roll Guaranteed", Keywords = "random"))
class AIFLOW_API UAIFlowNode_ExecutionRollGuaranteed final : public UFlowNode
{
	GENERATED_UCLASS_BODY()

	// Initial odds of our "guaranteed output" rout. ( 1 / MaximumAttempts )
	UPROPERTY(EditAnywhere, Category = "RollGuaranteed", meta = (ClampMin = 2, UIMin = 2))
	int32 MaximumAttempts = 2;

	// After a successful output should we reset? Otherwise we fail until we make MaximumAttempts.
	UPROPERTY(EditAnywhere, Category = "RollGuaranteed")
	bool bResetOnSuccess = false;

private:
	UPROPERTY(SaveGame)
	int32 RollAttempts = 0;
		
	UPROPERTY(SaveGame)
	bool bHasSuccessfullyRolled = false;

public:
#if WITH_EDITOR
	virtual bool CanUserAddOutput() const override { return false; }
#endif

protected:
	virtual void ExecuteInput(const FName& PinName) override;
	virtual void Cleanup() override;

#if WITH_EDITOR
	virtual FString GetNodeDescription() const override;
#endif
	
	static const FName INPIN_Reset;
	static const FName OUTPIN_GuaranteedOut;
	static const FName OUTPIN_FailureOut;
};
