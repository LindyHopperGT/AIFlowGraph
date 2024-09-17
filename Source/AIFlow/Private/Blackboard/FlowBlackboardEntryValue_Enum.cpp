// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#include "Blackboard/FlowBlackboardEntryValue_Enum.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Enum.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIFlowLogChannels.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlowBlackboardEntryValue_Enum)

#if WITH_EDITOR

void UFlowBlackboardEntryValue_Enum::PostInitProperties()
{
	Super::PostInitProperties();

	if (!HasAnyFlags(RF_ArchetypeObject | RF_ClassDefaultObject))
	{
		if (Key.AllowedTypes.IsEmpty())
		{
			UBlackboardKeyType_Enum* EnumKeyType = NewObject<UBlackboardKeyType_Enum>(this);

			EnumKeyType->EnumType = EnumValue.EnumClass;

			Key.AllowedTypes.Add(EnumKeyType);
		}
	}
}

bool UFlowBlackboardEntryValue_Enum::TryReconfigureFromBlackboardKeyType(const UBlackboardKeyType& KeyType)
{
	const bool bSuperMadeChanges = Super::TryReconfigureFromBlackboardKeyType(KeyType);

	// Superclass is responsible for ensuring the UBlackboardKeyType_Enum is correct,
	// but the subtype (eg EnumClass) may be mismatched, so we will need to ensure that here.

	check(Key.AllowedTypes.Num() == 1 && Key.AllowedTypes[0]->IsA(GetSupportedBlackboardKeyType()));

	const UBlackboardKeyType_Enum* EnumKeyType = CastChecked<UBlackboardKeyType_Enum>(&KeyType);
	if (EnumValue.EnumClass != EnumKeyType->EnumType)
	{
		EnumValue.EnumClass = EnumKeyType->EnumType;

		// Copy the EnumClass & EnumName sub-configuration over
		if (EnumKeyType->bIsEnumNameValid)
		{
			EnumValue.EnumName = EnumKeyType->EnumName;
		}
		else
		{
			EnumValue.EnumName.Empty();
		}

		EnsureValueIsCompatibleWithEnumClass();

		return true;
	}

	return bSuperMadeChanges;
}

FString UFlowBlackboardEntryValue_Enum::GetEditorValueString() const
{
	return EnumValue.Value.ToString();
}

FText UFlowBlackboardEntryValue_Enum::BuildNodeConfigText() const
{
	return FText::FromString(FString::Printf(TEXT("Set %s to \"%s\""), *Key.GetKeyName().ToString(), *GetEditorValueString()));
}

void UFlowBlackboardEntryValue_Enum::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	const FName MemberPropertyName = PropertyChangedEvent.MemberProperty->GetFName();

	if (MemberPropertyName == GET_MEMBER_NAME_CHECKED(UFlowBlackboardEntryValue_Enum, Key))
	{
		(void) TryUpdateEnumTypesFromKey();
	}
	else if (MemberPropertyName == GET_MEMBER_NAME_CHECKED(UFlowBlackboardEntryValue_Enum, EnumValue))
	{
		if (ensure(Key.AllowedTypes.Num() == 1))
		{
			// Update the EnumClass on the EnumProperty whenever it changes
			UBlackboardKeyType_Enum* EnumKeyType = Cast<UBlackboardKeyType_Enum>(Key.AllowedTypes[0]);

			EnumKeyType->EnumType = EnumValue.EnumClass;
		}
	}

	EnsureValueIsCompatibleWithEnumClass();
}

