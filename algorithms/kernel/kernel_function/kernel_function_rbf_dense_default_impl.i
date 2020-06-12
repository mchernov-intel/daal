/* file: kernel_function_rbf_dense_default_impl.i */
/*******************************************************************************
* Copyright 2014-2020 Intel Corporation
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*******************************************************************************/

/*
//++
//  RBF kernel functions implementation
//--
*/

#ifndef __KERNEL_FUNCTION_RBF_DENSE_DEFAULT_IMPL_I__
#define __KERNEL_FUNCTION_RBF_DENSE_DEFAULT_IMPL_I__

#include "algorithms/kernel_function/kernel_function_types_rbf.h"
#include "service/kernel/data_management/service_numeric_table.h"
#include "externals/service_math.h"
#include "externals/service_blas.h"
#include "algorithms/threading/threading.h"

using namespace daal::data_management;

namespace daal
{
namespace algorithms
{
namespace kernel_function
{
namespace rbf
{
namespace internal
{
template <typename algorithmFPType, CpuType cpu>
services::Status KernelImplRBF<defaultDense, algorithmFPType, cpu>::computeInternalVectorVector(const NumericTable * a1, const NumericTable * a2,
                                                                                                NumericTable * r, const ParameterBase * par)
{
    //prepareData
    const size_t nFeatures = a1->getNumberOfColumns();

    ReadRows<algorithmFPType, cpu> mtA1(*const_cast<NumericTable *>(a1), par->rowIndexX, 1);
    DAAL_CHECK_BLOCK_STATUS(mtA1);
    const algorithmFPType * dataA1 = mtA1.get();

    ReadRows<algorithmFPType, cpu> mtA2(*const_cast<NumericTable *>(a2), par->rowIndexY, 1);
    DAAL_CHECK_BLOCK_STATUS(mtA2);
    const algorithmFPType * dataA2 = mtA2.get();

    WriteOnlyRows<algorithmFPType, cpu> mtR(r, par->rowIndexResult, 1);
    DAAL_CHECK_BLOCK_STATUS(mtR);

    //compute
    const Parameter * rbfPar          = static_cast<const Parameter *>(par);
    const algorithmFPType invSqrSigma = (algorithmFPType)(1.0 / (rbfPar->sigma * rbfPar->sigma));
    algorithmFPType factor            = 0.0;
    PRAGMA_IVDEP
    PRAGMA_VECTOR_ALWAYS
    for (size_t i = 0; i < nFeatures; i++)
    {
        algorithmFPType diff = (dataA1[i] - dataA2[i]);
        factor += diff * diff;
    }
    factor *= -0.5 * invSqrSigma;
    daal::internal::Math<algorithmFPType, cpu>::vExp(1, &factor, mtR.get());
    return services::Status();
}

template <typename algorithmFPType, CpuType cpu>
services::Status KernelImplRBF<defaultDense, algorithmFPType, cpu>::computeInternalMatrixVector(const NumericTable * a1, const NumericTable * a2,
                                                                                                NumericTable * r, const ParameterBase * par)
{
    //prepareData
    const size_t nVectors1 = a1->getNumberOfRows();
    const size_t nFeatures = a1->getNumberOfColumns();

    ReadRows<algorithmFPType, cpu> mtA1(*const_cast<NumericTable *>(a1), 0, nVectors1);
    DAAL_CHECK_BLOCK_STATUS(mtA1);
    const algorithmFPType * dataA1 = mtA1.get();

    ReadRows<algorithmFPType, cpu> mtA2(*const_cast<NumericTable *>(a2), par->rowIndexY, 1);
    DAAL_CHECK_BLOCK_STATUS(mtA2);
    const algorithmFPType * dataA2 = mtA2.get();

    WriteOnlyRows<algorithmFPType, cpu> mtR(r, 0, nVectors1);
    DAAL_CHECK_BLOCK_STATUS(mtR);
    algorithmFPType * dataR = mtR.get();

    //compute
    const Parameter * rbfPar          = static_cast<const Parameter *>(par);
    const algorithmFPType invSqrSigma = (algorithmFPType)(1.0 / (rbfPar->sigma * rbfPar->sigma));
    for (size_t i = 0; i < nVectors1; i++)
    {
        algorithmFPType factor = 0.0;
        PRAGMA_IVDEP
        PRAGMA_VECTOR_ALWAYS
        for (size_t j = 0; j < nFeatures; j++)
        {
            algorithmFPType diff = (dataA1[i * nFeatures + j] - dataA2[j]);
            factor += diff * diff;
        }
        dataR[i] = -0.5 * invSqrSigma * factor;

        if (dataR[i] < Math<algorithmFPType, cpu>::vExpThreshold())
        {
            dataR[i] = Math<algorithmFPType, cpu>::vExpThreshold();
        }
    }
    daal::internal::Math<algorithmFPType, cpu>::vExp(nVectors1, dataR, dataR);
    return services::Status();
}

template <typename algorithmFPType, CpuType cpu>
services::Status KernelImplRBF<defaultDense, algorithmFPType, cpu>::computeInternalMatrixMatrix(const NumericTable * a1, const NumericTable * a2,
                                                                                                NumericTable * r, const ParameterBase * par)
{
    SafeStatus safeStat;

    const size_t nVectors1 = a1->getNumberOfRows();
    const size_t nVectors2 = a2->getNumberOfRows();
    const size_t nFeatures = a1->getNumberOfColumns();

    const Parameter * rbfPar    = static_cast<const Parameter *>(par);
    const algorithmFPType coeff = (algorithmFPType)(-0.5 / (rbfPar->sigma * rbfPar->sigma));

    if (a1 != a2)
    {
        char trans = 'T', notrans = 'N';
        algorithmFPType zero = 0.0, negTwo = -2.0;

        DAAL_OVERFLOW_CHECK_BY_ADDING(size_t, nVectors1, nVectors2);
        DAAL_OVERFLOW_CHECK_BY_MULTIPLICATION(size_t, nVectors1 + nVectors2, sizeof(algorithmFPType));

        daal::internal::TArray<algorithmFPType, cpu> aBuf((nVectors1 + nVectors2));
        DAAL_CHECK(aBuf.get(), services::ErrorMemoryAllocationFailed);
        algorithmFPType * buffer = aBuf.get();

        algorithmFPType * sqrDataA1 = buffer;
        algorithmFPType * sqrDataA2 = buffer + nVectors1;

        const size_t blockSize = 256;
        const size_t blockNum1 = nVectors1 / blockSize + !!(nVectors1 % blockSize);
        const size_t blockNum2 = nVectors2 / blockSize + !!(nVectors2 % blockSize);

        daal::threader_for(blockNum1, blockNum1, [&](const size_t iBlock) {
            const size_t numRowsInBlock = (iBlock != blockNum1 - 1) ? blockSize : nVectors1 - iBlock * blockSize;
            const size_t startRow       = iBlock * blockSize;
            const size_t finishRow      = startRow + numRowsInBlock;

            ReadRows<algorithmFPType, cpu> mtA1(*const_cast<NumericTable *>(a1), startRow, numRowsInBlock);
            DAAL_CHECK_BLOCK_STATUS_THR(mtA1);
            const algorithmFPType * dataA1 = const_cast<algorithmFPType *>(mtA1.get());

            for (size_t i = startRow, ii = 0; i < finishRow; ++i, ++ii)
            {
                sqrDataA1[i] = zero;
                for (size_t j = 0; j < nFeatures; ++j)
                {
                    sqrDataA1[i] += dataA1[ii * nFeatures + j] * dataA1[ii * nFeatures + j];
                }
            }
        });

        daal::threader_for(blockNum2, blockNum2, [&](const size_t iBlock) {
            const size_t numRowsInBlock = (iBlock != blockNum2 - 1) ? blockSize : nVectors2 - iBlock * blockSize;
            const size_t startRow       = iBlock * blockSize;
            const size_t finishRow      = startRow + numRowsInBlock;

            ReadRows<algorithmFPType, cpu> mtA2(*const_cast<NumericTable *>(a2), startRow, numRowsInBlock);
            DAAL_CHECK_BLOCK_STATUS_THR(mtA2);
            const algorithmFPType * dataA2 = const_cast<algorithmFPType *>(mtA2.get());

            for (size_t i = startRow, ii = 0; i < finishRow; ++i, ++ii)
            {
                sqrDataA2[i] = zero;
                for (size_t j = 0; j < nFeatures; ++j)
                {
                    sqrDataA2[i] += dataA2[ii * nFeatures + j] * dataA2[ii * nFeatures + j];
                }
            }
        });

        daal::threader_for(blockNum1, blockNum1, [&](const size_t iBlock1) {
            DAAL_INT numRowsInBlock1 = (iBlock1 != blockNum1 - 1) ? blockSize : nVectors1 - iBlock1 * blockSize;
            DAAL_INT startRow1       = iBlock1 * blockSize;
            DAAL_INT finishRow1      = startRow1 + numRowsInBlock1;

            ReadRows<algorithmFPType, cpu> mtA1(*const_cast<NumericTable *>(a1), startRow1, numRowsInBlock1);
            DAAL_CHECK_BLOCK_STATUS_THR(mtA1);
            const algorithmFPType * dataA1 = const_cast<algorithmFPType *>(mtA1.get());

            WriteRows<algorithmFPType, cpu> mtR(r, startRow1, numRowsInBlock1);
            DAAL_CHECK_BLOCK_STATUS_THR(mtR);
            algorithmFPType * dataR = mtR.get();

            daal::threader_for(blockNum2, blockNum2, [&, nVectors2, blockNum2](const size_t iBlock2) {
                DAAL_INT numRowsInBlock2 = (iBlock2 != blockNum2 - 1) ? blockSize : nVectors2 - iBlock2 * blockSize;
                DAAL_INT startRow2       = iBlock2 * blockSize;

                ReadRows<algorithmFPType, cpu> mtA2(*const_cast<NumericTable *>(a2), startRow2, numRowsInBlock2);
                DAAL_CHECK_BLOCK_STATUS_THR(mtA2);
                const algorithmFPType * dataA2 = const_cast<algorithmFPType *>(mtA2.get());

                Blas<algorithmFPType, cpu>::xxgemm(&trans, &notrans, &numRowsInBlock2, &numRowsInBlock1, (DAAL_INT *)&nFeatures, &negTwo, dataA2,
                                                   (DAAL_INT *)&nFeatures, dataA1, (DAAL_INT *)&nFeatures, &zero, dataR + iBlock2 * blockSize,
                                                   (DAAL_INT *)&nVectors2);
            });

            for (size_t i = startRow1, ii = 0; i < finishRow1; ++i, ++ii)
            {
                PRAGMA_IVDEP
                PRAGMA_VECTOR_ALWAYS
                for (size_t k = 0; k < nVectors2; ++k)
                {
                    dataR[ii * nVectors2 + k] += (sqrDataA1[i] + sqrDataA2[k]);
                    dataR[ii * nVectors2 + k] *= coeff;
                    if (dataR[ii * nVectors2 + k] < Math<algorithmFPType, cpu>::vExpThreshold())
                    {
                        dataR[ii * nVectors2 + k] = Math<algorithmFPType, cpu>::vExpThreshold();
                    }
                }
            }

            daal::internal::Math<algorithmFPType, cpu>::vExp(numRowsInBlock1 * nVectors2, dataR, dataR);
        });
    }
    else
    {
        char trans = 'T', notrans = 'N';
        algorithmFPType zero = 0.0, one = 1.0, two = 2.0;

        const size_t blockSize = 256;
        const size_t blockNum1 = nVectors1 / blockSize + !!(nVectors1 % blockSize);
        const size_t blockNum2 = nVectors2 / blockSize + !!(nVectors2 % blockSize);

        daal::threader_for(blockNum1, blockNum1, [&](const size_t iBlock1) {
            DAAL_INT numRowsInBlock1 = (iBlock1 != blockNum1 - 1) ? blockSize : nVectors1 - iBlock1 * blockSize;
            DAAL_INT startRow1       = iBlock1 * blockSize;

            ReadRows<algorithmFPType, cpu> mtA1(*const_cast<NumericTable *>(a1), startRow1, numRowsInBlock1);
            DAAL_CHECK_BLOCK_STATUS_THR(mtA1);
            const algorithmFPType * dataA1 = const_cast<algorithmFPType *>(mtA1.get());

            WriteRows<algorithmFPType, cpu> mtR(r, startRow1, numRowsInBlock1);
            DAAL_CHECK_BLOCK_STATUS_THR(mtR);
            algorithmFPType * dataRLocal = mtR.get();

            daal::threader_for(iBlock1 + 1, iBlock1 + 1, [&](const size_t iBlock2) {
                DAAL_INT numRowsInBlock2 = (iBlock2 != blockNum2 - 1) ? blockSize : nVectors2 - iBlock2 * blockSize;
                DAAL_INT startRow2       = iBlock2 * blockSize;

                ReadRows<algorithmFPType, cpu> mtA2(*const_cast<NumericTable *>(a2), startRow2, numRowsInBlock2);
                DAAL_CHECK_BLOCK_STATUS_THR(mtA2);
                const algorithmFPType * dataA2 = const_cast<algorithmFPType *>(mtA2.get());

                Blas<algorithmFPType, cpu>::xxgemm(&trans, &notrans, &numRowsInBlock2, &numRowsInBlock1, (DAAL_INT *)&nFeatures, &one, dataA2,
                                                   (DAAL_INT *)&nFeatures, dataA1, (DAAL_INT *)&nFeatures, &zero, dataRLocal + iBlock2 * blockSize,
                                                   (DAAL_INT *)&nVectors2);
            });
        });

        WriteRows<algorithmFPType, cpu> mtR(r, 0, nVectors1);
        DAAL_CHECK_BLOCK_STATUS(mtR);
        algorithmFPType * dataR = mtR.get();

        const size_t blockNum = nVectors1 / blockSize + !!(nVectors1 % blockSize);

        TArray<algorithmFPType, cpu> diagonal(nVectors1);
        DAAL_CHECK(diagonal.get(), services::ErrorMemoryAllocationFailed);
        for (size_t i = 0; i < nVectors1; ++i) diagonal[i] = dataR[i * nVectors1 + i];

        daal::threader_for(blockNum, blockNum, [&](const size_t iBlock) {
            const size_t numRowsInBlock = (iBlock != blockNum - 1) ? blockSize : nVectors1 - iBlock * blockSize;
            const size_t startRow       = iBlock * blockSize;
            const size_t finishRow      = startRow + numRowsInBlock;

            for (size_t i = startRow; i < finishRow; ++i)
            {
                PRAGMA_IVDEP
                PRAGMA_VECTOR_ALWAYS
                for (size_t k = 0; k < i; ++k)
                {
                    dataR[i * nVectors1 + k] = coeff * (diagonal[i] + diagonal[k] - two * dataR[i * nVectors1 + k]);
                }

                dataR[i * nVectors1 + i] = zero;
                daal::internal::Math<algorithmFPType, cpu>::vExp(i + 1, dataR + i * nVectors1, dataR + i * nVectors1);

                PRAGMA_IVDEP
                PRAGMA_VECTOR_ALWAYS
                for (size_t k = 0; k < i; ++k)
                {
                    dataR[k * nVectors1 + i] = dataR[i * nVectors1 + k];
                }
            }
        });
    }

    return services::Status();
}

} // namespace internal

} // namespace rbf

} // namespace kernel_function

} // namespace algorithms

} // namespace daal

#endif
