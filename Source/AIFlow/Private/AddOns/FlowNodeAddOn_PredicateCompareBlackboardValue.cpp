// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#include "AddOns/FlowNodeAddOn_PredicateCompareBlackboardValue.h"
#include "AIFlowActorBlackboardHelper.h"
#include "Blackboard/FlowBlackboardEntryValue.h"
#include "FlowAsset.h"
#include "FlowSettings.h"
#include "AIFlowLogChannels.h"

#include "Nodes/FlowNodeBase.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Enum.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Float.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Int.h"
#include "EdGraph/EdGraph.h"

#define LOCTEXT_NAMESPACE "FlowNodeAddOn_PredicateCompareBlackboardValue"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlowNodeAddOn_PredicateCompareBlackboardValue)

UFlowNodeAddOn_PredicateCompareBlackboardValue::UFlowNodeAddOn_PredicateCompareBlackboardValue()
	: Super()
{
#if WITH_EDITOR
	NodeDisplayStyle = FlowNodeStyle::AddOn_Predicate;
	Category = TEXT("Blackboard");
#endif
}

#if WITH_EDITOR

void UFlowNodeAddOn_PredicateCompareBlackboardValue::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (!PropertyChangedEvent.MemberProperty)
	{
		return;
	}

	(void) TryRefreshIsKeyLeftSelected();

	const UBlackboardData* BlackboardAssetForEditor = GetBlackboardAssetForEditor();

	const FName PropertyName = PropertyChangedEvent.Property->GetFName();
	const FName MemberPropertyName = PropertyChangedEvent.MemberProperty->GetFName();

	if (MemberPropertyName == GET_MEMBER_NAME_CHECKED(UFlowNodeAddOn_PredicateCompareBlackboardValue, KeyLeft) ||
		PropertyName == GET_MEMBER_NAME_CHECKED(UFlowNodeAddOn_PredicateCompareBlackboardValue, OperatorType))
	{
		if (IsValid(BlackboardAssetForEditor))
		{
			TSubclassOf<UBlackboardKeyType> KeyTypeClass = TryGetKeyTypeClass(*BlackboardAssetForEditor);
			TSubclassOf<UFlowBlackboardEntryValue> FlowEntrySubclass = UFlowBlackboardEntryValue::GetFlowBlackboardEntryValueClassForKeyType(KeyTypeClass);

			const bool bIsArithmeticOperation = IsArithmeticOperation(OperatorType);
			const bool bAllowArithmeticOperations = (FlowEntrySubclass && FlowEntrySubclass->GetDefaultObject<UFlowBlackboardEntryValue>()->SupportsArithmeticOperations());

			if (!bAllowArithmeticOperations && bIsArithmeticOperation)
			{
				// Reset OperatorType if the KeyType doesn't support Arithmetic operations
				OperatorType = EPredicateCompareOperatorType::EqualityFirst;
			}
		}
	}

	if (IsValid(BlackboardAssetForEditor) && TryRefreshKeyEntriesAndExplicitValues(*BlackboardAssetForEditor))
	{
		OnReconstructionRequested.ExecuteIfBound();
	}
}

bool UFlowNodeAddOn_PredicateCompareBlackboardValue::TryRefreshIsKeyLeftSelected()
{
	const bool bIsKeyLeftSelectedPrev = bIsKeyLeftSelected;
	bIsKeyLeftSelected = !KeyLeft.GetKeyName().IsNone();

	if (bIsKeyLeftSelectedPrev != bIsKeyLeftSelected)
	{
		return true;
	}

	return false;
}

bool UFlowNodeAddOn_PredicateCompareBlackboardValue::TryRefreshKeyEntriesAndExplicitValues(const UBlackboardData& BlackboardData)
{
	bool bMadeChanges = false;

	const bool bAllowRightKeyEntry = bIsKeyLeftSelected && !bUseExplicitValueForRightHandSide;
	bMadeChanges = TryRefreshSelectedKeyType(BlackboardData, bAllowRightKeyEntry, KeyRight) || bMadeChanges;

	const bool bAllowRightExplicitValue = bIsKeyLeftSelected && bUseExplicitValueForRightHandSide;
	bMadeChanges = TryRefreshExplicitValue(BlackboardData, bAllowRightExplicitValue, ExplicitValueRight) || bMadeChanges;

	return bMadeChanges;
}

