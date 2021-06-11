// Copyright 2013 Tom SF Haines

// Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

//   http://www.apache.org/licenses/LICENSE-2.0

// Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.

#include "philox.h"

#include <stdlib.h>
#include <stdint.h>
#include <math.h>

// For C99...
#ifndef M_PI
#define M_PI 3.141592653589793238462643383279502884L
#endif


// Copyright 2013 Tom SF Haines

// Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

//   http://www.apache.org/licenses/LICENSE-2.0

// Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.



// Random number generator, designed to go directly from a sequence position to a random number - out is the counter on entry, the output when done...
void philox(unsigned int out[4]);



// Converts the output of philox (any of the 4 output unsigned ints) into a uniform draw in [0, 1)...
float uniform(unsigned int ui);



// Returns a draw from a standard normal distribution given two outputs from philox. You can optionally provide a pointer into which a second (independent) output is written...
float box_muller(unsigned int pa, unsigned int pb, float * second);



// Helper struct - given a pointer to 4 unisgned ints to index into the philox rng this generates random numbers, efficiently...
typedef struct PhiloxRNG PhiloxRNG;

struct PhiloxRNG
{
 unsigned int * index;
 unsigned int data[4];
 unsigned int next;
};



// Initialises the RNG with a pointer to an index, that decides what random number to get next - the index will be incrimented as its used up, so it can be used to initalise a future PhiloxRNG instance...
void PhiloxRNG_init(PhiloxRNG * this, unsigned int * index);

// Returns the next random number...
unsigned int PhiloxRNG_next(PhiloxRNG * this);

// Returns a uniform draw from the rng, in [0, 1)...
float PhiloxRNG_uniform(PhiloxRNG * this);

// Returns a standard Normal draw from the rng; can optionally output a second, which is independent of the first...
float PhiloxRNG_Gaussian(PhiloxRNG * this, float * second);

// Returns a draw from a Gamma distribution, where beta (the scale) is fixed to one...
float PhiloxRNG_UnscaledGamma(PhiloxRNG * this, float alpha);

// Returns a draw from a Gamma distribution...
float PhiloxRNG_Gamma(PhiloxRNG * this, float alpha, float beta);

// Returns a draw from a Beta distribution...
float PhiloxRNG_Beta(PhiloxRNG * this, float alpha, float beta);



static unsigned int mul_hi(unsigned int a, unsigned int b)
{
 uint64_t _a = a;
 uint64_t _b = b;

 return (_a * _b) >> 32;
}



void philox(unsigned int out[4])
{
 const unsigned int key[2] = {0x4edbf6fa, 0x6aa1107f}; // Normally a parameter, but I only need one hash per input.
 const unsigned int mult[2] = {0xCD9E8D57, 0xD2511F53};
 int rnd, i;

 // Iterate and do each round in turn, updating the counter before we finally return it (Indexing from 1 is conveniant for the Weyl sequence.)...
 for (rnd=1;rnd<=10;rnd++)
 {
  // Calculate key for this step, by applying the Weyl sequence on it...
   unsigned int keyWeyl[2];
   keyWeyl[0] = key[0] * rnd;
   keyWeyl[1] = key[1] * rnd;

  // Apply the s-blocks, also swap the r values between the s-blocks...
   unsigned int next[4];
   next[0] = out[1] * mult[0];
   next[2] = out[3] * mult[1];

   next[3] = mul_hi(out[1],mult[0]) ^ keyWeyl[0] ^ out[0];
   next[1] = mul_hi(out[3],mult[1]) ^ keyWeyl[1] ^ out[2];

  // Prepare for the next step...
   for (i=0;i<4;i++) out[i] = next[i];
 }
}



float uniform(unsigned int ui)
{
 return (float)ui / 4294967296.0;
}



float box_muller(unsigned int pa, unsigned int pb, float * second)
{
 float ra = uniform(pa);
 float rb = uniform(pb);

 float mult = sqrt(-2.0 * log(ra));
 float inner = 2 * M_PI * rb;

 if (second!=NULL) *second = mult * sin(inner);
 return mult * cos(inner);
}



