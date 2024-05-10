// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#pragma once

#include "Types/ConfigurableEnumProperty.h"
#include "FlowBlackboardEntryValue.h"

#include "FlowBlackboardEntryValue_Enum.generated.h"

/**
 * Configuration object class for setting blackboard entries for UBlackboardKeyType_Enum
 */
UCLASS(BlueprintType, DisplayName = "Enum Blackboard Value")
class UFlowBlackboardEntryValue_Enum : public UFlowBlackboardEntryValue
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
	virtual bool TryReconfigureFromBlackboardKeyType(const UBlackboardKeyType& KeyType) override;
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
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	//~End UObject

	void EnsureValueIsCompatibleWithEnumClass();
#endif // WITH_EDITOR

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Configuration)
	FConfigurableEnumProperty EnumValue;
};