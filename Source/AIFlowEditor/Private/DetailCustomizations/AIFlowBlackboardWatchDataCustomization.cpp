// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#include "AIFlowBlackboardWatchDataCustomization.h"
#include "Blackboard/AIFlowBlackboardWatchData.h"

#include "DetailLayoutBuilder.h"
#include "DetailCategoryBuilder.h"
#include "PropertyHandle.h"
#include "DetailWidgetRow.h"
#include "Widgets/Text/STextBlock.h"

#include "Types/FlowDataPinValue.h"
#include "Types/FlowNamedDataPinProperty.h"

TSharedRef<IDetailCustomization> FAIFlowBlackboardWatchDataCustomization::MakeInstance()
{
	return MakeShareable(new FAIFlowBlackboardWatchDataCustomization());
}

void FAIFlowBlackboardWatchDataCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	// Get the NamedProperties array on UAIFlowBlackboardWatchData
	TSharedRef<IPropertyHandle> NamedPropertiesHandle =
		DetailBuilder.GetProperty(
			GET_MEMBER_NAME_CHECKED(UAIFlowBlackboardWatchData, NamedProperties),
			UAIFlowBlackboardWatchData::StaticClass());

	if (!NamedPropertiesHandle->IsValidHandle())
	{
		return;
	}

	// Determine the watch data object and display mode
	UAIFlowBlackboardWatchData* WatchData = nullptr;
	EAIFlowBlackboardDisplayMode DisplayMode = EAIFlowBlackboardDisplayMode::DisplayEntriesAsText;

	{
		TArray<TWeakObjectPtr<UObject>> CustomizedObjects;
		DetailBuilder.GetObjectsBeingCustomized(CustomizedObjects);

		if (CustomizedObjects.Num() > 0)
		{
			WatchData = Cast<UAIFlowBlackboardWatchData>(CustomizedObjects[0].Get());
			if (WatchData)
			{
				DisplayMode = WatchData->DisplayMode;
			}
		}
	}

	// Hide the default array row; we'll present the contents manually.
	DetailBuilder.HideProperty(NamedPropertiesHandle);

	// Put our entries into the same category as the property.
	const FName CategoryName = *NamedPropertiesHandle->GetDefaultCategoryName().ToString();
	IDetailCategoryBuilder& Category = DetailBuilder.EditCategory(CategoryName, FText::GetEmpty(), ECategoryPriority::Default);

	// Treat NamedProperties as an array (for handles), but use the UObject data to access FFlowNamedDataPinProperty.
	TSharedPtr<IPropertyHandleArray> ArrayHandle = NamedPropertiesHandle->AsArray();
	if (!ArrayHandle.IsValid())
	{
		return;
	}

	uint32 NumElements = 0;
	ArrayHandle->GetNumElements(NumElements);

	for (uint32 Index = 0; Index < NumElements; ++Index)
	{
		TSharedPtr<IPropertyHandle> ElementHandle = ArrayHandle->GetElement(Index);
		if (!ElementHandle.IsValid())
		{
			continue;
		}

		// Each element is an FFlowNamedDataPinProperty struct.
		TSharedPtr<IPropertyHandle> NameHandle = ElementHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FFlowNamedDataPinProperty, Name));
		TSharedPtr<IPropertyHandle> DataPinValueHandle = ElementHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FFlowNamedDataPinProperty, DataPinValue));

		if (!NameHandle.IsValid() || !DataPinValueHandle.IsValid())
		{
			// Fallback: if something is wrong, just show the whole element as a normal property.
			IDetailPropertyRow& FallbackRow = Category.AddProperty(ElementHandle.ToSharedRef());
			FallbackRow.ShouldAutoExpand(true);
			continue;
		}

		// Compute the row display name from the Name property
		FText NameText;

		FString NameString;
		if (NameHandle->GetValueAsDisplayString(NameString) == FPropertyAccess::Success)
		{
			NameText = FText::FromString(NameString);
		}

		// Safely fetch the corresponding FFlowNamedDataPinProperty from the UObject
		const FFlowNamedDataPinProperty* NamedPropertyPtr = nullptr;
		if (WatchData && WatchData->NamedProperties.IsValidIndex(static_cast<int32>(Index)))
		{
			NamedPropertyPtr = &WatchData->NamedProperties[static_cast<int32>(Index)];
		}

		if (DisplayMode == EAIFlowBlackboardDisplayMode::DisplayEntriesAsText)
		{
			// Text mode: evaluate FFlowDataPinValue via TryConvertValuesToString
			FText ValueText = FText::FromString(TEXT("(no text representation)"));

			if (NamedPropertyPtr)
			{
				const TInstancedStruct<FFlowDataPinValue>& InstancedValue = NamedPropertyPtr->DataPinValue;
				if (const FFlowDataPinValue* ValuePtr = InstancedValue.GetPtr())
				{
					FString Stringified;
					if (ValuePtr->TryConvertValuesToString(Stringified))
					{
						ValueText = FText::FromString(Stringified);
					}
				}
			}

			Category.AddCustomRow(NameText)
				.NameContent()
				[
					SNew(STextBlock)
						.Text(NameText)
				]
				.ValueContent()
				.MinDesiredWidth(250.0f)
				.MaxDesiredWidth(600.0f)
				[
					SNew(STextBlock)
						.Text(ValueText)
						.AutoWrapText(true)
						.WrappingPolicy(ETextWrappingPolicy::AllowPerCharacterWrapping)
				];
		}
		else
		{
			// FlowPinValues mode: use the existing DataPinValue property editor
			Category.AddProperty(DataPinValueHandle.ToSharedRef())
				.DisplayName(NameText);
		}
	}
}