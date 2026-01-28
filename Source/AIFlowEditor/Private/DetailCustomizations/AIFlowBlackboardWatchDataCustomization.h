// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#pragma once

#include "IDetailCustomization.h"

/**
* Details customization for UAIFlowBlackboardWatchData.
*
* For each NamedProperties element (FFlowNamedDataPinProperty), creates a row
* whose display name is the property's Name and whose value content depends
* on the selected display mode:
*   - DisplayEntriesAsText: a plain text representation derived from
*       FFlowDataPinValue::TryConvertValuesToString
*   - DisplayEntriesAsFlowPinValues: the DataPinValue instanced struct
*       (default property editor)
*/
class FAIFlowBlackboardWatchDataCustomization : public IDetailCustomization
{
public:
	static TSharedRef<IDetailCustomization> MakeInstance();

	// IDetailCustomization interface
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;
};