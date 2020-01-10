/* file: input.cpp */
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

/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>/* Header for class com_intel_daal_algorithms_association_rules_Batch */

#include "daal.h"
#include "com_intel_daal_algorithms_association_rules_Input.h"
#include "common_helpers.h"

USING_COMMON_NAMESPACES();

JNIEXPORT jlong JNICALL Java_com_intel_daal_algorithms_association_1rules_Input_cInit(JNIEnv * env, jobject thisObj, jlong algAddr, jint prec,
                                                                                      jint method, jint cmode)
{
    return jniBatch<association_rules::Method, association_rules::Batch, association_rules::apriori>::getInput(prec, method, algAddr);
}

/*
 * Class:     com_intel_daal_algorithms_association_rules_Input
 * Method:    cSetDataSet
 * Signature:(JIJ)I
 */
JNIEXPORT void JNICALL Java_com_intel_daal_algorithms_association_1rules_Input_cSetInput(JNIEnv * env, jobject thisObj, jlong inputAddr, jint id,
                                                                                         jlong ntAddr)
{
    jniInput<association_rules::Input>::set<association_rules::InputId, NumericTable>(inputAddr, id, ntAddr);
}

JNIEXPORT jlong JNICALL Java_com_intel_daal_algorithms_association_1rules_Input_cGetInput(JNIEnv * env, jobject thisObj, jlong inputAddr, jint id)
{
    return jniInput<association_rules::Input>::get<association_rules::InputId, NumericTable>(inputAddr, id);
}