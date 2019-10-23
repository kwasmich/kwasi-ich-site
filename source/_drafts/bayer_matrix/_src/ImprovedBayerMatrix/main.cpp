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

inline unsigned int dist( const unsigned int in_X, const unsigned int in_Y ) {
	return in_X * in_X + in_Y * in_Y;
}

int main (int argc, char * const argv[]) {
	
	
//	for ( int y = 0; y < 16; y++ ) {
//		for ( int x = 0; x < 16; x++ ) {
//			int z = zOrder( x, y );
//			unsigned short xx = 0;
//			unsigned short yy = 0;
//			zUnorder( z, xx, yy );
//			printf( "%3u ", z );
//		}
//		printf( "\n" );
//	}
//	
//	for ( int i = 0; i < 100; i++ ) {
//		int a = spread(i);
//		int b = unspread(a);
//		printf( "%i %i %i\n", i, a, b );
//	}
//	
//	return 0;
	
	const int d = 4;
	const unsigned int dd2 = d*d/2;
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
		
		for ( unsigned int z = 0; z < d * d; z++ ) {
			unsigned short x = 0;
			unsigned short y = 0;
			zUnorder( z, x, y );
			
			distance = 0;
			
			if (array[x+y*d] == 0) {
				unsigned int minDistance = 2000000000;
				unsigned int factor = 1;
				
				for ( int j = 0; j < i; j++ ) {
					int dxAbs = abs(x - position[j][0]);
					int dyAbs = abs(y - position[j][1]);
					int dx = std::min( dxAbs, d - dxAbs);
					int dy = std::min( dyAbs, d - dyAbs);
					distances[j] = dist( dx, dy );
				}				
				
				if ( distances[i - 1] == dd2 ) {
					factor *= dd2;
					distance = UINT_MAX;
				} else {
					for ( int j = i - 1; j >= 0; j-- ) {
						minDistance = std::min( distances[j], minDistance );
						distance += minDistance * factor;
						factor *= dd2;
					}
				}
			}
			
			if (distance > maxDistance) {
				maxDistance = distance;
				maxDistancePos[0] = x;
				maxDistancePos[1] = y;
			}
		}
		
		
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
						distance = dist( dx, dy );
					}
					
					printf("%2i ", distance);
				}
				
				printf("  ");
			}
			
			for (int x = 0; x < d; x++) {
				distance = 0;

				if (array[x+y*d] == 0) {
					unsigned int minDistance = 2000000000;
					unsigned int factor = 1;
					
					for ( int j = 0; j < i; j++ ) {
						int dxAbs = abs(x - position[j][0]);
						int dyAbs = abs(y - position[j][1]);
						int dx = std::min( dxAbs, d - dxAbs);
						int dy = std::min( dyAbs, d - dyAbs);
						distances[j] = dist( dx, dy );
					}
										
					if ( distances[i - 1] == dd2 ) {
						distance = UINT_MAX;
					} else {
						for ( int j = i - 1; j >= 0; j-- ) {
							minDistance = std::min( distances[j], minDistance );
							distance += minDistance * factor;
							factor *= dd2;
						}
					}
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