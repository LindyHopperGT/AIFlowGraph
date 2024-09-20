// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#pragma once

#include "AIFlowActorBlackboardHelper.generated.h"

// Forward Declarations
class AActor;
class FTextBuilder;
class UBlackboardComponent;
class UBlackboardData;
class UFlowBlackboardEntryValue;

// Rule enum for injecting missing blackboards on Actors
UENUM()
enum class EActorBlackboardInjectRule : uint8
{
	// TODO (gtaylor) Add support for injecting the missing blackboard
	InjectOntoActorIfMissing UMETA(Hidden),

	// TODO (gtaylor) Add support for injecting the missing blackboard
	InjectOntoControllerIfMissing UMETA(Hidden),

	DoNotInjectIfMissing,

	Max UMETA(Hidden),
	Invalid UMETA(Hidden),
	Min = 0 UMETA(Hidden),
};

// Rule enum for searching for Actor blackboards
UENUM()
enum class EActorBlackboardSearchRule : uint8
{
	// Search the Actor only for the BlackboardComponent
	ActorOnly UMETA(DisplayName = "Actor Only"),

	// Search the Actor's Controller only for the BlackboardComponent
	ControllerOnly UMETA(DisplayName = "Controller Only"),

	// Search both the Actor and the Controller for the BlackboardComponent
	ActorAndController UMETA(DisplayName = "Actor & Controller"),

	Max UMETA(Hidden),
	Invalid UMETA(Hidden),
	Min = 0 UMETA(Hidden),
};

// Method to apply Per-Actor Options to Actors
UENUM(BlueprintType)
enum class EPerActorOptionsAssignmentMethod : uint8
{
	// Options applied to actors in-order, 
	// wrapping if the PerActorOptions are insufficient for the number of actors
	InOrderWithWrapping,

	// Options are shuffled and then applied to actors, 
	// wrapping if the PerActorOptions are insufficient for the number of actors
	ShuffledWithWrapping,

	// Options are shuffled and then applied to actors, 
	// reshuffling if the PerActorOptions are insufficient for the number of actors
	ShuffledWithReshuffling,

	Max UMETA(Hidden),
	Invalid UMETA(Hidden),
	Min = 0 UMETA(Hidden),
};

// A bundle of Blackboard Entries to set on an actor(s)
USTRUCT(BlueprintType)
struct FAIFlowConfigureBlackboardOption
{
	GENERATED_BODY()

public:

	// Entries to set on the blackboard
	UPROPERTY(EditAnywhere, Instanced, Category = Configuration)
	TArray<UFlowBlackboardEntryValue*> Entries;
};

// Helper struct to handle the shared functionality of setting blackboard values for actors.
// Used by UFlowNodeAddOn_ConfigureSpawnedActorBlackboard and UFlowNode_SetBlackboardValues.
USTRUCT()
struct FAIFlowActorBlackboardHelper
{
	GENERATED_BODY()

public:

	// Apply groups of blackboard entry options to a blackboard component.
	// Handles the incrementing and other management of AssignmentMethod state data.
	void ApplyBlackboardOptionsToBlackboardComponent(
		UBlackboardComponent& BlackboardComponent,
		EPerActorOptionsAssignmentMethod AssignmentMethod,
		const FAIFlowConfigureBlackboardOption& EntriesForEveryActor,
		const TArray<FAIFlowConfigureBlackboardOption>& PerActorOptions);

	// Find or add (if the InjectRule allows) the desired BlackboardComponent on Actors.
	// If no OptionalBlackboardData is specified, it uses the first blackboard component that can be found,
	// otherwise, it restricts the result to a blackboard component that uses the blackboard data specified.
	static TArray<UBlackboardComponent*> FindOrAddBlackboardComponentOnActors(
		const TArray<AActor*>& Actors,
		UBlackboardData* OptionalBlackboardData,
		EActorBlackboardSearchRule SearchRule,
		EActorBlackboardInjectRule InjectRule);

	// Find or add (if the InjectRule allows) the desired BlackboardComponent on an Actor.
	// If no OptionalBlackboardData is specified, it uses the first blackboard component that can be found,
	// otherwise, it restricts the result to a blackboard component that uses the blackboard data specified.
	static UBlackboardComponent* FindOrAddBlackboardComponentOnActor(
		AActor& Actor,
		UBlackboardData* OptionalBlackboardData,
		EActorBlackboardSearchRule SearchRule,
		EActorBlackboardInjectRule InjectRule);

#if WITH_EDITOR
	// Helper function to append text for Flow Node/AddOn Configuration display
	AIFLOW_API static void AppendBlackboardOptions(
		const FAIFlowConfigureBlackboardOption& EntriesForEveryActor,
		const TArray<FAIFlowConfigureBlackboardOption> PerActorOptions,
		FTextBuilder& InOutTextBuilder);
#endif // WITH_EDITOR

protected:

	// Apply the Blackboard Entries's value changes to the specified blackboard
	static void ApplyBlackboardEntries(UBlackboardComponent& BlackboardComponent, const TArray<UFlowBlackboardEntryValue*>& EntriesToApply);

	// Helper function to setup and maintain the OrderedOptionIndices and OrderedOptionIndex according to the AssignmentMethod.
	int32 ChooseNextBlackboardOptionIndex(
		EPerActorOptionsAssignmentMethod AssignmentMethod,
		const TArray<FAIFlowConfigureBlackboardOption>& InOutPerActorOptions);

	// Builds the OrderedOptionIndices array (if empty)
	void EnsureOrderedOptionIndices(int32 OptionNum);

protected:

	// Most recently used Index in the OrderedOptionIndices array.
	UPROPERTY(Transient)
	int32 OrderedOptionIndex = INDEX_NONE;

	// Array of indices into the PerActorOptions.
	// May be in-order, or shuffled, based on the AssignmentMethod used to generate the array.
	UPROPERTY(Transient)
	TArray<int32> OrderedOptionIndices;
};
