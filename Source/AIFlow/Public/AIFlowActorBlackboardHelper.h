// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#pragma once

#include "Templates/SubclassOf.h"
#include "Types/FlowEnumUtils.h"
#include "StructUtils/InstancedStruct.h"
#include "Types/FlowPinEnums.h"

#include "AIFlowActorBlackboardHelper.generated.h"

// Forward Declarations
class AActor;
class FTextBuilder;
class UBlackboardComponent;
class UBlackboardData;
class UFlowBlackboardEntryValue;
class UFlowNodeBase;
class UFlowInjectComponentsManager;
struct FFlowBlackboardEntry;
struct FFlowDataPinValue;
class UBlackboardKeyType;

// Rule enum for injecting missing blackboards on Actors
UENUM()
enum class EActorBlackboardInjectRule : uint8
{
	InjectOntoActorIfMissing,
	InjectOntoControllerIfMissing,
	DoNotInjectIfMissing,

	Max UMETA(Hidden),
	Invalid UMETA(Hidden),
	Min = 0 UMETA(Hidden),

	NeedsInjectComponentsManagerFirst = InjectOntoActorIfMissing UMETA(Hidden),
	NeedsInjectComponentsManagerLast = InjectOntoControllerIfMissing UMETA(Hidden),
};
FLOW_ENUM_RANGE_VALUES(EActorBlackboardInjectRule);

namespace EActorBlackboardInjectRule_Classifiers
{
	FORCEINLINE bool NeedsInjectComponentsManager(EActorBlackboardInjectRule Rule) { return FLOW_IS_ENUM_IN_SUBRANGE(Rule, EActorBlackboardInjectRule::NeedsInjectComponentsManager); }
}

// Rule enum for searching for Actor blackboards
UENUM()
enum class EActorBlackboardSearchRule : uint8
{
	// Search the Actor only for the BlackboardComponent
	ActorOnly UMETA(DisplayName = "Actor Only"),

	// Search the Actor's Controller only for the BlackboardComponent
	ControllerOnly UMETA(DisplayName = "Controller Only"),

	// Search the GameState actor for the BlackboardComponent
	GameStateOnly UMETA(DisplayName = "GameState Only"),

	// Search both the Actor and the Controller for the BlackboardComponent
	ActorAndController UMETA(DisplayName = "Actor & Controller"),

	// Search the Actor, its Controller and the GameState actor for the BlackboardComponent
	ActorAndControllerAndGameState UMETA(DisplayName = "Actor, Controller & GameState"),

	Max UMETA(Hidden),
	Invalid UMETA(Hidden),
	Min = 0 UMETA(Hidden),
};
FLOW_ENUM_RANGE_VALUES(EActorBlackboardSearchRule);

namespace EActorBlackboardSearchRule_Classifiers
{
	FORCEINLINE bool CanSearchActor(EActorBlackboardSearchRule Rule) { return Rule == EActorBlackboardSearchRule::ActorOnly || Rule == EActorBlackboardSearchRule::ActorAndController || Rule == EActorBlackboardSearchRule::ActorAndControllerAndGameState; }
	FORCEINLINE bool CanSearchController(EActorBlackboardSearchRule Rule) { return Rule == EActorBlackboardSearchRule::ControllerOnly || Rule == EActorBlackboardSearchRule::ActorAndController || Rule == EActorBlackboardSearchRule::ActorAndControllerAndGameState; }
	FORCEINLINE bool CanSearchGameState(EActorBlackboardSearchRule Rule) { return Rule == EActorBlackboardSearchRule::GameStateOnly || Rule == EActorBlackboardSearchRule::ActorAndControllerAndGameState; }
}

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
FLOW_ENUM_RANGE_VALUES(EPerActorOptionsAssignmentMethod);

// A bundle of Blackboard Entries to set on an actor(s)
USTRUCT(BlueprintType)
struct FAIFlowConfigureBlackboardOption
{
	GENERATED_BODY()

public:

