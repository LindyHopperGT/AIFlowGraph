// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#include "Blackboard/FlowBlackboardEntryValue_Class.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Class.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIFlowLogChannels.h"
#include "Types/FlowPinEnums.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlowBlackboardEntryValue_Class)

#if WITH_EDITOR
void UFlowBlackboardEntryValue_Class::PostInitProperties()
{
	Super::PostInitProperties();

	if (!HasAnyFlags(RF_ArchetypeObject | RF_ClassDefaultObject))
	{
		if (Key.AllowedTypes.IsEmpty())
		{
			UBlackboardKeyType_Class* ClassType = NewObject<UBlackboardKeyType_Class>(this);
			ClassType->BaseClass = BaseClass;
			Key.AllowedTypes.Add(ClassType);
		}
	}
}

bool UFlowBlackboardEntryValue_Class::TryReconfigureFromBlackboardKeyType(const UBlackboardKeyType& KeyType)
{
	const bool bSuperMadeChanges = Super::TryReconfigureFromBlackboardKeyType(KeyType);

	// Superclass is responsible for ensuring the UBlackboardKeyType_Class is correct,
	// but the subtype (BaseClass) may be mismatched, so we will need to ensure that here.

	check(Key.AllowedTypes.Num() == 1 && Key.AllowedTypes[0]->IsA(GetSupportedBlackboardKeyType()));

	const UBlackboardKeyType_Class* ClassKeyType = CastChecked<UBlackboardKeyType_Class>(&KeyType);
	if (BaseClass != ClassKeyType->BaseClass)
	{
		BaseClass = ClassKeyType->BaseClass;

		EnsureClassInstanceIsCompatibleWithBaseClass();

		return true;
	}

	return bSuperMadeChanges;
}

FString UFlowBlackboardEntryValue_Class::GetEditorValueString() const
{
	if (IsValid(ClassInstance))
	{
		return *ClassInstance->GetName();
	}
	else
	{
		return TEXT("<none>");
	}
}

FText UFlowBlackboardEntryValue_Class::BuildNodeConfigText() const
{
	return FText::FromString(FString::Printf(TEXT("Set %s to \"%s\""), *Key.GetKeyName().ToString(), *GetEditorValueString()));
}

void UFlowBlackboardEntryValue_Class::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	const FName MemberPropertyName = PropertyChangedEvent.MemberProperty->GetFName();

	if (MemberPropertyName == GET_MEMBER_NAME_CHECKED(UFlowBlackboardEntryValue_Class, BaseClass))
	{
		if (ensure(Key.AllowedTypes.Num() == 1))
		{
			UBlackboardKeyType_Class* ClassType = CastChecked<UBlackboardKeyType_Class>(Key.AllowedTypes[0]);

			ClassType->BaseClass = BaseClass;
		}
	}

	EnsureClassInstanceIsCompatibleWithBaseClass();
}

void UFlowBlackboardEntryValue_Class::EnsureClassInstanceIsCompatibleWithBaseClass()
{
	if (ClassInstance && !ClassInstance->IsChildOf(BaseClass))
	{
		// Clear the ClassInstance if it is not compatible with the BaseClass
		ClassInstance = nullptr;
	}
}

#endif // WITH_EDITOR

bool UFlowBlackboardEntryValue_Class::TryProvideFlowDataPinProperty(const bool bIsInputPin, TInstancedStruct<FFlowDataPinProperty>& OutFlowDataPinProperty) const
{
	// TODO (gtaylor) Implement Class data pin support
	FLOW_ASSERT_ENUM_MAX(EFlowPinType, 13);

	return false;
}

bool UFlowBlackboardEntryValue_Class::TrySetValueFromInputDataPin(const FName& PinName, UFlowNode& PinOwnerFlowNode)
{
	// TODO (gtaylor) Implement Class data pin support

	return false;
}

void UFlowBlackboardEntryValue_Class::SetOnBlackboardComponent(UBlackboardComponent* BlackboardComponent) const
{
	if (IsValid(BlackboardComponent))
	{
		BlackboardComponent->SetValueAsClass(Key.GetKeyName(), ClassInstance);
	}
}

EBlackboardCompare::Type UFlowBlackboardEntryValue_Class::CompareKeyValues(const UBlackboardComponent* BlackboardComponent, const FName& OtherKeyName) const
{
	if (!IsValid(BlackboardComponent))
	{
		UE_LOG(LogAIFlow, Error, TEXT("Cannot CompareKeyValues without a Blackboard!"));
		return EBlackboardCompare::NotEqual;
	}

	const UClass* OtherValueAsClass = BlackboardComponent->GetValueAsClass(OtherKeyName);

	if (ClassInstance == OtherValueAsClass)
	{
		return EBlackboardCompare::Equal;
	}
	else
	{
		return EBlackboardCompare::NotEqual;
	}
}

TSubclassOf<UBlackboardKeyType> UFlowBlackboardEntryValue_Class::GetSupportedBlackboardKeyType() const
{
	return UBlackboardKeyType_Class::StaticClass();
}
