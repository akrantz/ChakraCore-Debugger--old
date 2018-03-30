//---------------------------------------------------------------------------------------------------
// Copyright (C) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
//---------------------------------------------------------------------------------------------------

#pragma once

#include "String16.h"
#include <ChakraCore.h>

namespace JsDebug
{
    namespace PropertyHelpers
    {
        JsErrorCode GetProperty(JsValueRef object, const char* name, JsValueRef* value);
        JsErrorCode GetProperty(JsValueRef object, const char* name, bool* value);
        JsErrorCode GetProperty(JsValueRef object, const char* name, int* value);
        JsErrorCode GetProperty(JsValueRef object, const char* name, String16* value);
        JsErrorCode GetPropertyAsString(JsValueRef object, const char* name, String16* value);

        JsErrorCode GetIndexedProperty(JsValueRef object, int index, JsValueRef* value);

        JsErrorCode HasProperty(JsValueRef object, const char* name, bool* hasProperty);

        bool TryGetProperty(JsValueRef object, const char* name, JsValueRef* value);
        bool TryGetProperty(JsValueRef object, const char* name, bool* value);
        bool TryGetProperty(JsValueRef object, const char* name, int* value);
        bool TryGetProperty(JsValueRef object, const char* name, String16* value);
    }
}
