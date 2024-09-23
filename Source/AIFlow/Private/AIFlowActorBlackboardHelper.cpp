// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#include "AIFlowActorBlackboardHelper.h"
#include "AIFlowAsset.h"
#include "AIFlowLogChannels.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Blackboard/FlowBlackboardEntryValue.h"
#include "GameFramework/Controller.h"
#include "GameFramework/Pawn.h"
#include "Types/FlowArray.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AIFlowActorBlackboardHelper)

UBlackboardComponent* FAIFlowActorBlackboardHelper::FindOrAddBlackboardComponentOnActor(
	AActor& Actor,
	UBlackboardData* OptionalBlackboardData,
	EActorBlackboardSearchRule SearchRule,
	EActorBlackboardInjectRule InjectRule)
{
	static_assert(static_cast<__underlying_type(EActorBlackboardSearchRule)>(EActorBlackboardSearchRule::Max) == 3, "This code may need updating if the enum values change");
	const bool bCanSearchActor =
		SearchRule == EActorBlackboardSearchRule::ActorAndController ||
		SearchRule == EActorBlackboardSearchRule::ActorOnly;

	const bool bCanSearchAIController =
		SearchRule == EActorBlackboardSearchRule::ActorAndController ||
		SearchRule == EActorBlackboardSearchRule::ControllerOnly;

	UBlackboardComponent* FoundBlackboardComponent = nullptr;

	if (bCanSearchActor)
	{
		FoundBlackboardComponent = UAIFlowAsset::TryFindBlackboardComponentOnActor(Actor, OptionalBlackboardData);
	}

	AController* Controller = nullptr;

	if (bCanSearchAIController && !FoundBlackboardComponent)
	{
		if (APawn* Pawn = Cast<APawn>(&Actor))
		{
			Controller = Pawn->GetController();
			if (IsValid(Controller))
			{
				FoundBlackboardComponent = UAIFlowAsset::TryFindBlackboardComponentOnActor(Actor, OptionalBlackboardData);
			}
		}
	}

	if (!FoundBlackboardComponent)
	{
		static_assert(static_cast<__underlying_type(EActorBlackboardInjectRule)>(EActorBlackboardInjectRule::Max) == 3, "This code may need updating if the enum values change");
		switch (InjectRule)
		{
		case EActorBlackboardInjectRule::InjectOntoActorIfMissing:
			{
				if (!IsValid(OptionalBlackboardData))
				{
					UE_LOG(LogAIFlow, Error, TEXT("Must specify OptionalBlackboardData if injecting missing a blackboard.  Without it, we don't know what blackboard data to use for the injected component!"));

					break;
				}

				// TODO (gtaylor) Not yet implemented (this enum value should not be selectable yet).
				checkNoEntry();
			}
			break;

		case EActorBlackboardInjectRule::InjectOntoControllerIfMissing:
			{
				if (!IsValid(OptionalBlackboardData))
				{
					UE_LOG(LogAIFlow, Error, TEXT("Must specify OptionalBlackboardData if injecting missing a blackboard.  Without it, we don't know what blackboard data to use for the injected component!"));

					break;
				}

				// TODO (gtaylor) Not yet implemented (this enum value should not be selectable yet).
				checkNoEntry();
			}
			break;

		case EActorBlackboardInjectRule::DoNotInjectIfMissing:
			break;

		default: break;
		}
	}

	return FoundBlackboardComponent;
}

void FAIFlowActorBlackboardHelper::ApplyBlackboardEntries(UBlackboardComponent& BlackboardComponent, const TArray<UFlowBlackboardEntryValue*>& EntriesToApply)
{
	for (const UFlowBlackboardEntryValue* Entry : EntriesToApply)
	{
		if (IsValid(Entry))
		{
			Entry->SetOnBlackboardComponent(&BlackboardComponent);
		}
	}
}

void FAIFlowActorBlackboardHelper::ApplyBlackboardOptionsToBlackboardComponent(
	UBlackboardComponent& BlackboardComponent,
	EPerActorOptionsAssignmentMethod ApplicationMethod,
	const FAIFlowConfigureBlackboardOption& EntriesForEveryActor,
	const TArray<FAIFlowConfigureBlackboardOption>& PerActorOptions)
{
	if (!EntriesForEveryActor.Entries.IsEmpty())
	{
		ApplyBlackboardEntries(BlackboardComponent, EntriesForEveryActor.Entries);
	}

	if (!PerActorOptions.IsEmpty())
	{
		const int32 PerActorOptionIndex = ChooseNextBlackboardOptionIndex(ApplicationMethod, PerActorOptions);

		if (PerActorOptionIndex != INDEX_NONE)
		{
			const FAIFlowConfigureBlackboardOption& Option = PerActorOptions[PerActorOptionIndex];

			ApplyBlackboardEntries(BlackboardComponent, Option.Entries);
		}
	}
}