bool UFlowNodeAddOn_PredicateCompareBlackboardValue::TryRefreshSelectedKeyType(const UBlackboardData& BlackboardData, bool bEnableKeyEntry, FFlowBlackboardEntry& InOutKeyProperty)
{
	if (bEnableKeyEntry)
	{
		UBlackboardKeyType const* KeyType = TryGetKeyType(BlackboardData);
		const bool bMadeChanges = InOutKeyProperty.AllowedTypes.IsEmpty() || !KeyType->IsAllowedByFilter(InOutKeyProperty.AllowedTypes[0]);

		if (bMadeChanges)
		{
			InOutKeyProperty.AllowedTypes.Reset();
			UBlackboardKeyType* InstancedKeyType = NewObject<UBlackboardKeyType>(this, KeyType->GetClass(), NAME_None, RF_NoFlags, const_cast<UBlackboardKeyType*>(KeyType));
			InOutKeyProperty.AllowedTypes.Add(InstancedKeyType);

			InOutKeyProperty.KeyName = NAME_None;
		}

		return bMadeChanges;
	}
	else
	{
		const bool bMadeChanges = !InOutKeyProperty.AllowedTypes.IsEmpty();

		if (bMadeChanges)
		{
			InOutKeyProperty.AllowedTypes.Reset();
			InOutKeyProperty.KeyName = NAME_None;
		}

		return bMadeChanges;
	}
}

bool UFlowNodeAddOn_PredicateCompareBlackboardValue::TryRefreshExplicitValue(const UBlackboardData& BlackboardData, bool bEnableExplicitValue, TObjectPtr<UFlowBlackboardEntryValue>& InOutExplicitValue)
{
	if (bEnableExplicitValue)
	{
		const UBlackboardKeyType* KeyType = TryGetKeyType(BlackboardData);
		check(IsValid(KeyType));

		// Create the ExplicitValue object if we don't have one or it is not of the desired type
		TSubclassOf<UFlowBlackboardEntryValue> DesiredValueClass = UFlowBlackboardEntryValue::GetFlowBlackboardEntryValueClassForKeyType(KeyType->GetClass());
		bool bMadeChanges = !IsValid(InOutExplicitValue) || DesiredValueClass != InOutExplicitValue->GetClass();

		if (bMadeChanges)
		{
			InOutExplicitValue = NewObject<UFlowBlackboardEntryValue>(this, DesiredValueClass);

			// Hide the "key" of the UFlowBlackboardEntryValue, as we are only using this helper object to store the value,
			// and not using it to set a key/value pair, in this case.
			InOutExplicitValue->KeyVisibility = EFlowBlackboardEntryValueKeyVisibility::NotVisible;
		}

		// Configure the subtype of the UFlowBlackboardEntryValue based on the KeyType's subtype
		bMadeChanges = InOutExplicitValue->TryReconfigureFromBlackboardKeyType(*KeyType) || bMadeChanges;

		return bMadeChanges;
	}
	else
	{
		// Clear the ExplicitValue object if we no longer enable explicit value
		const bool bMadeChanges = IsValid(InOutExplicitValue);

		InOutExplicitValue = nullptr;

		return bMadeChanges;
	}
}

TSubclassOf<UBlackboardKeyType> UFlowNodeAddOn_PredicateCompareBlackboardValue::TryGetKeyTypeClass(const UBlackboardData& BlackboardData) const
{
	UBlackboardKeyType const* KeyType = TryGetKeyType(BlackboardData);
	if (IsValid(KeyType))
	{
		return KeyType->GetClass();
	}

	return nullptr;
}

UBlackboardKeyType const* UFlowNodeAddOn_PredicateCompareBlackboardValue::TryGetKeyType(const UBlackboardData& BlackboardData) const
{
	FBlackboard::FKey KeyID = FBlackboard::InvalidKey;
	FBlackboardEntry const* KeyEntry = nullptr;

	constexpr bool bWarnIfMissing = false;

	if (!TryGetBlackboardKeyInfo(BlackboardData, KeyLeft, KeyID, KeyEntry, bWarnIfMissing))
	{
		return nullptr;
	}

	return KeyEntry->KeyType;
}

UBlackboardData* UFlowNodeAddOn_PredicateCompareBlackboardValue::GetBlackboardAssetForEditor() const
{
	if (IsValid(SpecificBlackboardAsset))
	{
		return SpecificBlackboardAsset;
	}

	return GetBlackboardAsset();
}

