// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#include "ConfigurableEnumPropertyCustomization.h"
#include "Types/ConfigurableEnumProperty.h"

#include "IDetailChildrenBuilder.h"
#include "UObject/UnrealType.h"

// FConfigurableEnumPropertyCustomization Implementation

void FConfigurableEnumPropertyCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> InStructPropertyHandle, IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	if (TSharedPtr<IPropertyHandle> EnumClassHandle = InStructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FConfigurableEnumProperty, EnumClass)))
	{
		StructBuilder.AddProperty(EnumClassHandle.ToSharedRef());
	}

	if (TSharedPtr<IPropertyHandle> EnumNameHandle = InStructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FConfigurableEnumProperty, EnumName)))
	{
		StructBuilder.AddProperty(EnumNameHandle.ToSharedRef());
	}
}

TSharedPtr<IPropertyHandle> FConfigurableEnumPropertyCustomization::GetCuratedNamePropertyHandle() const
{
	check(StructPropertyHandle->IsValidHandle());

	TSharedPtr<IPropertyHandle> FoundHandle = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FConfigurableEnumProperty, Value));
	check(FoundHandle);

	return FoundHandle;
}

TArray<FName> FConfigurableEnumPropertyCustomization::GetCuratedNameOptions() const
{
	TArray<FName> Results;

	const UEnum* Enum = GetEnumClass();

	if (IsValid(Enum))
	{
		Results = GetEnumValues(*Enum);
	}

	return Results;
}

TArray<FName> FConfigurableEnumPropertyCustomization::GetEnumValues(const UEnum& Enum)
{
	TArray<FName> EnumValues;

	for (int Index = 0; Index < Enum.GetMaxEnumValue(); Index++)
	{
		if (!Enum.IsValidEnumValue(Index))
		{
			continue;
		}

		static const TCHAR* MetaDataKey_Hidden = TEXT("Hidden");
		if (!Enum.HasMetaData(MetaDataKey_Hidden, Index))
		{
			EnumValues.Add(*Enum.GetDisplayNameTextByIndex(Index).ToString());
		}
	}

	return EnumValues;
}

void FConfigurableEnumPropertyCustomization::SetCuratedName(const FName& NewValue)
{
	TSharedPtr<IPropertyHandle> ValueHandle = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FConfigurableEnumProperty, Value));

	check(ValueHandle);

	ValueHandle->SetPerObjectValue(0, NewValue.ToString());
}

bool FConfigurableEnumPropertyCustomization::TryGetCuratedName(FName& OutName) const
{
	const FConfigurableEnumProperty* ConfigurableEnumProperty = GetConfigurableEnumProperty();
	if (ConfigurableEnumProperty)
	{
		OutName = ConfigurableEnumProperty->Value;

		return true;
	}
	else
	{
		return false;
	}
}

const UEnum* FConfigurableEnumPropertyCustomization::GetEnumClass() const
{
	const FConfigurableEnumProperty* ConfigurableEnumProperty = GetConfigurableEnumProperty();
	if (!ConfigurableEnumProperty)
	{
		return nullptr;
	}

	return ConfigurableEnumProperty->EnumClass;
}
