// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#pragma once

#include "BehaviorTree/Blackboard/BlackboardKeyEnums.h"
#include "Interfaces/FlowDataPinPropertyProviderInterface.h"
#include "Interfaces/FlowBlackboardAssetProvider.h"
#include "Templates/SubclassOf.h"
#include "UObject/Object.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "Types/FlowBlackboardEntry.h"

#include "FlowBlackboardEntryValue.generated.h"

// Forward Declarations
class UBlackboardKeyType;
class UBlackboardComponent;
class UFlowBlackboardEntryValue;
class UFlowNode;
struct FFlowDataPinProperty;

// Enum to control visibility of the UFlowBlackboardEntryValue's Key in EditCondition
UENUM()
enum class EFlowBlackboardEntryValueKeyVisibility : uint8
{
	Visible,
	NotVisible,
};

/**
 * Configuration base class for setting blackboard entries for UBlackboardKeyType entries
 */
UCLASS(Abstract, BlueprintType, EditInlineNew, DisplayName = "Blackboard Value")
class AIFLOW_API UFlowBlackboardEntryValue
	: public UObject
	, public IFlowBlackboardAssetProvider
	, public IFlowDataPinPropertyProviderInterface
{
	GENERATED_BODY()

public:

	// UFlowBlackboardEntryValue

	// Uses the data in this UFlowBlackboardEntryValue to set the matching key's value on the given blackboard
	virtual void SetOnBlackboardComponent(UBlackboardComponent* BlackboardComponent) const PURE_VIRTUAL(SetOnBlackboardComponent);

	// Compares the value contained in this object vs. the given key's value on the blackboard,
	// similar to UBlackboardComponent::CompareKeyValues()
	virtual EBlackboardCompare::Type CompareKeyValues(const UBlackboardComponent* BlackboardComponent, const FName& OtherKeyName) const PURE_VIRTUAL(CompareKeyValues, return EBlackboardCompare::NotEqual;);

	// Returns the UBlackboardKeyType subclass that this UFlowBlackboardKeyValue is built for
	virtual TSubclassOf<UBlackboardKeyType> GetSupportedBlackboardKeyType() const PURE_VIRTUAL(GetSupportedBlackboardKeyType, return nullptr;);

	// Worker function for arithmetic compare operations (of the form EArithmeticCompare) that are done on blackboard entries.
	// Only subclasses that SupportArithmeticOperations need to implement this function.
	virtual bool TryGetNumericalValuesForArithmeticOperation(int32 * OutIntValue = nullptr, float* OutFloatValue = nullptr) const { return false; }

	// Set this UFlowBlackboardEntryValue's value to the value from a Data Pin, resolved by PinOwnerFlowNode
	virtual bool TrySetValueFromInputDataPin(const FName& PinName, UFlowNode& PinOwnerFlowNode) PURE_VIRTUAL(TrySetValueFromInputDataPin, return false;);

#if WITH_EDITOR
	// Does this class support arithmetic compare operations (of the form EArithmeticCompare) and 
	// thus supports TryGetNumericalValuesForArithmeticOperation ?
	virtual bool SupportsArithmeticOperations() const { return false; }

	// Tries to reconfigure this object to match the given UBlackboardKeyType.
	// Must be a supported UBlackboardKeyType subclass.
	// This is used when procedurally reconfiguring EnumClass and other subtype changes etc. in editor 
	virtual bool TryReconfigureFromBlackboardKeyType(const UBlackboardKeyType& KeyType);

	// Returns the Value of this object in string form, for editor use
	virtual FString GetEditorValueString() const { return FString(); }

	// Returns the NodeConfigText, used in populating the "NodeConfig" area of FlowNode & AddOns in the flow editor
	virtual FText BuildNodeConfigText() const PURE_VIRTUAL(BuildNodeConfigText, return FText();)

	// Returns the first found UFlowBlackboardEntryValue subclass that supports the given UBlackboardKeyType, 
	// from the available subclasses.
	static TSubclassOf<UFlowBlackboardEntryValue> GetFlowBlackboardEntryValueClassForKeyType(TSubclassOf<UBlackboardKeyType> KeyTypeClass);
#endif // WITH_EDITOR
	// --

	// IBlackboardAssetProvider
	virtual UBlackboardData* GetBlackboardAsset() const override;
	// --

#if WITH_EDITOR
	// IFlowBlackboardAssetProvider
	virtual UBlackboardData* GetBlackboardAssetForPropertyHandle(const TSharedPtr<IPropertyHandle>& PropertyHandle) const override;
	// --
#endif // WITH_EDITOR

	// IFlowDataPinPropertyProviderInterface
	virtual bool TryProvideFlowDataPinProperty(const bool bIsInputPin, TInstancedStruct<FFlowDataPinProperty>& OutFlowDataPinProperty) const { return false; }
	// --

	// Try to provide a blackboard entry value for a key, if the type is supported by this class, in the form of a FFlowDataPinProperty
	virtual bool TryProvideFlowDataPinPropertyFromBlackboardEntry(
		const FName& BlackboardKeyName,
		const UBlackboardKeyType& BlackboardKeyType,
		UBlackboardComponent* OptionalBlackboardComponent,
		TInstancedStruct<FFlowDataPinProperty>& OutFlowDataPinProperty) const PURE_VIRTUAL(TryProvideFlowDataPinPropertyFromBlackboardEntry, return false;);

	// Ensures the CachedBlackboardEntryValueSubclassArray has been cached and returns its value
	static const TArray<TWeakObjectPtr<UClass>>& EnsureBlackboardEntryValueSubclassArray();

protected:

	// Template worker function for TryProvideFlowDataPinPropertyFromBlackboardEntry()
	template <typename TBlackboardEntryType, typename TFlowDataPinOutputPropertyType>
	static bool TryProvideFlowDataPinPropertyFromBlackboardEntryTemplate(
		const FName& BlackboardKeyName,
		const UBlackboardKeyType& BlackboardKeyType,
		UBlackboardComponent* OptionalBlackboardComponent,
		TInstancedStruct<FFlowDataPinProperty>& OutFlowDataPinProperty)
	{
		if (BlackboardKeyType.IsA<TBlackboardEntryType>())
		{
			const typename TBlackboardEntryType::FDataType Value =
				OptionalBlackboardComponent ?
					OptionalBlackboardComponent->GetValue<TBlackboardEntryType>(BlackboardKeyName) :
					TBlackboardEntryType::InvalidValue;

			OutFlowDataPinProperty.InitializeAs<TFlowDataPinOutputPropertyType>(Value);

			return true;
		}

		return false;
	}

public:

	// Target blackboard key for this entry to set
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Configuration, meta = (EditCondition = "KeyVisibility == EFlowBlackboardEntryValueKeyVisibility::Visible", EditConditionHides))
	FFlowBlackboardEntry Key;

	// Cached array of all of the subclasses of UFlowBlackboardEntryValue
	static TArray<TWeakObjectPtr<UClass>> CachedBlackboardEntryValueSubclassArray;

#if WITH_EDITORONLY_DATA
	// Used to control visibility of Key property
	// (in some use-cases, we only want to use the value portion of the FFlowBlackboardEntry, 
	// and do not want to show the Key in the editor).
	UPROPERTY(EditDefaultsOnly, Category = Configuration)
	EFlowBlackboardEntryValueKeyVisibility KeyVisibility = EFlowBlackboardEntryValueKeyVisibility::Visible;
#endif // WITH_EDITORONLY_DATA
};