UBlackboardData* UFlowNodeAddOn_PredicateCompareBlackboardValue::GetBlackboardAssetForPropertyHandle(const TSharedPtr<IPropertyHandle>& PropertyHandle) const
{
	if (UBlackboardData* BlackboardAssetForEditor = GetBlackboardAssetForEditor())
	{
		return BlackboardAssetForEditor;
	}
	
	return Super::GetBlackboardAssetForPropertyHandle(PropertyHandle);
}

FText UFlowNodeAddOn_PredicateCompareBlackboardValue::GetNodeTitle() const
{
	if (!KeyLeft.GetKeyName().IsNone() && UFlowSettings::Get()->bUseAdaptiveNodeTitles)
	{
		const FText LHS = FText::FromName(KeyLeft.GetKeyName());
		const FText Operator = FText::FromString(GetOperatorSymbolString(OperatorType));

		// Quoting "" the explicit value to make a key/literal comparison visually distinct from a key/key comparison
		const FText RHS = 
			bUseExplicitValueForRightHandSide ?
				FText::FromString(TEXT("\"") + (ExplicitValueRight ? ExplicitValueRight->GetEditorValueString() : TEXT("<unknown>")) + TEXT("\"")) :
				FText::FromName(KeyRight.GetKeyName());
		return FText::Format(LOCTEXT("CustomCompareBlackboardValueTitle", "{0} {1} {2}"), { LHS, Operator, RHS });
	}

	return Super::GetNodeTitle();
}

#endif // WITH_EDITOR

bool UFlowNodeAddOn_PredicateCompareBlackboardValue::TryGetBlackboardKeyInfo(
	const UBlackboardData& BlackboardData,
	const FFlowBlackboardEntry& KeyEntry,
	FBlackboard::FKey& OutKeyID,
	FBlackboardEntry const*& OutKeyTypeEntry,
	bool bWarnIfMissing) const
{
	const FName& KeyName = KeyEntry.GetKeyName();
	if (KeyName.IsNone())
	{
		return false;
	}

	OutKeyID = BlackboardData.GetKeyID(KeyName);

	if (OutKeyID == FBlackboard::InvalidKey)
	{
		if (bWarnIfMissing)
		{
			LogError(
				FString::Printf(
					TEXT("Invalid blackboard key %s in blackboard %s"),
					*KeyName.ToString(),
					*BlackboardData.GetName()));
		}

		return false;
	}

	OutKeyTypeEntry = BlackboardData.GetKey(OutKeyID);

	if (!OutKeyTypeEntry)
	{
		LogError(FString::Printf(TEXT("Invalid blackboard key type for %s in blackboard %s!"), *KeyName.ToString(), *BlackboardData.GetName()));

		return false;
	}

	return true;
}

bool UFlowNodeAddOn_PredicateCompareBlackboardValue::EvaluatePredicate_Implementation() const
{
	const FAIFlowCachedBlackboardReference CachedBlackboard(*this, SpecificBlackboardAsset, SpecificBlackboardSearchRule);

	if (!CachedBlackboard.IsValid())
	{
		LogError(TEXT("Cannot EvaluatePredicate on a blackboard key without a Blackboard Component or Asset"));

		return false;
	}

	// Get the Left key first, which tells us what type we're dealing with
	constexpr bool bWarnIfBlackboardKeysAreMissing = true;
	FBlackboard::FKey KeyLeftID = FBlackboard::InvalidKey;
	FBlackboardEntry const* KeyLeftTypeEntry = nullptr;
	if (!TryGetBlackboardKeyInfo(*CachedBlackboard.BlackboardData, KeyLeft, KeyLeftID, KeyLeftTypeEntry, bWarnIfBlackboardKeysAreMissing))
	{
		LogError(TEXT("Cannot EvaluatePredicate on a blackboard key without a valid Key (left)"));

		return false;
	}

	const TSubclassOf<UBlackboardKeyType> KeyLeftTypeClass = KeyLeftTypeEntry->KeyType->GetClass();

	// Are we comparing vs. an explicit value or against another blackboard key's value?
	bool bCompareResult = false;
	if (IsValid(ExplicitValueRight))
	{
		// do the key vs explicit value comparison
		bCompareResult = ComputeCompareResultWithExplicitValue(*CachedBlackboard.BlackboardComponent, KeyLeftTypeClass, KeyLeftID, *ExplicitValueRight);
	}
	else
	{
		// Look up the right-hand side key's information
		FBlackboard::FKey KeyRightID = FBlackboard::InvalidKey;
		FBlackboardEntry const* KeyRightTypeEntry = nullptr;
		if (!TryGetBlackboardKeyInfo(*CachedBlackboard.BlackboardData, KeyRight, KeyRightID, KeyRightTypeEntry, bWarnIfBlackboardKeysAreMissing))
		{
			LogError(TEXT("Cannot EvaluatePredicate on a blackboard key without a valid Key (right)"));

			return false;
		}

		const TSubclassOf<UBlackboardKeyType> KeyRightTypeClass = KeyLeftTypeEntry->KeyType->GetClass();
		if (KeyLeftTypeClass != KeyRightTypeClass)
		{
			LogError(
				FString::Printf(
					TEXT("Cannot EvaluatePredicate on a blackboard key with mismatched key types: left %s %s and right %s %s"),
					*KeyLeft.GetKeyName().ToString(),
					KeyLeftTypeClass ? *KeyLeftTypeClass->GetName() : TEXT("<null>"),
					*KeyRight.GetKeyName().ToString(),
					KeyRightTypeClass ? *KeyRightTypeClass->GetName() : TEXT("<null>")));

			return false;
		}

		// Do the key vs. key comparison 
		bCompareResult = ComputeCompareResult(*CachedBlackboard.BlackboardComponent, KeyLeftTypeClass, KeyLeftID, KeyRightID);
	}

	return bCompareResult;
}

