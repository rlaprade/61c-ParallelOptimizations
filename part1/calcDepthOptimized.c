// CS 61C Fall 2014 Project 3

// include SSE intrinsics
#if defined(_MSC_VER)
#include <intrin.h>
#elif defined(__GNUC__) && (defined(__x86_64__) || defined(__i386__))
#include <x86intrin.h>
#endif

#include "calcDepthOptimized.h"
#include "calcDepthNaive.h"

#define ABS(x) (((x) < 0) ? (-(x)) : (x))

float displacement(int dx, int dy)
{
	float squaredDisplacement = dx * dx + dy * dy;
	float displacement = sqrt(squaredDisplacement);
	return displacement;
}

void calcDepthOptimized(float *depth, float *left, float *right, int imageWidth, int imageHeight, int featureWidth, int featureHeight, int maximumDisplacement)
{
    int row_index = 0;
	for (int y = 0; y < imageHeight; y++)
	{
		for (int x = 0; x < featureWidth; x++) {
            depth[row_index + x] = 0;
        }
        for (int x = imageWidth - featureWidth; x < imageWidth; x++) {
            depth[row_index + x] = 0;
        }
		for (int x = featureWidth; x < imageWidth - featureWidth; x++)
		{
			if ((y < featureHeight) || (y >= imageHeight - featureHeight))
			{
				depth[row_index + x] = 0;
				continue;
			}

			float minimumSquaredDifference = -1;
			int minimumDy = 0;
			int minimumDx = 0;

			for (int dy = -maximumDisplacement; dy <= maximumDisplacement; dy++)
			{
				for (int dx = -maximumDisplacement; dx <= maximumDisplacement; dx++)
				{
					if (y + dy - featureHeight < 0 || y + dy + featureHeight >= imageHeight || x + dx - featureWidth < 0 || x + dx + featureWidth >= imageWidth)
					{
						continue;
					}

					float squaredDifference = 0;

					for (int boxY = -featureHeight; boxY <= featureHeight; boxY++)
					{
						for (int boxX = -featureWidth; boxX <= featureWidth; boxX++)
						{
							int leftX = x + boxX;
							int leftY = y + boxY;
							int rightX = dx + leftX;  //removed 1 addition
							int rightY = dy + leftY;  //removed 1 addition

							float difference = left[leftY * imageWidth + leftX] - right[rightY * imageWidth + rightX];
							squaredDifference += difference * difference;

						}
					}

					if ((minimumSquaredDifference == -1) || ((minimumSquaredDifference == squaredDifference) && (displacement(dx, dy) < displacement(minimumDx, minimumDy))) || (minimumSquaredDifference > squaredDifference))
					{
						minimumSquaredDifference = squaredDifference;
						minimumDx = dx;
						minimumDy = dy;
					}
				}
			}

			if (minimumSquaredDifference != -1)
			{
				if (maximumDisplacement == 0)
				{
					depth[row_index + x] = 0;
				}
				else
				{
					depth[row_index + x] = displacement(minimumDx, minimumDy);
				}
			}
			else
			{
				depth[row_index + x] = 0;
			}
		}
        row_index += imageWidth; //INVARIANT: row_index = y * imageWidth 
	}
    
}
