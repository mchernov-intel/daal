/* file: kmeans_dense_lloyd_multinode_batch_fpt_cpu.cpp */
/*******************************************************************************
* Copyright 2020 Intel Corporation
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
//  Implementation of Lloyd method for K-means algorithm.
//--
*/

#include "algorithms/kernel/kmeans/oneapi/kmeans_oneccl_dense_lloyd_batch_kernel_ucapi.h"
#include "algorithms/kernel/kmeans/oneapi/kmeans_oneccl_dense_lloyd_batch_kernel_ucapi_impl.i"
#include "algorithms/kernel/kmeans/kmeans_multinode_batch_container.h"

namespace daal
{
namespace algorithms
{
namespace preview
{
namespace kmeans
{
namespace interface1
{
template class MultiNodeBatchContainer<DAAL_FPTYPE, defaultDense, DAAL_CPU>;
} // namespace interface1
} // namespace kmeans
} // namespace preview
} // namespace algorithms
} // namespace daal
