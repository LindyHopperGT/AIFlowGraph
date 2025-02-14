// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#include "Blackboard/FlowBlackboardEntryValue_Object.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIFlowLogChannels.h"
#include "Nodes/FlowNode.h"
#include "Types/FlowDataPinProperties.h"
#include "Types/FlowDataPinResults.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlowBlackboardEntryValue_Object)

#if WITH_EDITOR
void UFlowBlackboardEntryValue_Object::PostInitProperties()
{
	Super::PostInitProperties();

	if (!HasAnyFlags(RF_ArchetypeObject | RF_ClassDefaultObject))
	{
		if (Key.AllowedTypes.IsEmpty())
		{
			UBlackboardKeyType_Object* ObjectType = NewObject<UBlackboardKeyType_Object>(this);
			ObjectType->BaseClass = BaseClass;
			Key.AllowedTypes.Add(ObjectType);
		}
	}
}

bool UFlowBlackboardEntryValue_Object::TryReconfigureFromBlackboardKeyType(const UBlackboardKeyType& KeyType)
{
	const bool bSuperMadeChanges = Super::TryReconfigureFromBlackboardKeyType(KeyType);

	// Superclass is responsible for ensuring the UBlackboardKeyType_Object is correct,
	// but the subtype (eg BaseClass) may be mismatched, so we will need to ensure that here.

	check(Key.AllowedTypes.Num() == 1 && Key.AllowedTypes[0]->IsA(GetSupportedBlackboardKeyType()));

	const UBlackboardKeyType_Object* ObjectKeyType = CastChecked<UBlackboardKeyType_Object>(&KeyType);
	if (BaseClass != ObjectKeyType->BaseClass)
	{
		BaseClass = ObjectKeyType->BaseClass;

		RefreshObjectTypeFromBaseClass();
		EnsureObjectsAreCompatibleWithBaseClass();

		return true;
	}

	return bSuperMadeChanges;
}

FString UFlowBlackboardEntryValue_Object::GetEditorValueString() const
{
	if (IsValid(ObjectInstance))
	{
		return *ObjectInstance->GetName();
	}
	else if (IsValid(ObjectAsset))
	{
		return *ObjectAsset->GetName();
	}
	else
	{
		return TEXT("<none>");
	}
}

FText UFlowBlackboardEntryValue_Object::BuildNodeConfigText() const
{
	return FText::FromString(FString::Printf(TEXT("Set %s to \"%s\""), *Key.GetKeyName().ToString(), *GetEditorValueString()));
}

void UFlowBlackboardEntryValue_Object::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	const FName MemberPropertyName = PropertyChangedEvent.MemberProperty->GetFName();

	if (MemberPropertyName == GET_MEMBER_NAME_CHECKED(UFlowBlackboardEntryValue_Object, BaseClass))
	{
		if (ensure(Key.AllowedTypes.Num() == 1))
		{
			// Update the BaseClass on the Key whenever it changes
			UBlackboardKeyType_Object* ObjectType = CastChecked<UBlackboardKeyType_Object>(Key.AllowedTypes[0]);

			ObjectType->BaseClass = BaseClass;
		}
	}

	RefreshObjectTypeFromBaseClass();
	EnsureObjectsAreCompatibleWithBaseClass();
}

void UFlowBlackboardEntryValue_Object::EnsureObjectsAreCompatibleWithBaseClass()
{
	if (ObjectInstance && !ObjectInstance->IsA(BaseClass))
	{
		// Clear the ObjectInstance if it is not compatible with the BaseClass
		ObjectInstance = nullptr;
	}

	if (ObjectAsset && !ObjectAsset->IsA(BaseClass))
	{
		// Clear the ObjectAsset if it is not compatible with the BaseClass
		ObjectAsset = nullptr;
	}
}

void UFlowBlackboardEntryValue_Object::RefreshObjectTypeFromBaseClass()
{
	if (BaseClass)
	{
		const bool bIsInstanced = (BaseClass->ClassFlags & CLASS_EditInlineNew) != 0;
		if (bIsInstanced)
		{
			ObjectTypeSelector = EObjectInstanceTypeSelector::Instanced;
		}
		else
		{
			ObjectTypeSelector = EObjectInstanceTypeSelector::Asset;
		}
	}
	else
	{
		ObjectTypeSelector = EObjectInstanceTypeSelector::Unknown;
	}
}

#endif // WITH_EDITOR

