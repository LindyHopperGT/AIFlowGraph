// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#pragma once

#include "AddOns/AIFlowNodeAddOn.h"
#include "AIFlowActorBlackboardHelper.h"
#include "Interfaces/FlowPredicateInterface.h"
#include "Types/FlowBlackboardEntry.h"

#include "GameplayTagContainer.h"
#include "BehaviorTree/Blackboard/BlackboardKey.h"
#include "BehaviorTree/Blackboard/BlackboardKeyEnums.h"
#include "Templates/SubclassOf.h"

#include "FlowNodeAddOn_PredicateCompareBlackboardValue.generated.h"

// TODO (gtaylor) Add specific actor sourcing options (data pins?) for comparing from other actors' blackboards

// Forward Declarations
class UFlowBlackboardEntryValue;
class UBlackboardKeyType;
struct FAIFlowCachedBlackboardReference;
struct FBlackboardEntry;

// Operator for UFlowNodeAddOn_PredicateCompareBlackboardValue's compare operation
// TODO jserrallonga 2024-11-21: Refactor this operator type enum to exist somewhere else. 
UENUM(BlueprintType)
enum class EPredicateCompareOperatorType : uint8
{
	// Supported by all UBlackboardKeyItem subclasses

	Equal			UMETA(DisplayName = "Is Equal To"),
	NotEqual		UMETA(DisplayName = "Is Not Equal To"),

	// Supported by UBlackboardKeyItem _Int, _Float and _Enum subclasses only

	Less			UMETA(DisplayName = "Is Less Than"),
	LessOrEqual		UMETA(DisplayName = "Is Less Than Or Equal To"),
	Greater			UMETA(DisplayName = "Is Greater Than"),
	GreaterOrEqual	UMETA(DisplayName = "Is Greater Than Or Equal To"),

	Max				UMETA(Hidden),
	Min = 0			UMETA(Hidden),

	// Subrange for equality operations
	EqualityFirst = Equal UMETA(Hidden),
	EqualityLast = NotEqual UMETA(Hidden),

	// Subrange for Arithmetic-only operations
	ArithmeticFirst = Less UMETA(Hidden),
	ArithmeticLast = GreaterOrEqual UMETA(Hidden),
};

FORCEINLINE_DEBUGGABLE FString GetOperatorSymbolString(const EPredicateCompareOperatorType OperatorType)
{
	static_assert(static_cast<int32>(EPredicateCompareOperatorType::Max) == 6, TEXT("This should be kept up to date with the enum"));
	switch(OperatorType)
	{
	case EPredicateCompareOperatorType::Equal:
		return TEXT("==");
	case EPredicateCompareOperatorType::NotEqual:
		return TEXT("!=");
	case EPredicateCompareOperatorType::Less:
		return TEXT("<");
	case EPredicateCompareOperatorType::LessOrEqual:
		return TEXT("<=");
	case EPredicateCompareOperatorType::Greater:
		return TEXT(">");
	case EPredicateCompareOperatorType::GreaterOrEqual:
		return TEXT(">=");
	default:
		return TEXT("[Invalid Operator]");
	}
}


