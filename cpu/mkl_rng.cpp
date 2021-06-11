#include <stdio.h>
#include <stdint.h>
#include "mkl.h"
#include "mkl_vsl.h"

int main()
{
   uint32_t r[1000]; /* buffer for random numbers */
   double s; /* average */
   VSLStreamStatePtr stream;
   int i, j;

   /* Initializing */
   s = 0.0;
   vslNewStream( &stream, VSL_BRNG_SFMT19937, 777 );

   /* Generating */
   for ( i=0; i<10; i++ )
   {
      viRngUniformBits32( VSL_RNG_METHOD_UNIFORMBITS32_STD, stream, 1000, r);
      for ( j=0; j<1000; j++ );
      {
         s += r[j];
      }
   }
   s /= 10000.0;

   /* Deleting the stream */
   vslDeleteStream( &stream );

   /* Printing results */
   printf( "Sample mean of normal distribution = %f\n", s );

   return 0;
}