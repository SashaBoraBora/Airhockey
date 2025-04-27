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

	TArray<FVector> Group1;
	TArray<FVector> Group2;
	float DistanceThreshold = 50.0f;

	// Improved grouping:
	for (const FVector& Touch : TouchPoints)
	{
		bool bAssigned = false;

		// Try to assign to Group1
		for (const FVector& P : Group1)
		{
			if (FVector::Dist(P, Touch) < DistanceThreshold)
			{
				Group1.Add(Touch);
				bAssigned = true;
				break;
			}
		}

		// Try Group2 if not assigned
		if (!bAssigned)
		{
			for (const FVector& P : Group2)
			{
				if (FVector::Dist(P, Touch) < DistanceThreshold)
				{
					Group2.Add(Touch);
					bAssigned = true;
					break;
				}
			}
		}

		// Start a new group if not assigned
		if (!bAssigned)
		{
			if (Group1.Num() == 0)
			{
				Group1.Add(Touch);
			}
			else if (Group2.Num() == 0)
			{
				Group2.Add(Touch);
			}
			else
			{
				// Too many groups!
				UE_LOG(LogTemp, Warning, TEXT("More than two paddle groups detected."));
			}
		}
	}

	// Find circumcenters
	FVector MarkerA = FVector::ZeroVector;
	FVector MarkerB = FVector::ZeroVector;
	bool bAValid = false;
	bool bBValid = false;

	if (Group1.Num() == 3)
	{
		UMyBlueprintFunctionLibrary::CalculateCircumcenter(Group1[0], Group1[1], Group1[2], MarkerA);
		bAValid = true;
	}
	if (Group2.Num() == 3)
	{
		UMyBlueprintFunctionLibrary::CalculateCircumcenter(Group2[0], Group2[1], Group2[2], MarkerB);
		bBValid = true;
	}

	// Assign based on distance to last known
	if (bAValid && bBValid)
	{
		float DistAto1 = FVector::Dist(MarkerA, LastKnownMarker1);
		float DistAto2 = FVector::Dist(MarkerA, LastKnownMarker2);
		float DistBto1 = FVector::Dist(MarkerB, LastKnownMarker1);
		float DistBto2 = FVector::Dist(MarkerB, LastKnownMarker2);

		if ((DistAto1 + DistBto2) <= (DistAto2 + DistBto1))
		{
			// A matches Marker1, B matches Marker2
			OutMarker1 = MarkerA;
			OutMarker2 = MarkerB;
		}
		else
		{
			// Swap
			OutMarker1 = MarkerB;
			OutMarker2 = MarkerA;
		}

		LastKnownMarker1 = OutMarker1;
		LastKnownMarker2 = OutMarker2;

		bIsMarker1Tracked = true;
		bIsMarker2Tracked = true;
	}
	else if (bAValid)
	{
		float DistToMarker1 = FVector::Dist(MarkerA, LastKnownMarker1);
		float DistToMarker2 = FVector::Dist(MarkerA, LastKnownMarker2);

		if (DistToMarker1 <= DistToMarker2)
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
		float DistToMarker1 = FVector::Dist(MarkerB, LastKnownMarker1);
		float DistToMarker2 = FVector::Dist(MarkerB, LastKnownMarker2);

		if (DistToMarker1 <= DistToMarker2)
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
