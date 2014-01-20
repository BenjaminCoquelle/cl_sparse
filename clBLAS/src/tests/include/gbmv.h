/* ************************************************************************
 * Copyright 2013 Advanced Micro Devices, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * ************************************************************************/


#ifndef GBMV_H_
#define GBMV_H_

#include <gtest/gtest.h>
#include <clBLAS.h>
#include <common.h>
#include <BlasBase.h>
#include <ExtraTestSizes.h>
#include <blas-random.h>
#include <blas-math.h>

using namespace clMath;
using ::testing::TestWithParam;

class GBMV : public TestWithParam<
    ::std::tr1::tuple<
        clblasOrder,     // order
        clblasTranspose, // transA
        int,                // M
        int,                // N
        int,                // KL
        int,                // KU
        ExtraTestSizes,
        ComplexLong,		// Alpha
		ComplexLong, 		// Beta
        int                 // numCommandQueues
        > > {
public:
    void getParams(TestParams *params)
    {
        memset(params, 0, sizeof(TestParams));

        params->order = order;
        params->transA = transA;
        params->seed = seed;
        params->M = M;
        params->N = N;
        params->KL = KL;
        params->KU = KU;
        params->lda = lda;
        params->incx = incx;
        params->incy = incy;
        params->offA = offA;
        params->offa = offA;
        params->offBX = offx;
        params->offCY = offy;
        params->alpha = paramAlpha;
        params->beta = paramBeta;
        params->numCommandQueues = numCommandQueues;
    }

protected:
    virtual void SetUp()
    {
        ExtraTestSizes extra;

        order = ::std::tr1::get<0>(GetParam());
        transA = ::std::tr1::get<1>(GetParam());
        M = ::std::tr1::get<2>(GetParam());
        N = ::std::tr1::get<3>(GetParam());
        KL = ::std::tr1::get<4>(GetParam());
        KU = ::std::tr1::get<5>(GetParam());
        extra = ::std::tr1::get<6>(GetParam());
        offA = extra.offA;
        offx = extra.offBX;
        offy = extra.offCY;
        lda = extra.strideA.ld;
        incx = extra.strideBX.inc;
        incy = extra.strideCY.inc;
        paramAlpha = ::std::tr1::get<7>(GetParam());
		paramBeta  = ::std::tr1::get<8>(GetParam());
        numCommandQueues = ::std::tr1::get<9>(GetParam());

        base = ::clMath::BlasBase::getInstance();
        seed = base->seed();

        useNumCommandQueues = base->useNumCommandQueues();
        if (useNumCommandQueues) {
            numCommandQueues = base->numCommandQueues();
        }

        KL = KL % M;
        KU = KU % N;
        lda = ::std::max(lda, (KL+KU+1));

        printTestParams(order, transA, M, N, KL, KU, paramAlpha, offA,
                            lda, offx, incx, paramBeta, offy, incy);
        ::std::cerr << "seed = " << seed << ::std::endl;
        ::std::cerr << "queues = " << numCommandQueues << ::std::endl;
    }

    clblasOrder order;
    clblasTranspose transA;
    size_t M, N, KL, KU;
    size_t lda;
    int incx, incy;
    size_t offA, offx, offy;
    unsigned int seed;

    ComplexLong paramAlpha, paramBeta;
    ::clMath::BlasBase *base;

    bool useNumCommandQueues;
    cl_uint numCommandQueues;
};

template <typename T>
static void
randomGbmvMatrices(
    clblasOrder order,
    clblasTranspose trans,
    size_t M,
    size_t N,
    T *alpha,
    T *beta,
    T *A,
    size_t lda,
    T *X,
	int incx,
	T *Y,
	int incy
    )
{
    size_t i;
	size_t lenX, lenY, lenA;
    cl_double bound, maxAB, maxMN;

	// bound is calculated by solving the equation (alpha*x^2 + x - UPPER_BOUND) < 0
	bound = UPPER_BOUND<T>();
	if(module(maxVal(*alpha)) > (sqrt(bound) / (2.0)))
		*alpha = random<T>((sqrt(bound) / (2.0)));

    if(module(maxVal(*beta)) > (sqrt(bound) / (2.0)))
		*beta = random<T>((sqrt(bound) / (2.0)));

	maxAB = module( ::std::max(maxVal(*alpha), maxVal(*beta)) );   // Take max of alpha & beta
	maxMN = (cl_double)::std::max( M, N );
	bound = sqrt( bound / (maxAB*maxMN) );           // (maxAB * N * bound^2 + maxAB * bound - UPPER_BOUND) < 0

    lenA = ((order == clblasRowMajor)? M: N) * lda;
    for (i = 0; i < lenA; i++) {
        A[i] = random<T>(bound);
    }

	if( trans == clblasNoTrans )
	{
    	lenX = 1 + ((N - 1) * abs(incx));
    	lenY = 1 + ((M - 1) * abs(incy));
    }
    else {
        lenX = 1 + ((M - 1) * abs(incx));
    	lenY = 1 + ((N - 1) * abs(incy));
    }
    if (X != NULL) {
        for (i = 0; i < lenX; i++) {
			X[i] = random<T>(bound);
        }
    }
    if (Y != NULL) {
        for (i = 0; i < lenY; i++) {
			Y[i] = random<T>(bound);
        }
    }
}

#endif  // GBMV_H_
