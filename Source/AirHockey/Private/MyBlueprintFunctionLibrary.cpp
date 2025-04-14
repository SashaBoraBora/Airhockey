#include "MyBlueprintFunctionLibrary.h"
#include "Engine/Engine.h"

// Static memory to store last known positions
static FVector LastKnownMarker1 = FVector::ZeroVector;
static FVector LastKnownMarker2 = FVector::ZeroVector;

void UMyBlueprintFunctionLibrary::CalculateCircumcenter(const FVector& A, const FVector& B, const FVector& C, FVector& OutCircumcenter)
{
	FVector MidAB = (A + B) * 0.5f;
	FVector MidBC = (B + C) * 0.5f;

	FVector AB = B - A;
	FVector BC = C - B;

	FVector TriangleNormal = FVector::CrossProduct(AB, BC).GetSafeNormal();

	FVector PerpAB = FVector::CrossProduct(AB, TriangleNormal).GetSafeNormal();
	FVector PerpBC = FVector::CrossProduct(BC, TriangleNormal).GetSafeNormal();

	FVector Line1Point = MidAB;
	FVector Line1Direction = PerpAB;
	FVector Line2Point = MidBC;
	FVector Line2Direction = PerpBC;

	float Determinant = Line1Direction.X * Line2Direction.Y - Line1Direction.Y * Line2Direction.X;

	if (FMath::Abs(Determinant) < KINDA_SMALL_NUMBER)
	{
		OutCircumcenter = FVector::ZeroVector;
		return;
	}

	float T = ((Line2Point.X - Line1Point.X) * Line2Direction.Y - (Line2Point.Y - Line1Point.Y) * Line2Direction.X) / Determinant;
	OutCircumcenter = Line1Point + T * Line1Direction;
}

void UMyBlueprintFunctionLibrary::ProcessTouchInputs(
	const TArray<FVector>& TouchPoints,
	FVector& OutMarker1,
	FVector& OutMarker2,
	bool& bIsMarker1Tracked,
	bool& bIsMarker2Tracked)
{
	// Reset tracking
	bIsMarker1Tracked = false;
	bIsMarker2Tracked = false;

	// Default to last known positions
	OutMarker1 = LastKnownMarker1;
	OutMarker2 = LastKnownMarker2;

	if (TouchPoints.Num() < 3)
	{
		UE_LOG(LogTemp, Warning, TEXT("Not enough touch points detected."));
		return;
	}

	TArray<FVector> Group1, Group2;
	float DistanceThreshold = 50.0f;

	// Group points
	for (const FVector& Touch : TouchPoints)
	{
		bool bCloseToGroup1 = false;
		for (const FVector& P : Group1)
		{
			if (FVector::Dist(P, Touch) < DistanceThreshold)
			{
				bCloseToGroup1 = true;
				break;
			}
		}

		if (bCloseToGroup1 || Group1.Num() == 0)
		{
			Group1.Add(Touch);
		}
		else
		{
			Group2.Add(Touch);
		}
	}

	FVector MarkerA = FVector::ZeroVector;
	FVector MarkerB = FVector::ZeroVector;
	bool bAValid = false;
	bool bBValid = false;

	// Compute for Group1
	if (Group1.Num() == 3)
	{
		UMyBlueprintFunctionLibrary::CalculateCircumcenter(Group1[0], Group1[1], Group1[2], MarkerA);
		bAValid = true;
	}
	// Compute for Group2
	if (Group2.Num() == 3)
	{
		UMyBlueprintFunctionLibrary::CalculateCircumcenter(Group2[0], Group2[1], Group2[2], MarkerB);
		bBValid = true;
	}

	// Assign based on Y axis
	if (bAValid && bBValid)
	{
		if (MarkerA.Y >= MarkerB.Y)
		{
			OutMarker1 = MarkerA;
			OutMarker2 = MarkerB;
			LastKnownMarker1 = MarkerA;
			LastKnownMarker2 = MarkerB;
		}
		else
		{
			OutMarker1 = MarkerB;
			OutMarker2 = MarkerA;
			LastKnownMarker1 = MarkerB;
			LastKnownMarker2 = MarkerA;
		}
		bIsMarker1Tracked = true;
		bIsMarker2Tracked = true;
	}
	else if (bAValid)
	{
		if (MarkerA.Y >= 0)
		{
			OutMarker1 = MarkerA;
			LastKnownMarker1 = MarkerA;
			bIsMarker1Tracked = true;
		}
		else
		{
			OutMarker2 = MarkerA;
			LastKnownMarker2 = MarkerA;
			bIsMarker2Tracked = true;
		}
	}
	else if (bBValid)
	{
		if (MarkerB.Y >= 0)
		{
			OutMarker1 = MarkerB;
			LastKnownMarker1 = MarkerB;
			bIsMarker1Tracked = true;
		}
		else
		{
			OutMarker2 = MarkerB;
			LastKnownMarker2 = MarkerB;
			bIsMarker2Tracked = true;
		}
	}
}
