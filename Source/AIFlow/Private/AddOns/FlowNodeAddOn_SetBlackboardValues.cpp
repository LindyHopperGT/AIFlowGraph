// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#include "AddOns/FlowNodeAddOn_SetBlackboardValues.h"
#include "Blackboard/FlowBlackboardEntryValue.h"
#include "AIFlowLogChannels.h"
#include "Nodes/AIFlowNode.h"
#include "Nodes/FlowNode_SetBlackboardValues.h"

#include "BehaviorTree/BlackboardComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlowNodeAddOn_SetBlackboardValues)

UFlowNodeAddOn_SetBlackboardValues::UFlowNodeAddOn_SetBlackboardValues()
	: Super()
{
#if WITH_EDITOR
	NodeStyle = EFlowNodeStyle::Default;
	Category = TEXT("Blackboard");
#endif
}

EFlowAddOnAcceptResult UFlowNodeAddOn_SetBlackboardValues::AcceptFlowNodeAddOnParent_Implementation(const UFlowNodeBase* ParentTemplate) const
{
#if WITH_EDITOR
	if (Super::AcceptFlowNodeAddOnParent_Implementation(ParentTemplate) == EFlowAddOnAcceptResult::Reject)
	{
		return EFlowAddOnAcceptResult::Reject;
	}

	// Allow for any AIFlowNode subclass by default (they may opt-out if they desire)
	//  except for UFlowNode_SetBlackboardValues, which already can do this anyway (and that would be weird)
	if (IsValid(ParentTemplate) &&
		ParentTemplate->IsA<UAIFlowNode>() && 
		!ParentTemplate->IsA<UFlowNode_SetBlackboardValues>())
	{
		return EFlowAddOnAcceptResult::TentativeAccept;
	}

#endif // WITH_EDITOR
	return EFlowAddOnAcceptResult::Undetermined;
}

void UFlowNodeAddOn_SetBlackboardValues::ExecuteInput(const FName& PinName)
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
}

void UFlowNodeAddOn_SetBlackboardValues::UpdateNodeConfigText_Implementation()
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