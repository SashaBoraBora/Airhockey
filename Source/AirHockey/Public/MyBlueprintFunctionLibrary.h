#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MyBlueprintFunctionLibrary.generated.h"

UCLASS()
class AIRHOCKEY_API UMyBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	/** Berechnet den Umkreismittelpunkt eines Dreiecks */
	UFUNCTION(BlueprintCallable, Category = "Math|Geometry")
	static void CalculateCircumcenter(const FVector& A, const FVector& B, const FVector& C, FVector& OutCircumcenter);
};