	// Entries to set on the blackboard
	UPROPERTY(EditAnywhere, Instanced, Category = BlackboardEntriesToSet, DisplayName = "Blackboard Entries")
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
		const TArray<FAIFlowConfigureBlackboardOption>* PerActorOptions);

	// Find or add (if the InjectRule allows) the desired BlackboardComponent on Actors.
	// If no OptionalBlackboardData is specified, it uses the first blackboard component that can be found,
	// otherwise, it restricts the result to a blackboard component that uses the blackboard data specified.
	static TArray<UBlackboardComponent*> FindOrAddBlackboardComponentOnActors(
		const TArray<AActor*>& Actors,
		UFlowInjectComponentsManager* InjectComponentsManager,
		TSubclassOf<UBlackboardComponent> BlackboardComponentClass,
		UBlackboardData* OptionalBlackboardData,
		EActorBlackboardSearchRule SearchRule,
		EActorBlackboardInjectRule InjectRule);

	// Find or add (if the InjectRule allows) the desired BlackboardComponent on an Actor.
	// If no OptionalBlackboardData is specified, it uses the first blackboard component that can be found,
	// otherwise, it restricts the result to a blackboard component that uses the blackboard data specified.
	static UBlackboardComponent* FindOrAddBlackboardComponentOnActor(
		AActor& Actor,
		UFlowInjectComponentsManager* InjectComponentsManager,
		TSubclassOf<UBlackboardComponent> BlackboardComponentClass,
		UBlackboardData* OptionalBlackboardData,
		EActorBlackboardSearchRule SearchRule,
		EActorBlackboardInjectRule InjectRule);

	// Try to find the blackboard on either the Actor, their Controller or the GameState, as directed by the supplied parameters
	static UBlackboardComponent* TryFindBlackboardComponent(UWorld& World, EActorBlackboardSearchRule SearchRule, AActor* OptionalActor, UBlackboardData* OptionalBlackboardData);

	AIFLOW_API static EFlowDataPinResolveResult TryProvideFlowDataPinPropertyFromBlackboardEntry(
		const FName& BlackboardKeyName,
		const UBlackboardKeyType* BlackboardKeyType,
		UBlackboardComponent* OptionalBlackboardComponent,
		TInstancedStruct<FFlowDataPinValue>& OutFlowDataPinProperty);

#if WITH_EDITOR
	// Helper function to append text for Flow Node/AddOn Configuration display
	AIFLOW_API static void AppendBlackboardOptions(
		const TArray<FAIFlowConfigureBlackboardOption>& PerActorOptions,
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

// Helper struct to cache the blackboard component and runtime data reference
USTRUCT()
struct FAIFlowCachedBlackboardReference
{
	GENERATED_BODY()

public:

	AIFLOW_API FAIFlowCachedBlackboardReference() = default;
	AIFLOW_API FAIFlowCachedBlackboardReference(const UFlowNodeBase& FlowNodeBase, UBlackboardData* OptionalSpecificBlackboardData = nullptr, EActorBlackboardSearchRule SpecificBlackboardSearchRule = EActorBlackboardSearchRule::ActorAndControllerAndGameState)
		{ (void) TryCacheBlackboardReference(FlowNodeBase, OptionalSpecificBlackboardData, SpecificBlackboardSearchRule); }

	AIFLOW_API bool TryCacheBlackboardReference(const UFlowNodeBase& FlowNodeBase, UBlackboardData* OptionalSpecificBlackboardData = nullptr, EActorBlackboardSearchRule SpecificBlackboardSearchRule = EActorBlackboardSearchRule::ActorAndControllerAndGameState);
	AIFLOW_API bool IsValid() const;

public:

	UPROPERTY(Transient)
	TObjectPtr<UBlackboardComponent> BlackboardComponent = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UBlackboardData> BlackboardData = nullptr;
};
