#include "MyBlueprintFunctionLibrary.h"
#include "Engine/Engine.h" // Needed for logging

void UMyBlueprintFunctionLibrary::CalculateCircumcenter(const FVector& A, const FVector& B, const FVector& C, FVector& OutCircumcenter)
{
    UE_LOG(LogTemp, Warning, TEXT("Calculating circumcenter for points: A(%f, %f, %f), B(%f, %f, %f), C(%f, %f, %f)"),
        A.X, A.Y, A.Z, B.X, B.Y, B.Z, C.X, C.Y, C.Z);

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

    UE_LOG(LogTemp, Warning, TEXT("Determinant: %f"), Determinant);

    if (FMath::Abs(Determinant) < KINDA_SMALL_NUMBER)
    {
        UE_LOG(LogTemp, Warning, TEXT("Points are collinear! Cannot calculate circumcenter."));
        OutCircumcenter = FVector::ZeroVector;
        return;
    }

    float T = ((Line2Point.X - Line1Point.X) * Line2Direction.Y - (Line2Point.Y - Line1Point.Y) * Line2Direction.X) / Determinant;
    OutCircumcenter = Line1Point + T * Line1Direction;

    UE_LOG(LogTemp, Warning, TEXT("Calculated circumcenter: (%f, %f, %f)"), OutCircumcenter.X, OutCircumcenter.Y, OutCircumcenter.Z);
}

void UMyBlueprintFunctionLibrary::ProcessTouchInputs(const TArray<FVector>& TouchPoints, FVector& OutMarker1, FVector& OutMarker2)
{
    if (TouchPoints.Num() < 3)
    {
        UE_LOG(LogTemp, Warning, TEXT("Not enough touch points detected! At least 3 required."));
        OutMarker1 = FVector::ZeroVector;
        OutMarker2 = FVector::ZeroVector;
        return;
    }

    UE_LOG(LogTemp, Warning, TEXT("Processing %d touch points..."), TouchPoints.Num());

    // Log touch points
    for (int32 i = 0; i < TouchPoints.Num(); i++)
    {
        UE_LOG(LogTemp, Warning, TEXT("TouchPoint %d: X=%f Y=%f Z=%f"), i, TouchPoints[i].X, TouchPoints[i].Y, TouchPoints[i].Z);
    }

    TArray<FVector> Group1, Group2;

    // Step 1: Group touch points based on proximity
    float DistanceThreshold = 50.0f; // Adjust as needed for your touch table

    for (const FVector& Touch : TouchPoints)
    {
        if (Group1.Num() == 0)
        {
            Group1.Add(Touch);
        }
        else
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

            if (bCloseToGroup1)
            {
                Group1.Add(Touch);
            }
            else
            {
                Group2.Add(Touch);
            }
        }
    }

    UE_LOG(LogTemp, Warning, TEXT("Group1 has %d points, Group2 has %d points"), Group1.Num(), Group2.Num());

    // Ensure Group1 has exactly 3 points
    if (Group1.Num() != 3)
    {
        UE_LOG(LogTemp, Warning, TEXT("Error grouping touch points! Group1 has %d points."), Group1.Num());
        OutMarker1 = FVector::ZeroVector;
        OutMarker2 = FVector::ZeroVector;
        return;
    }

    // Compute circumcenter for Group1
    CalculateCircumcenter(Group1[0], Group1[1], Group1[2], OutMarker1);

    // Handle Singleplayer Mode (only one marker detected)
    if (Group2.Num() < 3)
    {
        OutMarker2 = FVector::ZeroVector; // No second marker
        UE_LOG(LogTemp, Warning, TEXT("Singleplayer mode detected. Only one marker found."));
        return;
    }

    // Ensure Group2 has exactly 3 points
    if (Group2.Num() != 3)
    {
        UE_LOG(LogTemp, Warning, TEXT("Error grouping touch points! Group2 has %d points."), Group2.Num());
        OutMarker2 = FVector::ZeroVector;
        return;
    }

    // Compute circumcenter for Group2
    CalculateCircumcenter(Group2[0], Group2[1], Group2[2], OutMarker2);
}
