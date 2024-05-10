// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#pragma once

#include "FlowBlackboardEntryValue.h"

#include "FlowBlackboardEntryValue_Float.generated.h"

/**
 * Configuration object for setting blackboard entries for UBlackboardKeyType_Float entries
 */
UCLASS(BlueprintType, DisplayName = "Float Blackboard Value")
class AIFLOW_API UFlowBlackboardEntryValue_Float : public UFlowBlackboardEntryValue
{
	GENERATED_BODY()

public:

	//~Begin UFlowBlackboardEntryValue
	virtual void SetOnBlackboardComponent(UBlackboardComponent* BlackboardComponent) const override;
	virtual EBlackboardCompare::Type CompareKeyValues(const UBlackboardComponent* BlackboardComponent, const FName& OtherKeyName) const override;
	virtual TSubclassOf<UBlackboardKeyType> GetSupportedBlackboardKeyType() const override;
	virtual bool TryGetNumericalValuesForArithmeticOperation(int32* OutIntValue, float* OutFloatValue) const override;
#if WITH_EDITOR
	virtual bool SupportsArithmeticOperations() const override { return true; }
	virtual FString GetEditorValueString() const override;
#endif // WITH_EDITOR
	//~End UFlowBlackboardEntryValue

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
	float FloatValue = 0.0f;
};