void PhiloxRNG_init(PhiloxRNG * this, unsigned int * index)
{
 this->index = index;
 this->next = 4;
}


unsigned int PhiloxRNG_next(PhiloxRNG * this)
{
 if (this->next>3)
 {
  this->next = 0;
  // Copy the index into data...
   int i;
   for (i=0; i<4; i++) this->data[i] = this->index[i];

  // Move to the next index...
   this->index[3] += 1;
   if (this->index[3]==0)
   {
    this->index[2] += 1;
    if (this->index[2]==0)
    {
     this->index[1] += 1;
     if (this->index[1]==0)
     {
      this->index[0] += 1;
     }
    }
   }

  // Randomise the data with the Philox function...
   philox(this->data);
 }

 unsigned int ret = this->data[this->next];
 this->next += 1;
 return ret;
}


float PhiloxRNG_uniform(PhiloxRNG * this)
{
 return (float)PhiloxRNG_next(this) / 4294967296.0;
}


float PhiloxRNG_Gaussian(PhiloxRNG * this, float * second)
{
 float ra = (float)PhiloxRNG_next(this) / 4294967296.0;
 float rb = (float)PhiloxRNG_next(this) / 4294967296.0;

 float mult = sqrt(-2.0 * log(ra));
 float inner = 2 * M_PI * rb;

 if (second!=NULL) *second = mult * sin(inner);
 return mult * cos(inner);
}


float PhiloxRNG_UnscaledGamma(PhiloxRNG * this, float alpha)
{
 // Check if the alpha value is high enough to approximate via a normal distribution...
  if (alpha>32.0)
  {
   while (1)
   {
    float ret = alpha + sqrt(alpha) * PhiloxRNG_Gaussian(this, NULL);
    if (ret<0.0) continue;
    return ret;
   }
  }

 // If alpha is one, within tolerance, just use an exponential distribution...
  if (fabs(alpha-1.0)<1e-4)
  {
   return -log(1.0 - PhiloxRNG_uniform(this));
  }

 if (alpha>1.0)
 {
  // If alpha is 1 or greater use the Cheng/Feast method...
   while (1)
   {
    float u1 = PhiloxRNG_uniform(this);
    float u2 = PhiloxRNG_uniform(this);
    float v = ((alpha - 1.0/(6.0*alpha))*u1) / ((alpha-1.0)*u2);

    float lt2 = 2.0*(u2-1.0)/(alpha-1) + v + 1.0/v;
    if (lt2<=2.0)
    {
     return (alpha-1.0)*v;
    }

    float lt1 = 2.0*log(u2)/(alpha-1.0) - log(v) + v;
    if (lt1<=1.0)
    {
     return (alpha-1.0)*v;
    }
   }
 }
 else
 {
  // If alpha is less than 1 use a rejection sampling method...
   while (1)
   {
    float u1 = 1.0 - PhiloxRNG_uniform(this);
    float u2 = 1.0 - PhiloxRNG_uniform(this);
    float u3 = 1.0 - PhiloxRNG_uniform(this);

    float frac, point;
    if (u1 <= (M_E / (M_E + alpha)))
    {
     frac = pow(u2, 1.0/alpha);
     point = u3 * pow(frac, alpha-1.0);
    }
    else
    {
     frac = 1.0 - log(u2);
     point = u3 * exp(-frac);
    }

    if (point <= (pow(frac, alpha-1.0) * exp(-frac)))
    {
     return frac;
     break;
    }
   }
 }
}


float PhiloxRNG_Gamma(PhiloxRNG * this, float alpha, float beta)
{
 return PhiloxRNG_UnscaledGamma(this, alpha) / beta;
}


float PhiloxRNG_Beta(PhiloxRNG * this, float alpha, float beta)
{
 float g1 = PhiloxRNG_UnscaledGamma(this, alpha);
 float g2 = PhiloxRNG_UnscaledGamma(this, beta);

 return g1 / (g1 + g2);
}
