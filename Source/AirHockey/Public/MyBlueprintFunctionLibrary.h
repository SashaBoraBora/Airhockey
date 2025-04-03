#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MyBlueprintFunctionLibrary.generated.h"

UCLASS()
class AIRHOCKEY_API UMyBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	/** Calculates the circumcenter of a triangle */
	UFUNCTION(BlueprintCallable, Category = "Math|Geometry")
	static void CalculateCircumcenter(const FVector& A, const FVector& B, const FVector& C, FVector& OutCircumcenter);

	/** Processes touch inputs to detect air hockey paddle positions */
	UFUNCTION(BlueprintCallable, Category = "Math|Geometry")
	static void ProcessTouchInputs(const TArray<FVector>& TouchPoints, FVector& OutMarker1, FVector& OutMarker2);
};