bool UFlowBlackboardEntryValue_Enum::TryUpdateEnumTypesFromKey()
{
	const UBlackboardData* BlackboardData = GetBlackboardAsset();
	if (!IsValid(BlackboardData))
	{
		return false;
	}

	const FBlackboard::FKey KeyID = BlackboardData->GetKeyID(Key.GetKeyName());
	if (KeyID == FBlackboard::InvalidKey)
	{
		return false;
	}

	const FBlackboardEntry* BlackboardEntry = BlackboardData->GetKey(KeyID);
	if (!BlackboardEntry)
	{
		return false;
	}

	const UBlackboardKeyType_Enum* KeyTypeEnum = Cast<UBlackboardKeyType_Enum>(BlackboardEntry->KeyType);
	if (!IsValid(KeyTypeEnum))
	{
		return false;
	}

	// Apply the UBlackboardKeyType_Enum values to EnumValue (FConfigurableEnumProperty)
	if (KeyTypeEnum->bIsEnumNameValid)
	{
		EnumValue.EnumName = KeyTypeEnum->EnumName;
	}
	else
	{
		EnumValue.EnumName.Empty();
	}

	EnumValue.EnumClass = KeyTypeEnum->EnumType;

	return true;
}

void UFlowBlackboardEntryValue_Enum::EnsureValueIsCompatibleWithEnumClass()
{
	if (!IsValid(EnumValue.EnumClass) && !EnumValue.Value.IsNone())
	{
		EnumValue.Value = NAME_None;
	}

	if (IsValid(EnumValue.EnumClass) && EnumValue.EnumClass->IsValidEnumName(EnumValue.Value))
	{
		if (EnumValue.EnumClass->IsValidEnumValue(-1))
		{
			EnumValue.Value = FName(EnumValue.EnumClass->GetDisplayNameTextByIndex(-1).ToString());
		}
		else
		{
			// Use the last entry if no INDEX_NONE/-1
			EnumValue.Value = FName(EnumValue.EnumClass->GetDisplayNameTextByIndex(EnumValue.EnumClass->NumEnums() - 1).ToString());
		}
	}
}

#endif // WITH_EDITOR

void UFlowBlackboardEntryValue_Enum::SetOnBlackboardComponent(UBlackboardComponent* BlackboardComponent) const
{
	if (IsValid(BlackboardComponent))
	{
		const uint64 EnumValueAsInt = EnumValue.EnumClass->GetValueByName(EnumValue.Value);
		BlackboardComponent->SetValueAsEnum(Key.GetKeyName(), EnumValueAsInt);
	}
}

EBlackboardCompare::Type UFlowBlackboardEntryValue_Enum::CompareKeyValues(const UBlackboardComponent* BlackboardComponent, const FName& OtherKeyName) const
{
	if (!IsValid(BlackboardComponent))
	{
		UE_LOG(LogAIFlow, Error, TEXT("Cannot CompareKeyValues without a Blackboard!"));
		return EBlackboardCompare::NotEqual;
	}

	if (!EnumValue.EnumClass)
	{
		return EBlackboardCompare::NotEqual;
	}

	const uint64 EnumValueAsInt = EnumValue.EnumClass->GetValueByName(EnumValue.Value);
	const uint8 OtherValueAsEnumInt = BlackboardComponent->GetValueAsEnum(OtherKeyName);

	// NOTE (gtaylor) Is there a way to verify that the OtherKey's enum class is the same as EnumClass?

	if (EnumValueAsInt == OtherValueAsEnumInt)
	{
		return EBlackboardCompare::Equal;
	}
	else
	{
		return EBlackboardCompare::NotEqual;
	}
}

TSubclassOf<UBlackboardKeyType> UFlowBlackboardEntryValue_Enum::GetSupportedBlackboardKeyType() const
{
	return UBlackboardKeyType_Enum::StaticClass();
}

bool UFlowBlackboardEntryValue_Enum::TryGetNumericalValuesForArithmeticOperation(int32* OutIntValue, float* OutFloatValue) const
{
	if (!IsValid(EnumValue.EnumClass))
	{
		return false;
	}

	const uint64 EnumValueAsInt = EnumValue.EnumClass->GetValueByName(EnumValue.Value);

	if (OutIntValue)
	{
		*OutIntValue = static_cast<int32>(EnumValueAsInt);
	}

	if (OutFloatValue)
	{
		*OutFloatValue = static_cast<float>(EnumValueAsInt);
	}

	return true;
}