bool UFlowNodeAddOn_PredicateCompareBlackboardValue::ComputeCompareResultWithExplicitValue(
	const UBlackboardComponent& BlackboardComponent,
	const TSubclassOf<UBlackboardKeyType> BlackboardKeyType,
	const FBlackboard::FKey LeftKeyID,
	const UFlowBlackboardEntryValue& RightExplicitValue) const
{
	if (IsEqualityOperation(OperatorType))
	{
		// Do the equality (==, !=) comparison
		const EBlackboardCompare::Type CompareResult = ExplicitValueRight->CompareKeyValues(&BlackboardComponent, KeyLeft.GetKeyName());

		const bool bIsMatch = (CompareResult == EBlackboardCompare::Equal);
		const bool bExpectsMatch = (OperatorType == EPredicateCompareOperatorType::Equal);

		const bool bActualResultMatchedExpectation = (bIsMatch == bExpectsMatch);

		return bActualResultMatchedExpectation;
	}

	if (IsArithmeticOperation(OperatorType))
	{
		// Do the arithmetic (<, <=, >, >=) comparison
		int32 RightIntValue = 0;
		float RightFloatValue = 0.0f;
		if (!RightExplicitValue.TryGetNumericalValuesForArithmeticOperation(&RightIntValue, &RightFloatValue))
		{
			// If the type does not support arithmetic operations, TryGetNumericalValuesForArithmeticOperation()
			// will always return false (as that is the default in UFlowBlackboardEntryValue), 
			// so this check will prevent an arithmetic operation on types that do not support it.

			LogError(
				FString::Printf(
					TEXT("%s does not support arithmetic comparison operations"),
					*RightExplicitValue.GetName()));

			return false;
		}

		const UBlackboardKeyType* BlackboardKeyTypeCDO = BlackboardKeyType->GetDefaultObject<UBlackboardKeyType>();
		check(IsValid(BlackboardKeyTypeCDO));

		const uint8* LeftMemory = BlackboardComponent.GetKeyRawData(LeftKeyID);
		check(LeftMemory);

		const EArithmeticKeyOperation::Type ArithmeticOp = ConvertPredicateCompareOperatorTypeToArithmeticKeyOperation(OperatorType);

		const bool bArithmeticResult = 
			BlackboardKeyTypeCDO->WrappedTestArithmeticOperation(
				BlackboardComponent,
				LeftMemory,
				ArithmeticOp,
				RightIntValue,
				RightFloatValue);

		return bArithmeticResult;
	}

	LogError(FString::Printf(TEXT("Incorrectly configured CompareBlackboardValues %s"), *GetName()));

	return false;
}

