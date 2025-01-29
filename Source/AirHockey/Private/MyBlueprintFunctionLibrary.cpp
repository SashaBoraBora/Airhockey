#include "MyBlueprintFunctionLibrary.h"

void UMyBlueprintFunctionLibrary::CalculateCircumcenter(const FVector& A, const FVector& B, const FVector& C, FVector& OutCircumcenter)
{
	// Compute the midpoints of AB and BC
	FVector MidAB = (A + B) * 0.5f;
	FVector MidBC = (B + C) * 0.5f;

	// Compute perpendicular vectors to AB and BC
	FVector AB = B - A;
	FVector BC = C - B;

	// Compute normal to the triangle plane
	FVector TriangleNormal = FVector::CrossProduct(AB, BC).GetSafeNormal();

	// Compute perpendicular vectors to AB and BC in the triangle's plane
	FVector PerpAB = FVector::CrossProduct(AB, TriangleNormal).GetSafeNormal();
	FVector PerpBC = FVector::CrossProduct(BC, TriangleNormal).GetSafeNormal();

	// Solve for the intersection of the perpendicular bisectors
	FVector Line1Point = MidAB;
	FVector Line1Direction = PerpAB;
	FVector Line2Point = MidBC;
	FVector Line2Direction = PerpBC;

	// Solve for intersection using a determinant method
	float Determinant = Line1Direction.X * Line2Direction.Y - Line1Direction.Y * Line2Direction.X;

	if (FMath::Abs(Determinant) < KINDA_SMALL_NUMBER)
	{
		// Falls die Punkte auf einer Linie liegen, gibt es keinen eindeutigen Umkreismittelpunkt
		OutCircumcenter = FVector::ZeroVector;
		return;
	}

	// Berechnung des Schnittpunkts der Mittelsenkrechten
	float T = ((Line2Point.X - Line1Point.X) * Line2Direction.Y - (Line2Point.Y - Line1Point.Y) * Line2Direction.X) / Determinant;
	OutCircumcenter = Line1Point + T * Line1Direction;
}
