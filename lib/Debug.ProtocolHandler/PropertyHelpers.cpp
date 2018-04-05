//---------------------------------------------------------------------------------------------------
// Copyright (C) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
//---------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "PropertyHelpers.h"
#include "ErrorHelpers.h"

namespace JsDebug
{
    namespace
    {
        String16 ValueAsString(JsValueRef object, bool doConversion = false)
        {
            JsValueRef stringValue = JS_INVALID_REFERENCE;

            if (doConversion)
            {
                IfJsErrorThrow(JsConvertValueToString(object, &stringValue));
            }
            else
            {
                stringValue = object;
            }

            int stringLength = 0;
            IfJsErrorThrow(JsGetStringLength(stringValue, &stringLength));

            std::vector<uint16_t> buffer;

            // Resize the vector an initialize the elements to zero.
            buffer.resize(stringLength, 0);

            IfJsErrorThrow(JsCopyStringUtf16(stringValue, 0, buffer.size(), buffer.data(), nullptr));

            return String16(buffer.data(), buffer.size());
        }

        JsErrorCode GetPropertyAsStringInternal(
            JsValueRef object,
            const char* name,
            bool convertToString,
            String16* value)
        {
            JsValueRef objValue = JS_INVALID_REFERENCE;
            IfFailRet(PropertyHelpers::GetProperty(object, name, &objValue));

            JsValueRef stringValue = JS_INVALID_REFERENCE;

            if (convertToString)
            {
                IfFailRet(JsConvertValueToString(objValue, &stringValue));
            }
            else
            {
                stringValue = objValue;
            }

            int stringLength = 0;
            IfFailRet(JsGetStringLength(stringValue, &stringLength));

            std::vector<uint16_t> buffer;
            buffer.reserve(stringLength);
            IfFailRet(JsCopyStringUtf16(stringValue, 0, stringLength, buffer.data(), nullptr));

            String16 str(buffer.data(), stringLength);
            std::swap(*value, str);

            return JsNoError;
        }
    }

    JsErrorCode PropertyHelpers::GetProperty(JsValueRef object, const char* name, JsValueRef* value)
    {
        JsPropertyIdRef propertyId = JS_INVALID_REFERENCE;
        IfFailRet(JsCreatePropertyId(name, std::strlen(name), &propertyId));
        IfFailRet(JsGetProperty(object, propertyId, value));

        return JsNoError;
    }

    JsErrorCode PropertyHelpers::GetProperty(JsValueRef object, const char* name, bool* value)
    {
        JsValueRef objValue = JS_INVALID_REFERENCE;
        IfFailRet(GetProperty(object, name, &objValue));
        IfFailRet(JsBooleanToBool(objValue, value));

        return JsNoError;
    }

    JsErrorCode PropertyHelpers::GetProperty(JsValueRef object, const char* name, int* value)
    {
        JsValueRef objValue = JS_INVALID_REFERENCE;
        IfFailRet(GetProperty(object, name, &objValue));
        IfFailRet(JsNumberToInt(objValue, value));

        return JsNoError;
    }

    JsErrorCode PropertyHelpers::GetProperty(JsValueRef object, const char* name, String16* value)
    {
        return GetPropertyAsStringInternal(object, name, false, value);
    }

    JsErrorCode PropertyHelpers::GetPropertyAsString(JsValueRef object, const char* name, String16* value)
    {
        return GetPropertyAsStringInternal(object, name, true, value);
    }

    JsErrorCode PropertyHelpers::GetIndexedProperty(JsValueRef object, int index, JsValueRef* value)
    {
        JsValueRef indexValue = JS_INVALID_REFERENCE;
        IfFailRet(JsIntToNumber(index, &indexValue));

        int stringLength = 0;
        IfFailRet(JsGetIndexedProperty(object, indexValue, value));

        return JsNoError;
    }

    JsErrorCode PropertyHelpers::HasProperty(JsValueRef object, const char* name, bool* hasProperty)
    {
        JsPropertyIdRef propertyId = JS_INVALID_REFERENCE;
        IfFailRet(JsCreatePropertyId(name, std::strlen(name), &propertyId));
        IfFailRet(JsHasProperty(object, propertyId, hasProperty));

        return JsNoError;
    }

    bool PropertyHelpers::TryGetProperty(JsValueRef object, const char* name, JsValueRef* value)
    {
        JsPropertyIdRef propertyId = JS_INVALID_REFERENCE;
        IfJsErrorThrow(JsCreatePropertyId(name, std::strlen(name), &propertyId));

        bool hasProperty = false;
        IfJsErrorThrow(JsHasProperty(object, propertyId, &hasProperty));

        if (hasProperty)
        {
            IfJsErrorThrow(JsGetProperty(object, propertyId, value));
            return true;
        }

        return false;
    }

    bool PropertyHelpers::TryGetProperty(JsValueRef object, const char* name, bool* value)
    {
        JsValueRef propertyValue = JS_INVALID_REFERENCE;
        if (TryGetProperty(object, name, &propertyValue))
        {
            IfJsErrorThrow(JsBooleanToBool(propertyValue, value));
            return true;
        }

        return false;
    }

    bool PropertyHelpers::TryGetProperty(JsValueRef object, const char* name, int* value)
    {
        JsValueRef propertyValue = JS_INVALID_REFERENCE;
        if (TryGetProperty(object, name, &propertyValue))
        {
            IfJsErrorThrow(JsNumberToInt(propertyValue, value));
            return true;
        }

        return false;
    }

    bool PropertyHelpers::TryGetProperty(JsValueRef object, const char* name, String16* value)
    {
        JsValueRef propertyValue = JS_INVALID_REFERENCE;
        if (TryGetProperty(object, name, &propertyValue))
        {
            *value = ValueAsString(propertyValue);
            return true;
        }

        return false;
    }
}