TArray<UBlackboardComponent*> FAIFlowActorBlackboardHelper::FindOrAddBlackboardComponentOnActors(
	const TArray<AActor*>& Actors,
	UBlackboardData* OptionalBlackboardData,
	EActorBlackboardSearchRule SearchRule,
	EActorBlackboardInjectRule InjectRule)
{
	TArray<UBlackboardComponent*> BlackboardComponents;

	BlackboardComponents.Reserve(Actors.Num());

	for (AActor* Actor : Actors)
	{
		if (!IsValid(Actor))
		{
			UE_LOG(LogAIFlow, Error, TEXT("Null Actor supplied to FindOrAddBlackboardComponentOnActors.  Filter out the nulls before calling this function."));

			continue;
		}

		UBlackboardComponent* FoundBlackboardComponent =
			FindOrAddBlackboardComponentOnActor(
				*Actor,
				OptionalBlackboardData,
				SearchRule,
				InjectRule);

		if (FoundBlackboardComponent)
		{
			BlackboardComponents.Add(FoundBlackboardComponent);
		}
	}

	return BlackboardComponents;
}

int32 FAIFlowActorBlackboardHelper::ChooseNextBlackboardOptionIndex(
	EPerActorOptionsAssignmentMethod ApplicationMethod, 
	const TArray<FAIFlowConfigureBlackboardOption>& PerActorOptions)
{
	static_assert(static_cast<uint8>(EPerActorOptionsAssignmentMethod::Max) == 3, "update this code when changing enum");

	if (PerActorOptions.IsEmpty())
	{
		return INDEX_NONE;
	}

	EnsureOrderedOptionIndices(PerActorOptions.Num());

	const int32 OrderedOptionIndicesLast = OrderedOptionIndices.Num() - 1;

	if (OrderedOptionIndex < 0 || OrderedOptionIndex >= OrderedOptionIndicesLast)
	{
		switch (ApplicationMethod)
		{
		case EPerActorOptionsAssignmentMethod::ShuffledWithWrapping:
			{
				if (OrderedOptionIndex == INDEX_NONE)
				{
					FRandomStream RandomStream(FMath::RandHelper(INT_MAX));
					FlowArray::ShuffleArray(OrderedOptionIndices, RandomStream);
				}
			}
			break;

		case EPerActorOptionsAssignmentMethod::ShuffledWithReshuffling:
			{
				FRandomStream RandomStream(FMath::RandHelper(INT_MAX));
				FlowArray::ShuffleArray(OrderedOptionIndices, RandomStream);
			}
			break;

		case EPerActorOptionsAssignmentMethod::InOrderWithWrapping:
			break;

		default: break;
		}

		OrderedOptionIndex = 0;
	}
	else
	{
		++OrderedOptionIndex;
	}

	// Choose the index in the PerActorOptions array from the current entry the OrderedOptionIndices array.
	const int32 NextOptionIndex = OrderedOptionIndices[OrderedOptionIndex];

	return NextOptionIndex;
}

void FAIFlowActorBlackboardHelper::EnsureOrderedOptionIndices(int32 OptionNum)
{
	if (OrderedOptionIndices.IsEmpty())
	{
		OrderedOptionIndices.Reserve(OptionNum);

		for (int32 Index = 0; Index < OptionNum; ++Index)
		{
			OrderedOptionIndices.Add(Index);
		}

		OrderedOptionIndex = INDEX_NONE;
	}
}

#if WITH_EDITOR
void FAIFlowActorBlackboardHelper::AppendBlackboardOptions(
	const TArray<FAIFlowConfigureBlackboardOption> PerActorOptions,
	FTextBuilder& InOutTextBuilder)
{
	for (int32 Index = 0; Index < PerActorOptions.Num(); ++Index)
	{
		const FAIFlowConfigureBlackboardOption& Option = PerActorOptions[Index];
		InOutTextBuilder.AppendLine(FString::Printf(TEXT("Config Option #%d:"), Index));

		for (const UFlowBlackboardEntryValue* PerActorEntry : Option.Entries)
		{
			if (IsValid(PerActorEntry))
			{
				InOutTextBuilder.AppendLine(FString(TEXT("  ")) + PerActorEntry->BuildNodeConfigText().ToString());
			}
		}
	}
}
#endif // WITH_EDITOR