bool UFlowNodeAddOn_PredicateCompareBlackboardValue::ComputeCompareResult(
	const UBlackboardComponent& BlackboardComponent,
	const TSubclassOf<UBlackboardKeyType> BlackboardKeyType,
	const FBlackboard::FKey LeftKeyID,
	const FBlackboard::FKey RightKeyID) const
{
	if (IsEqualityOperation(OperatorType))
	{
		// Do the equality (==, !=) comparison
		const EBlackboardCompare::Type CompareResult = BlackboardComponent.CompareKeyValues(BlackboardKeyType, LeftKeyID, RightKeyID);

		const bool bIsMatch = (CompareResult == EBlackboardCompare::Equal);
		const bool bExpectsMatch = (OperatorType == EPredicateCompareOperatorType::Equal);

		const bool bActualResultMatchedExpectation = (bIsMatch == bExpectsMatch);

		return bActualResultMatchedExpectation;
	}

	if (IsArithmeticOperation(OperatorType))
	{
		// Do the arithmetic (<, <=, >, >=) comparison

		// Fetch the numerical values for the right side blackboard key
		int32 RightIntValue = 0;
		float RightFloatValue = 0.0f;
		if (BlackboardKeyType == UBlackboardKeyType_Float::StaticClass())
		{
			RightFloatValue = BlackboardComponent.GetValueAsFloat(KeyRight.GetKeyName());
			RightIntValue = FMath::FloorToInt32(RightFloatValue);
		}
		else if (BlackboardKeyType == UBlackboardKeyType_Int::StaticClass())
		{
			RightIntValue = BlackboardComponent.GetValueAsInt(KeyRight.GetKeyName());
			RightFloatValue = static_cast<float>(RightIntValue);
		}
		else if (BlackboardKeyType == UBlackboardKeyType_Enum::StaticClass())
		{
			RightIntValue = BlackboardComponent.GetValueAsEnum(KeyRight.GetKeyName());
			RightFloatValue = static_cast<float>(RightIntValue);
		}
		else
		{
			LogError(
				FString::Printf(
					TEXT("%s does not support arithmetic comparison operations"),
					BlackboardKeyType ? *BlackboardKeyType->GetName() : TEXT("<null>")));

			return false;
		}

		const UBlackboardKeyType * BlackboardKeyTypeCDO = BlackboardKeyType->GetDefaultObject<UBlackboardKeyType>();
		check(IsValid(BlackboardKeyTypeCDO));

		const uint8* LeftMemory = BlackboardComponent.GetKeyRawData(LeftKeyID);
		check(LeftMemory);

		const EArithmeticKeyOperation::Type ArithmeticOp = ConvertPredicateCompareOperatorTypeToArithmeticKeyOperation(OperatorType);

		const bool bArithmeticResult =
			BlackboardKeyTypeCDO->WrappedTestArithmeticOperation(
				BlackboardComponent,
				LeftMemory,
				ArithmeticOp,
				RightIntValue,
				RightFloatValue);

		return bArithmeticResult;
	}

	LogError(FString::Printf(TEXT("Incorrectly configured CompareBlackboardValues %s"), *GetName()));

	return false;
}

EArithmeticKeyOperation::Type UFlowNodeAddOn_PredicateCompareBlackboardValue::ConvertPredicateCompareOperatorTypeToArithmeticKeyOperation(
	EPredicateCompareOperatorType OperatorType)
{
	// EPredicateCompareOperatorType is a matching enumeration to EArithmeticKeyOperation
	static_assert(static_cast<int32>(EArithmeticKeyOperation::Equal) == static_cast<int32>(EPredicateCompareOperatorType::Equal), TEXT("These should be numerically equivilent"));
	static_assert(static_cast<int32>(EArithmeticKeyOperation::NotEqual) == static_cast<int32>(EPredicateCompareOperatorType::NotEqual), TEXT("These should be numerically equivilent"));
	static_assert(static_cast<int32>(EArithmeticKeyOperation::Less) == static_cast<int32>(EPredicateCompareOperatorType::Less), TEXT("These should be numerically equivilent"));
	static_assert(static_cast<int32>(EArithmeticKeyOperation::LessOrEqual) == static_cast<int32>(EPredicateCompareOperatorType::LessOrEqual), TEXT("These should be numerically equivilent"));
	static_assert(static_cast<int32>(EArithmeticKeyOperation::Greater) == static_cast<int32>(EPredicateCompareOperatorType::Greater), TEXT("These should be numerically equivilent"));
	static_assert(static_cast<int32>(EArithmeticKeyOperation::GreaterOrEqual) == static_cast<int32>(EPredicateCompareOperatorType::GreaterOrEqual), TEXT("These should be numerically equivilent"));

	return static_cast<EArithmeticKeyOperation::Type>(OperatorType);
}

#undef LOCTEXT_NAMESPACE
