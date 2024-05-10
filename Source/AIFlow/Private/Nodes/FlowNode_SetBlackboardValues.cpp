// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#include "Nodes/FlowNode_SetBlackboardValues.h"
#include "Blackboard/FlowBlackboardEntryValue.h"
#include "BehaviorTree/BlackboardComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlowNode_SetBlackboardValues)

UFlowNode_SetBlackboardValues::UFlowNode_SetBlackboardValues()
	: Super()
{
#if WITH_EDITOR
	NodeStyle = EFlowNodeStyle::Default;
	Category = TEXT("Blackboard");
#endif
}

void UFlowNode_SetBlackboardValues::ExecuteInput(const FName& PinName)
{
	Super::ExecuteInput(PinName);

	UBlackboardComponent* BlackboardComponent = GetBlackboardComponent();
	if (IsValid(BlackboardComponent))
	{
		for (const UFlowBlackboardEntryValue* Entry : Entries)
		{
			if (!IsValid(Entry))
			{
				continue;
			}

			Entry->SetOnBlackboardComponent(BlackboardComponent);
		}
	}
	else
	{
		LogError(TEXT("Cannot SetBlackboardValues without a Blackboard"));
	}

	constexpr bool bIsFinished = true;
	TriggerFirstOutput(bIsFinished);
}

void UFlowNode_SetBlackboardValues::UpdateNodeConfigText_Implementation()
{
#if WITH_EDITOR

	FTextBuilder TextBuilder;
	for (const UFlowBlackboardEntryValue* Entry : Entries)
	{
		if (!IsValid(Entry))
		{
			continue;
		}

		TextBuilder.AppendLine(Entry->BuildNodeConfigText());
	}

	SetNodeConfigText(TextBuilder.ToText());
#endif // WITH_EDITOR
}