bool UFlowBlackboardEntryValue_Object::TryProvideFlowDataPinProperty(const bool bIsInputPin, TInstancedStruct<FFlowDataPinProperty>& OutFlowDataPinProperty) const
{
	UObject* ObjectValue = GetObjectValue();

	UClass* ClassFilter = nullptr;

#if WITH_EDITOR
	// Only the editor data has the BaseClass (and also FFlowDataPinOutputProperty_Object::ClassFilter)
	// so we only can supply (or use) that information in editor builds
	ClassFilter = BaseClass;
#endif // WITH_EDITOR

	if (bIsInputPin)
	{
		OutFlowDataPinProperty.InitializeAs<FFlowDataPinInputProperty_Object>(ObjectValue, ClassFilter);
	}
	else
	{
		OutFlowDataPinProperty.InitializeAs<FFlowDataPinOutputProperty_Object>(ObjectValue, ClassFilter);
	}

	return true;
}

bool UFlowBlackboardEntryValue_Object::TryProvideFlowDataPinPropertyFromBlackboardEntry(
	const FName& BlackboardKeyName,
	const UBlackboardKeyType& BlackboardKeyType,
	UBlackboardComponent* OptionalBlackboardComponent,
	TInstancedStruct<FFlowDataPinProperty>& OutFlowDataPinProperty) const
{
	if (TryProvideFlowDataPinPropertyFromBlackboardEntryTemplate<UBlackboardKeyType_Object, FFlowDataPinOutputProperty_Object>(
		BlackboardKeyName,
		BlackboardKeyType,
		OptionalBlackboardComponent,
		OutFlowDataPinProperty))
	{
#if WITH_EDITOR
		const UBlackboardKeyType_Object* TypedKeyType = CastChecked<UBlackboardKeyType_Object>(&BlackboardKeyType);
		FFlowDataPinOutputProperty_Object* MutableProperty = OutFlowDataPinProperty.GetMutablePtr<FFlowDataPinOutputProperty_Object>();

		// Only the editor data has the BaseClass or ClassFilter
		// so we only can supply (or use) that information in editor builds
		MutableProperty->ClassFilter = TypedKeyType->BaseClass;
#endif // WITH_EDITOR

		return true;
	}

	return false;
}

UObject* UFlowBlackboardEntryValue_Object::GetObjectValue() const
{
	return ObjectAsset ? ObjectAsset : ObjectInstance;
}

void UFlowBlackboardEntryValue_Object::SetObjectValue(UObject* InValue)
{
	UClass* ObjectClass = IsValid(InValue) ? InValue->GetClass() : nullptr;
	if (IsValid(ObjectClass))
	{
		const bool bIsInstanced = (ObjectClass->ClassFlags & CLASS_EditInlineNew) != 0;

		if (bIsInstanced)
		{
			ObjectInstance = InValue;
			ObjectAsset = nullptr;
		}
		else
		{
			ObjectInstance = nullptr;
			ObjectAsset = InValue;
		}
	}
	else
	{
		ObjectInstance = nullptr;
		ObjectAsset = nullptr;
	}
}

bool UFlowBlackboardEntryValue_Object::TrySetValueFromInputDataPin(const FName& PinName, UFlowNode& PinOwnerFlowNode)
{
	const FFlowDataPinResult_Object FlowDataPinResult = PinOwnerFlowNode.TryResolveDataPinAsObject(PinName);

	if (FlowDataPinResult.Result == EFlowDataPinResolveResult::Success)
	{
		SetObjectValue(FlowDataPinResult.Value);

		return true;
	}

	return false;
}

void UFlowBlackboardEntryValue_Object::SetOnBlackboardComponent(UBlackboardComponent* BlackboardComponent) const
{
	if (IsValid(BlackboardComponent))
	{
		if (ObjectInstance)
		{
			BlackboardComponent->SetValueAsObject(Key.GetKeyName(), ObjectInstance);
		}
		else
		{
			BlackboardComponent->SetValueAsObject(Key.GetKeyName(), ObjectAsset);
		}
	}
}

EBlackboardCompare::Type UFlowBlackboardEntryValue_Object::CompareKeyValues(const UBlackboardComponent* BlackboardComponent, const FName& OtherKeyName) const
{
	if (!IsValid(BlackboardComponent))
	{
		UE_LOG(LogAIFlow, Error, TEXT("Cannot CompareKeyValues without a Blackboard!"));
		return EBlackboardCompare::NotEqual;
	}

	const UObject* OtherValueAsObject = BlackboardComponent->GetValueAsObject(OtherKeyName);

	if ((ObjectInstance && ObjectInstance == OtherValueAsObject) ||
		(ObjectAsset == OtherValueAsObject))
	{
		return EBlackboardCompare::Equal;
	}
	else
	{
		return EBlackboardCompare::NotEqual;
	}
}

TSubclassOf<UBlackboardKeyType> UFlowBlackboardEntryValue_Object::GetSupportedBlackboardKeyType() const
{
	return UBlackboardKeyType_Object::StaticClass();
}