UCLASS(MinimalApi, NotBlueprintable, meta = (DisplayName = "Compare Blackboard Value"))
class UFlowNodeAddOn_PredicateCompareBlackboardValue
	: public UAIFlowNodeAddOn
	, public IFlowPredicateInterface
{
	GENERATED_BODY()

public:

	UFlowNodeAddOn_PredicateCompareBlackboardValue();

	// IFlowPredicateInterface
	virtual bool EvaluatePredicate_Implementation() const override;
	// --

#if WITH_EDITOR
	// UObject
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	// --

	// UFlowNodeBase
	virtual FText GetNodeTitle() const override;
	// --

	// IFlowBlackboardAssetProvider
	virtual UBlackboardData* GetBlackboardAssetForPropertyHandle(const TSharedPtr<IPropertyHandle>& PropertyHandle) const override;
	// --
#endif // WITH_EDITOR

protected:
#if WITH_EDITOR
	bool TryRefreshIsKeyLeftSelected();
	bool TryRefreshKeyEntriesAndExplicitValues(const UBlackboardData& BlackboardData);

	bool TryRefreshSelectedKeyType(const UBlackboardData& BlackboardData, bool bEnableKeyEntry, FFlowBlackboardEntry& InOutKeyProperty);
	bool TryRefreshExplicitValue(const UBlackboardData& BlackboardData, bool bEnableExplicitValue, TObjectPtr<UFlowBlackboardEntryValue>& InOutExplicitValue);

	TSubclassOf<UBlackboardKeyType> TryGetKeyTypeClass(const UBlackboardData& BlackboardData) const;
	UBlackboardKeyType const* TryGetKeyType(const UBlackboardData& BlackboardData) const;

	UBlackboardData* GetBlackboardAssetForEditor() const;
#endif // WITH_EDITOR

	bool TryGetBlackboardKeyInfo(
		const UBlackboardData& BlackboardData,
		const FFlowBlackboardEntry& KeyEntry,
		FBlackboard::FKey& OutKeyID,
		FBlackboardEntry const*& OutKeyTypeEntry,
		bool bWarnIfMissing) const;

	bool ComputeCompareResultWithExplicitValue(
		const UBlackboardComponent& BlackboardComponent,
		const TSubclassOf<UBlackboardKeyType> KeyType,
		const FBlackboard::FKey LeftKeyID,
		const UFlowBlackboardEntryValue& RightExplicitValue) const;
	bool ComputeCompareResult(
		const UBlackboardComponent& BlackboardComponent,
		const TSubclassOf<UBlackboardKeyType> KeyType,
		const FBlackboard::FKey LeftKeyID,
		const FBlackboard::FKey RightKeyID) const;

	FORCEINLINE static bool IsEqualityOperation(EPredicateCompareOperatorType Operation)
	{
		return
			Operation >= EPredicateCompareOperatorType::EqualityFirst &&
			Operation <= EPredicateCompareOperatorType::EqualityLast;
	}

	FORCEINLINE static bool IsArithmeticOperation(EPredicateCompareOperatorType Operation)
	{
		return
			Operation >= EPredicateCompareOperatorType::ArithmeticFirst &&
			Operation <= EPredicateCompareOperatorType::ArithmeticLast;
	}

	static EArithmeticKeyOperation::Type ConvertPredicateCompareOperatorTypeToArithmeticKeyOperation(EPredicateCompareOperatorType OperatorType);

protected:

	// Blackboard key for the Gameplay Tag or Tag Container to test with the Query
	UPROPERTY(EditAnywhere, Category = Configuration, DisplayName = "Key (left)")
	FFlowBlackboardEntry KeyLeft;

	// Note - &&-ing the bIsKeyLeftSelected bool  with itself in EditCondition is to prevent an implicit InlineEditConditionToggle 
	// from being introduced on these properties.  We don't want the embedded checkbox for bIsKeyLeftSelected, so I am making the 
	// EditCondition harmlessly more complicated so that UE doesn't imply that I want the InlineEditConditionToggle.

	UPROPERTY(EditAnywhere, Category = Configuration, DisplayName = "Operator", meta = (EditCondition = "bIsKeyLeftSelected && bIsKeyLeftSelected"))
	EPredicateCompareOperatorType OperatorType = EPredicateCompareOperatorType::Equal;

	// Search rule to use to find the "Specific Blackboard" (if specified)
	UPROPERTY(EditAnywhere, Category = Configuration, AdvancedDisplay, DisplayName = "Specific Blackboard Search Rule", meta = (EditCondition = "SpecificBlackboardAsset", DisplayAfter = SpecificBlackboardAsset))
	EActorBlackboardSearchRule SpecificBlackboardSearchRule = EActorBlackboardSearchRule::ActorAndControllerAndGameState;

	// Blackboard key for the Gameplay Tag or Tag Container to test with the Query
	UPROPERTY(EditAnywhere, Category = Configuration, DisplayName = "Key (right)", meta = (EditCondition = "!bUseExplicitValueForRightHandSide && bIsKeyLeftSelected == true"))
	FFlowBlackboardEntry KeyRight;

	UPROPERTY(EditAnywhere, Instanced, Category = Configuration, DisplayName = "Explicit Value (right)", meta = (EditCondition = "bUseExplicitValueForRightHandSide && bIsKeyLeftSelected"))
	TObjectPtr<UFlowBlackboardEntryValue> ExplicitValueRight = nullptr;

	// Specific blackboard to use for the comparison
	UPROPERTY(EditAnywhere, Category = Configuration, AdvancedDisplay, DisplayName = "Specific Blackboard")
	TObjectPtr<UBlackboardData> SpecificBlackboardAsset = nullptr;

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, Category = Configuration, meta = (EditCondition = "bIsKeyLeftSelected && bIsKeyLeftSelected"))
	bool bUseExplicitValueForRightHandSide = false;

	UPROPERTY()
	bool bIsKeyLeftSelected = false;
#endif // WITH_EDITORONLY_DATA
};
