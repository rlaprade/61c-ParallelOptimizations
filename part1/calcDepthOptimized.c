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
		for (int x = 0; x < imageWidth; x++)
		{
			if ((y < featureHeight) || (y >= imageHeight - featureHeight) || (x < featureWidth) || (x >= imageWidth - featureWidth))
			{
				depth[row_index + x] = 0;
				continue;
			}

			float minimumSquaredDifference = -1;
			int minimumDy = 0;
			int minimumDx = 0;

			for (int dy = -maximumDisplacement; dy <= maximumDisplacement; dy++)
			{
                int y_plus_dy = y + dy;
				for (int dx = -maximumDisplacement; dx <= maximumDisplacement; dx++)
				{
                    int x_plus_dx = x + dx;
					if (y_plus_dy - featureHeight < 0 || y_plus_dy + featureHeight >= imageHeight || x_plus_dx - featureWidth < 0 || x_plus_dx + featureWidth >= imageWidth)
					{
						continue;
					}

					float squaredDifference = 0;
                    // int computable_width = 2*featureWidth+1 - (2*featureWidth+1 % 4);
                    // float sq_diff[(2*featureHeight+1)*computable_width];
                    // float *sq_diff_ptr = sq_diff;
                    // int ptr = 0;

                    int boxY = -featureHeight;
                    while (boxY <= featureHeight)
					{
                        int leftY = y + boxY;
                        int rightY = dy + leftY;  //removed 1 addition
                        int l_row_index = leftY * imageWidth;
                        int r_row_index = rightY * imageWidth;
                        int boxX = -featureWidth;
                        while (boxX <= featureWidth-3)
						{
							int leftX = x + boxX;
							int rightX = dx + leftX;  //removed 1 addition
                            
                            __m128 left_four = _mm_loadu_ps(left + l_row_index + leftX);
                            __m128 rite_four = _mm_loadu_ps(right + r_row_index + rightX);
                            __m128 diff = _mm_sub_ps(left_four, rite_four);
                            __m128 sq_diff_reg = _mm_mul_ps(diff, diff);
                            float sq_diff[4];
                            _mm_store_ps(sq_diff, sq_diff_reg);
                            for(int i=0; i<4; i++) {
                                squaredDifference += sq_diff[i];
                            }
                            
                            boxX+=4;
						}
                        while (boxX <= featureWidth) {
                            int leftX = x + boxX;
                            int rightX = dx + leftX;
                            float difference = left[leftY * imageWidth + leftX] - right[rightY * imageWidth + rightX];
                            squaredDifference += difference * difference;
                            boxX++;
                        }
                        // switch (featureWidth-boxX) {
                            // case 1:
                        
                            // case 2:
                            
                            // case 3:
                            
                        // }
                        
                        boxY++;
					}
                    // for(int i=0; i<(2*featureHeight+1)*computable_width; i++) {
                        // squaredDifference += sq_diff[i];
                    // }
                    

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
