// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#include "Nodes/AIFlowNode_ExecutionRollGuaranteed.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AIFlowNode_ExecutionRollGuaranteed)

const FName UAIFlowNode_ExecutionRollGuaranteed::INPIN_Reset(TEXT("Reset"));

const FName UAIFlowNode_ExecutionRollGuaranteed::OUTPIN_GuaranteedOut(TEXT("Guaranteed Out"));
const FName UAIFlowNode_ExecutionRollGuaranteed::OUTPIN_FailureOut(TEXT("Failure Out"));

UAIFlowNode_ExecutionRollGuaranteed::UAIFlowNode_ExecutionRollGuaranteed(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
#if WITH_EDITOR
	NodeDisplayStyle = FlowNodeStyle::Default;
	Category = TEXT("Flow|Routing");
#endif

	FString ResetPinTooltip = TEXT("Finish work of this node.");
	ResetPinTooltip += LINE_TERMINATOR;
	ResetPinTooltip += TEXT("Calling In input will start triggering output pins once again.");

	InputPins.Add(FFlowPin(INPIN_Reset.ToString(), ResetPinTooltip));
	
	FString GuaranteedPinTooltip = TEXT("Roll for a chance to enter this rout sequentially increasing our chances.");
	FString FailurePinTooltip = TEXT("On failure we take this rout");
	OutputPins.Empty();
	OutputPins.Add(FFlowPin(OUTPIN_GuaranteedOut.ToString(), GuaranteedPinTooltip));
	OutputPins.Add(FFlowPin(OUTPIN_FailureOut.ToString(), FailurePinTooltip));
	
	AllowedSignalModes = {EFlowSignalMode::Enabled, EFlowSignalMode::Disabled};
}

void UAIFlowNode_ExecutionRollGuaranteed::ExecuteInput(const FName& PinName)
{
	if (PinName == DefaultInputPin.PinName)
	{
		++RollAttempts;
		ensure(RollAttempts <= MaximumAttempts);

		const bool bResetOnMaxAttempts = RollAttempts == MaximumAttempts;
		
		if(bHasSuccessfullyRolled)
		{
			TriggerOutput(OUTPIN_FailureOut, bResetOnMaxAttempts);
		}
		else
		{
			const int32 Random = FMath::RandRange(0, MaximumAttempts - RollAttempts);

			if(Random == 0)
			{
				bHasSuccessfullyRolled = true; 
				TriggerOutput(OUTPIN_GuaranteedOut, bResetOnSuccess || bResetOnMaxAttempts);
			}
			else
			{
				TriggerOutput(OUTPIN_FailureOut, false);
			}
		}
	}
	else if (PinName == INPIN_Reset)
	{
		Finish();
	}
}

void UAIFlowNode_ExecutionRollGuaranteed::Cleanup()
{
	RollAttempts = 0;
	bHasSuccessfullyRolled = false;
}

#if WITH_EDITOR
FString UAIFlowNode_ExecutionRollGuaranteed::GetNodeDescription() const
{
	FString Result;
	Result.Reserve(128);

	if (bResetOnSuccess)
	{
		Result.Append(TEXT("Reset On Success"));
	}

	return Result;
}
#endif
