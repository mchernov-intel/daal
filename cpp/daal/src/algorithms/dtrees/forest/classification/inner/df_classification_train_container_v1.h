/* file: df_classification_train_container_v1.h */
/*******************************************************************************
* Copyright 2014-2021 Intel Corporation
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
//  Implementation of decision forest container.
//--
*/

#ifndef __DF_CLASSIFICATION_TRAIN_CONTAINER_V1_H__
#define __DF_CLASSIFICATION_TRAIN_CONTAINER_V1_H__

#include "src/algorithms/kernel.h"
#include "algorithms/decision_forest/decision_forest_classification_training_types.h"
#include "algorithms/decision_forest/decision_forest_classification_training_batch.h"
#include "src/algorithms/dtrees/forest/classification/df_classification_train_kernel.h"
#include "src/algorithms/dtrees/forest/classification/oneapi/df_classification_train_hist_kernel_oneapi.h"
#include "src/algorithms/dtrees/forest/classification/df_classification_model_impl.h"
#include "src/services/service_algo_utils.h"

namespace daal
{
namespace algorithms
{
namespace decision_forest
{
namespace classification
{
namespace training
{
namespace interface1
{
DAAL_FORCEINLINE void convertParameter(interface1::Parameter & par1, interface3::Parameter & par3)
{
    par3.nTrees                      = par1.nTrees;
    par3.observationsPerTreeFraction = par1.observationsPerTreeFraction;
    par3.featuresPerNode             = par1.featuresPerNode;
    par3.maxTreeDepth                = par1.maxTreeDepth;
    par3.minObservationsInLeafNode   = par1.minObservationsInLeafNode;
    par3.seed                        = par1.seed;
    par3.engine                      = par1.engine;
    par3.impurityThreshold           = par1.impurityThreshold;
    par3.varImportance               = par1.varImportance;
    par3.resultsToCompute            = par1.resultsToCompute;
    par3.memorySavingMode            = par1.memorySavingMode;
    par3.bootstrap                   = par1.bootstrap;
}

template <typename algorithmFPType, Method method, CpuType cpu>
BatchContainer<algorithmFPType, method, cpu>::BatchContainer(daal::services::Environment::env * daalEnv)
{
    auto & context    = services::internal::getDefaultContext();
    auto & deviceInfo = context.getInfoDevice();

    if (method == hist && !deviceInfo.isCpu)
    {
        __DAAL_INITIALIZE_KERNELS_SYCL(internal::ClassificationTrainBatchKernelOneAPI, algorithmFPType, method);
    }
    else
    {
        __DAAL_INITIALIZE_KERNELS(internal::ClassificationTrainBatchKernel, algorithmFPType, method);
    }
}

template <typename algorithmFPType, Method method, CpuType cpu>
BatchContainer<algorithmFPType, method, cpu>::~BatchContainer()
{
    __DAAL_DEINITIALIZE_KERNELS();
}

template <typename algorithmFPType, Method method, CpuType cpu>
services::Status BatchContainer<algorithmFPType, method, cpu>::compute()
{
    auto & context    = services::internal::getDefaultContext();
    auto & deviceInfo = context.getInfoDevice();

    classifier::training::Input * input = static_cast<classifier::training::Input *>(_in);
    Result * result                     = static_cast<Result *>(_res);

    const NumericTable * const x = input->get(classifier::training::data).get();
    const NumericTable * const y = input->get(classifier::training::labels).get();
    const NumericTable * const w = nullptr;

    decision_forest::classification::Model * m = result->get(classifier::training::model).get();

    decision_forest::classification::training::interface1::Parameter * par =
        static_cast<decision_forest::classification::training::interface1::Parameter *>(_par);
    decision_forest::classification::training::interface3::Parameter par3(par->nClasses);
    convertParameter(*par, par3);
    daal::services::Environment::env & env = *_env;

    if (method == hist && !deviceInfo.isCpu)
    {
        __DAAL_CALL_KERNEL_SYCL(env, internal::ClassificationTrainBatchKernelOneAPI, __DAAL_KERNEL_ARGUMENTS(algorithmFPType, method), compute,
                                daal::services::internal::hostApp(*input), x, y, *m, *result, par3);
    }
    else
    {
        __DAAL_CALL_KERNEL(env, internal::ClassificationTrainBatchKernel, __DAAL_KERNEL_ARGUMENTS(algorithmFPType, method), compute,
                           daal::services::internal::hostApp(*input), x, y, w, *m, *result, par3);
    }
}

template <typename algorithmFPType, Method method, CpuType cpu>
services::Status BatchContainer<algorithmFPType, method, cpu>::setupCompute()
{
    Result * result                                              = static_cast<Result *>(_res);
    decision_forest::classification::Model * m                   = result->get(classifier::training::model).get();
    decision_forest::classification::internal::ModelImpl * pImpl = dynamic_cast<decision_forest::classification::internal::ModelImpl *>(m);
    DAAL_ASSERT(pImpl);
    pImpl->clear();
    return services::Status();
}
} // namespace interface1

} // namespace training
} // namespace classification
} // namespace decision_forest
} // namespace algorithms
} // namespace daal

#endif
