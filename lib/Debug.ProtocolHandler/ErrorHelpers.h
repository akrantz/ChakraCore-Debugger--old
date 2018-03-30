//---------------------------------------------------------------------------------------------------
// Copyright (C) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
//---------------------------------------------------------------------------------------------------

#pragma once

#include <ChakraCore.h>
#include <string>

#define IfFailRet(v) \
    { \
        JsErrorCode error = (v); \
        if (error != JsNoError) \
        { \
            return error; \
        } \
    }

namespace JsDebug
{
    class JsErrorException
        : public std::runtime_error
    {
    public:
        JsErrorException(JsErrorCode error, const char* message)
            : std::runtime_error(MakeMessage(error, message))
        {
        }

    private:
        static std::string MakeMessage(JsErrorCode error, std::string message)
        {
            if (!message.empty())
            {
                message.append(": ");
            }

            message.append(std::to_string(error));

            return message;
        }
    };

    inline void IfJsErrorThrow(JsErrorCode error, const char* message)
    {
        if (error != JsNoError)
        {
            throw JsErrorException(error, message);
        }
    }
}
