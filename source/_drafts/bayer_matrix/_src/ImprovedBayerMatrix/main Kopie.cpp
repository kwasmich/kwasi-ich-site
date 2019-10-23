#include <iostream>
#include <limits>

typedef	int pos[2];

inline unsigned int spread( const unsigned short in_X ) {
	unsigned int x = in_X;
	x = ( x | ( x << 8 ) ) & 0x00FF00FF;
	x = ( x | ( x << 4 ) ) & 0x0F0F0F0F;
	x = ( x | ( x << 2 ) ) & 0x33333333;
	x = ( x | ( x << 1 ) ) & 0x55555555;
	return x;
}

inline unsigned short unspread( const unsigned int in_X ) {
	unsigned int x = in_X;
	x = ( x | ( x >> 1 ) ) & 0x33333333;
	x = ( x | ( x >> 2 ) ) & 0x0F0F0F0F;
	x = ( x | ( x >> 4 ) ) & 0x00FF00FF;
	x = ( x | ( x >> 8 ) ) & 0x0000FFFF;
	return x;
}

inline unsigned int zOrder( const unsigned short in_X, const unsigned short in_Y ) {
	unsigned int x = spread( in_X );
	unsigned int y = spread( in_Y );
	return x | ( y << 1 );
}

inline void zUnorder( const unsigned int in_Z, unsigned short & out_X, unsigned short & out_Y ) {
	unsigned int x = unspread( in_Z & 0x55555555 );
	unsigned int y = unspread( ( in_Z >> 1 ) & 0x55555555 );
	out_X = x;
	out_Y = y;
}

int main (int argc, char * const argv[]) {
	
	for ( int y = 0; y < 16; y++ ) {
		for ( int x = 0; x < 16; x++ ) {
			int z = zOrder( x, y );
			unsigned short xx = 0;
			unsigned short yy = 0;
			zUnorder( z, xx, yy );
			printf( "%3u ", z );
		}
		printf( "\n" );
	}
	
	for ( int i = 0; i < 100; i++ ) {
		int a = spread(i);
		int b = unspread(a);
		printf( "%i %i %i\n", i, a, b );
	}
	
	
	return 0;
	
	const int d = 4;
	int * array = new int[d * d];
	pos * position = new pos[d * d];
	unsigned int * distances = new unsigned int[d * d];
	
	for ( int i = 0; i < d * d; i++ ) {
		array[i] = 0;
	}
	
	array[0] = 1;
	position[0][0] = 0;
	position[0][1] = 0;
	
	for ( int i  = 1; i < d * d; i++ ) {
		unsigned int maxDistance = 0;
		unsigned int distance = 0;
		pos maxDistancePos = { 0, 0 };
		
		printf("%i:\n", i);
		
		for ( int y = 0; y < d; y++ ) {
			for ( int j = 0; j < i; j++ ) {
				for ( int x = 0; x < d; x++ ) {
					if (array[x+y*d] > 0) {
						distance = 0;
					} else {
						//printf("%i %i\n", x, y );
						int dxAbs = abs(x - position[j][0]);
						int dyAbs = abs(y - position[j][1]);
						int dx = std::min( dxAbs, d - dxAbs);
						int dy = std::min( dyAbs, d - dyAbs);
						distance = dx * dx + dy * dy;
						//array[x+y*d] = distance;
					}
					
//					if (distance > maxDistance) {
//						maxDistance = distance;
//						maxDistancePos[0] = x;
//						maxDistancePos[1] = y;
//					}
					
					printf("%2i ", distance);
				}
				
				printf("  ");
			}
			
			for (int x = 0; x < d; x++) {
				distance = 0;

				if (array[x+y*d] == 0) {
					unsigned int minDistance = 2000000000;
					const unsigned int dd2 = d*d/2;
					unsigned int factor = 1;
					
					for ( int j = 0; j < i; j++ ) {
						int dxAbs = abs(x - position[j][0]);
						int dyAbs = abs(y - position[j][1]);
						int dx = std::min( dxAbs, d - dxAbs);
						int dy = std::min( dyAbs, d - dyAbs);
						distances[j] = dx * dx + dy * dy;
						//minDistance = std::min( minDistance, distances[j] );
					}
										
//					for ( int j = 0; j < i; j++ ) {
//						printf("%i*%i ", distances[j], factor);
//						distance += distances[j] * factor;
//						factor *= dd2;
//					}
//					
//					printf("%i*%i ", minDistance, factor);
//					distance += minDistance * factor;
					
					
					
					for ( int j = i - 1; j >= 0; j-- ) {
						minDistance = std::min( distances[j], minDistance );
						distance += minDistance * factor;
						factor *= dd2;
					}
					
//					int dxAbs = abs(x - position[i-1][0]);
//					int dyAbs = abs(y - position[i-1][1]);
//					int dx = std::min( dxAbs, d - dxAbs);
//					int dy = std::min( dyAbs, d - dyAbs);
//					int distance1 = dx * dx + dy * dy;
//					
//					if ( i > 1 ) {
//						dxAbs = abs(x - position[i-2][0]);
//						dyAbs = abs(y - position[i-2][1]);
//						dx = std::min( dxAbs, d - dxAbs);
//						dy = std::min( dyAbs, d - dyAbs);
//						int distance2 = dx * dx + dy * dy;
//						
//						if ( i > 2 ) {
//							dxAbs = abs(x - position[i-3][0]);
//							dyAbs = abs(y - position[i-3][1]);
//							dx = std::min( dxAbs, d - dxAbs);
//							dy = std::min( dyAbs, d - dyAbs);
//							int distance3 = dx * dx + dy * dy;
//							
//							if ( i > 3 ) {
//								dxAbs = abs(x - position[i-4][0]);
//								dyAbs = abs(y - position[i-4][1]);
//								dx = std::min( dxAbs, d - dxAbs);
//								dy = std::min( dyAbs, d - dyAbs);
//								int distance4 = dx * dx + dy * dy;
//								
//								distance = std::min(std::min(distance1, distance2), std::min(distance3, distance4)) * d*d/2 * d*d/2 * d*d/2 + distance1 * d*d/2 * d*d/2 + distance2 * d*d/2 + distance3;
//							} else {
//								distance = std::min(std::min(distance1, distance2), distance3) * d*d/2 * d*d/2 + distance1 * d*d/2 + distance2;
//							}
//						} else {
//							distance = std::min(distance1, distance2) * d*d/2 + distance1;
//						}
//					} else {
//						distance = distance1;
//					}
				}
				
				if (distance > maxDistance) {
					maxDistance = distance;
					maxDistancePos[0] = x;
					maxDistancePos[1] = y;
				}
				
				printf("%6u ", distance);
			}
			
			printf("\n");
		}
		
		printf("\n");
		
		array[maxDistancePos[0]+maxDistancePos[1]*d] = i + 1;
		position[i][0] = maxDistancePos[0];
		position[i][1] = maxDistancePos[1];
	}
	
	for (int y = 0; y < d; y++) {
		for (int x = 0; x < d; x++) {
			printf("%2i ", array[x+y*d]);
		}
		
		printf("\n");
	}
	
	printf("\n");
	
    // insert code here...
    std::cout << "Hello, World!\n";
    return 0;
}

//# use backtracking, win : biggest sum (careful of scaling: first entries have smaller factors)
//# use z curve