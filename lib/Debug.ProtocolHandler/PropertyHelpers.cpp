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

            IfJsErrorThrow(JsCopyStringUtf16(stringValue, 0, static_cast<int>(buffer.size()), buffer.data(), nullptr));

            return String16(buffer.data(), buffer.size());
        }

        void GetPropertyAsStringInternal(
            JsValueRef object,
            const char* name,
            bool convertToString,
            String16* value)
        {
            JsValueRef objValue = JS_INVALID_REFERENCE;
            PropertyHelpers::GetProperty(object, name, &objValue);

            JsValueRef stringValue = JS_INVALID_REFERENCE;

            if (convertToString)
            {
                IfJsErrorThrow(JsConvertValueToString(objValue, &stringValue));
            }
            else
            {
                stringValue = objValue;
            }

            int stringLength = 0;
            IfJsErrorThrow(JsGetStringLength(stringValue, &stringLength));

            std::vector<uint16_t> buffer;
            buffer.reserve(stringLength);
            IfJsErrorThrow(JsCopyStringUtf16(stringValue, 0, stringLength, buffer.data(), nullptr));

            String16 str(buffer.data(), stringLength);
            std::swap(*value, str);
        }
    }

    void PropertyHelpers::GetProperty(JsValueRef object, const char* name, JsValueRef* value)
    {
        JsPropertyIdRef propertyId = JS_INVALID_REFERENCE;
        IfJsErrorThrow(JsCreatePropertyId(name, std::strlen(name), &propertyId));
        IfJsErrorThrow(JsGetProperty(object, propertyId, value));
    }

    void PropertyHelpers::GetProperty(JsValueRef object, const char* name, bool* value)
    {
        JsValueRef objValue = JS_INVALID_REFERENCE;
        GetProperty(object, name, &objValue);
        IfJsErrorThrow(JsBooleanToBool(objValue, value));
    }

    void PropertyHelpers::GetProperty(JsValueRef object, const char* name, int* value)
    {
        JsValueRef objValue = JS_INVALID_REFERENCE;
        GetProperty(object, name, &objValue);
        IfJsErrorThrow(JsNumberToInt(objValue, value));
    }

    void PropertyHelpers::GetProperty(JsValueRef object, const char* name, String16* value)
    {
        return GetPropertyAsStringInternal(object, name, false, value);
    }

    void PropertyHelpers::GetPropertyAsString(JsValueRef object, const char* name, String16* value)
    {
        return GetPropertyAsStringInternal(object, name, true, value);
    }

    void PropertyHelpers::GetIndexedProperty(JsValueRef object, int index, JsValueRef* value)
    {
        JsValueRef indexValue = JS_INVALID_REFERENCE;
        IfJsErrorThrow(JsIntToNumber(index, &indexValue));

        int stringLength = 0;
        IfJsErrorThrow(JsGetIndexedProperty(object, indexValue, value));
    }

    bool PropertyHelpers::HasProperty(JsValueRef object, const char* name)
    {
        JsPropertyIdRef propertyId = JS_INVALID_REFERENCE;
        IfJsErrorThrow(JsCreatePropertyId(name, std::strlen(name), &propertyId));

        bool hasProperty = false;
        IfJsErrorThrow(JsHasProperty(object, propertyId, &hasProperty));

        return hasProperty;
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
