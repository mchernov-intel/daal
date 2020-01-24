/* file: Model.java */
/*******************************************************************************
* Copyright 2014-2019 Intel Corporation
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

/**
 * @defgroup boosting Boosting
 * @ingroup classification
 * @{
 */
package com.intel.daal.algorithms.boosting;

import com.intel.daal.utils.*;
import com.intel.daal.services.DaalContext;

/**
 * <a name="DAAL-CLASS-ALGORITHMS__BOOSTING__MODEL"></a>
 * @brief Base class for models of %boosting algorithms.
 *        Contains a collection of weak learner algorithm models constructed during training of the %boosting algorithm
 */
public class Model extends com.intel.daal.algorithms.classifier.Model {
    /** @private */
    static {
        LibUtils.loadLibrary();
    }

    /**
     * Constructs the model
     * @param context  Context to manage the model
     */
    public Model(DaalContext context) {
        super(context);
    }

    public Model(DaalContext context, long cModel) {
        super(context, cModel);
    }
}
/** @} */
