// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#pragma once

#include "FlowBlackboardEntryValue.h"

#include "FlowBlackboardEntryValue_Rotator.generated.h"

/**
 * Configuration object for setting blackboard entries for UBlackboardKeyType_Rotator entries
 */
UCLASS(BlueprintType, DisplayName = "Rotator Blackboard Value")
class AIFLOW_API UFlowBlackboardEntryValue_Rotator : public UFlowBlackboardEntryValue
{
	GENERATED_BODY()

public:

	//~Begin UFlowBlackboardEntryValue
	virtual void SetOnBlackboardComponent(UBlackboardComponent* BlackboardComponent) const override;
	virtual EBlackboardCompare::Type CompareKeyValues(const UBlackboardComponent* BlackboardComponent, const FName& OtherKeyName) const override;
	virtual TSubclassOf<UBlackboardKeyType> GetSupportedBlackboardKeyType() const override;
	virtual bool TrySetValueFromInputDataPin(const FName& PinName, UFlowNode& PinOwnerFlowNode) override;
#if WITH_EDITOR
	virtual FString GetEditorValueString() const override { return RotatorValue.ToCompactString(); }
#endif // WITH_EDITOR
	//~End UFlowBlackboardEntryValue
	
	// IFlowDataPinPropertyProviderInterface
	virtual bool TryProvideFlowDataPinProperty(const bool bIsInputPin, TInstancedStruct<FFlowDataPinProperty>& OutFlowDataPinProperty) const override;
	// --

	virtual bool TryProvideFlowDataPinPropertyFromBlackboardEntry(
		const FName& BlackboardKeyName,
		const UBlackboardKeyType& BlackboardKeyType,
		UBlackboardComponent* OptionalBlackboardComponent,
		TInstancedStruct<FFlowDataPinProperty>& OutFlowDataPinProperty) const override;

#if WITH_EDITOR
public:
	//~Begin UFlowNodeBase
	virtual FText BuildNodeConfigText() const override;
	//~End UFlowNodeBase

	//~Begin UObject
	virtual void PostInitProperties() override;
	//~End UObject
#endif // WITH_EDITOR

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Configuration, meta = (DisplayAfter = Key))
	FRotator RotatorValue = FRotator::ZeroRotator;
};
