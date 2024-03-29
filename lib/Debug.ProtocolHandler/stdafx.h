//---------------------------------------------------------------------------------------------------
// Copyright (C) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
//---------------------------------------------------------------------------------------------------

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

#include <cstdint>
#include <exception>
#include <iostream>
#include <memory>

#include <ChakraCore.h>

inline void IfJsErrorThrow(JsErrorCode error, const char* message)
{
    if (error != JsNoError)
    {
        throw std::runtime_error(message);
    }
}
