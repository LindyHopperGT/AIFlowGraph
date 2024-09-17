// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#include "Nodes/Rout/AIFlowNode_ExecutionRollWeighted.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AIFlowNode_ExecutionRollWeighted)

FAIFlowNode_RollWeightedOption::FAIFlowNode_RollWeightedOption(const FName InOutputName, int32 InWeight) 
	: OutputName(InOutputName)
	, Weight(InWeight)
{
}

UAIFlowNode_ExecutionRollWeighted::UAIFlowNode_ExecutionRollWeighted(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
#if WITH_EDITOR
	Category = TEXT("Route");
	NodeStyle = EFlowNodeStyle::Logic;
#endif

	OutputPins.Empty();
	
	AllowedSignalModes = {EFlowSignalMode::Enabled, EFlowSignalMode::Disabled};
}
	
int32 UAIFlowNode_ExecutionRollWeighted::GetWeightedRandomChoice()
{
	CalculateTotalWeight();
	if (TotalWeight <= 0)
	{
		return INDEX_NONE;
	}
	
	const int32 ChosenWeight = RandomStream.RandHelper(TotalWeight);
	check(ChosenWeight <= TotalWeight);
	check(ChosenWeight >= 0);

	int32 CumulativeWeight = 0;

	for (int32 OptionIndex = 0; OptionIndex < OutputPinOptions.Num(); ++OptionIndex)
	{
		const FAIFlowNode_RollWeightedOption& Option = OutputPinOptions[OptionIndex];
		const int32 Weight =  Option.OutputName != NAME_None ? Option.Weight : 0;
		check(Weight >= 0);

		CumulativeWeight += Weight;

		if (ChosenWeight < CumulativeWeight)
		{
			return OptionIndex;
		}
	}

	const int32 FinalIndex = OutputPinOptions.Num() - 1;
	return FinalIndex;	
}

void UAIFlowNode_ExecutionRollWeighted::ExecuteInput(const FName& PinName)
{
	const int32 ChosenOptionIndex = GetWeightedRandomChoice();
	if(OutputPins.IsValidIndex(ChosenOptionIndex))
	{
		TriggerOutput(OutputPins[ChosenOptionIndex].PinName, true);
	}
}

void UAIFlowNode_ExecutionRollWeighted::CalculateTotalWeight()
{
	TotalWeight = 0;
	for (int32 OptionIndex = 0; OptionIndex < OutputPinOptions.Num(); ++OptionIndex)
	{
		const FAIFlowNode_RollWeightedOption& Option = OutputPinOptions[OptionIndex];
		const int32 Weight =  Option.OutputName != NAME_None ? Option.Weight : 0;
		check(Weight >= 0);

		TotalWeight += Weight;
	}
}

#if WITH_EDITOR
void UAIFlowNode_ExecutionRollWeighted::UpdateOutputPins()
{
	OutputPins.Empty();
	for(const FAIFlowNode_RollWeightedOption Output : OutputPinOptions)
	{
		if(Output.OutputName != NAME_None)
		{
			OutputPins.Add(FFlowPin(Output.OutputName.ToString()));
		}
	}
}

void UAIFlowNode_ExecutionRollWeighted::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	
	const FName MemberPropertyName = (PropertyChangedEvent.MemberProperty != nullptr) ? PropertyChangedEvent.MemberProperty->GetFName() : NAME_None;

	if (MemberPropertyName == GET_MEMBER_NAME_CHECKED(UAIFlowNode_ExecutionRollWeighted, OutputPinOptions))
	{
		UpdateOutputPins();
		OnReconstructionRequested.ExecuteIfBound();
	}
}
#endif
