#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MyBlueprintFunctionLibrary.generated.h"

UCLASS()
class AIRHOCKEY_API UMyBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "Math|Geometry")
	static void CalculateCircumcenter(const FVector& A, const FVector& B, const FVector& C, FVector& OutCircumcenter);

	UFUNCTION(BlueprintCallable, Category = "TouchProcessing")
	static void ProcessTouchInputs(
		const TArray<FVector>& TouchPoints,
		FVector& OutMarker1,
		FVector& OutMarker2,
		bool& bIsMarker1Tracked,
		bool& bIsMarker2Tracked
	);
};
