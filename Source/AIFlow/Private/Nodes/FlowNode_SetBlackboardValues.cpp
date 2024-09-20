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

	const TArray<UBlackboardComponent*> BlackboardComponents = GetBlackboardComponentsToApplyTo();
	if (!BlackboardComponents.IsEmpty())
	{	for (UBlackboardComponent* BlackboardComponent : BlackboardComponents)
		{
			if (IsValid(BlackboardComponent))
			{
				ActorBlackboardHelper.ApplyBlackboardOptionsToBlackboardComponent(
					*BlackboardComponent,
					PerActorOptionsAssignmentMethod,
					EntriesForEveryActor,
					PerActorOptions);
			}
		}
	}
	else
	{
		LogError(TEXT("Cannot SetBlackboardValues without a Blackboard"));
	}

	constexpr bool bIsFinished = true;
	TriggerFirstOutput(bIsFinished);
}

TArray<UBlackboardComponent*> UFlowNode_SetBlackboardValues::GetBlackboardComponentsToApplyTo() const
{
	TArray<UBlackboardComponent*> BlackboardComponents;
	BlackboardComponents.Reserve(1);
	BlackboardComponents.Add(GetBlackboardComponent());

	return BlackboardComponents;
}

void UFlowNode_SetBlackboardValues::UpdateNodeConfigText_Implementation()
{
#if WITH_EDITOR
	FTextBuilder TextBuilder;

	FAIFlowActorBlackboardHelper::AppendBlackboardOptions(EntriesForEveryActor, PerActorOptions, TextBuilder);

	SetNodeConfigText(TextBuilder.ToText());
#endif // WITH_EDITOR
}