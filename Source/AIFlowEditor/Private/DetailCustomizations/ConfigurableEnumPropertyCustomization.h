// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#pragma once

#include "UnrealExtensions/IFlowCuratedNamePropertyCustomization.h"

#include "Types/ConfigurableEnumProperty.h"

// NOTE (gtaylor) this is nearly identical to FFlowDataPinOutputProperty_EnumCustomization, can we combine them?

// Details customization for FConfigurableEnumProperty
class FConfigurableEnumPropertyCustomization : public IFlowCuratedNamePropertyCustomization
{
private:
	typedef IFlowCuratedNamePropertyCustomization Super;

public:
	static TSharedRef<IPropertyTypeCustomization> MakeInstance() { return MakeShareable(new FConfigurableEnumPropertyCustomization()); }

protected:

	//~Begin IPropertyTypeCustomization
	virtual void CustomizeChildren(TSharedRef<IPropertyHandle> InStructPropertyHandle, IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;
	//~End IPropertyTypeCustomization

	//~Begin ICuratedNamePropertyCustomization
	virtual TSharedPtr<IPropertyHandle> GetCuratedNamePropertyHandle() const override;
	virtual void SetCuratedName(const FName& NewName) override;
	virtual bool TryGetCuratedName(FName& OutName) const override;
	virtual TArray<FName> GetCuratedNameOptions() const override;
	virtual bool AllowNameNoneIfOtherOptionsExist() const override { return false; }
	//~End ICuratedNamePropertyCustomization

	// Accessor to return the actual struct being edited
	FORCEINLINE FConfigurableEnumProperty* GetConfigurableEnumProperty() const
		{ return IFlowExtendedPropertyTypeCustomization::TryGetTypedStructValue<FConfigurableEnumProperty>(StructPropertyHandle); }
	
	void OnEnumNameChanged();

	const UEnum* GetEnumClass() const;

	static TArray<FName> GetEnumValues(const UEnum& Enum);
};
