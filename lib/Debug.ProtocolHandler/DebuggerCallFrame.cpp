//---------------------------------------------------------------------------------------------------
// Copyright (C) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
//---------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "DebuggerCallFrame.h"

#include "ErrorHelpers.h"
#include "PropertyHelpers.h"
#include "ProtocolHelpers.h"

namespace JsDebug
{
    using protocol::Array;
    using protocol::Debugger::CallFrame;
    using protocol::Debugger::Location;
    using protocol::Debugger::Scope;
    using protocol::Runtime::RemoteObject;
    using protocol::String;

    DebuggerCallFrame::DebuggerCallFrame(JsValueRef callFrameInfo)
        : m_callFrameInfo(callFrameInfo)
    {
    }

    int DebuggerCallFrame::SourceId() const
    {
        int scriptId = 0;
        IfJsErrorThrow(PropertyHelpers::GetProperty(m_callFrameInfo.Get(), "scriptId", &scriptId));

        return scriptId;
    }

    int DebuggerCallFrame::Line() const
    {
        int line = 0;
        IfJsErrorThrow(PropertyHelpers::GetProperty(m_callFrameInfo.Get(), "line", &line));

        return line;
    }

    int DebuggerCallFrame::Column() const
    {
        int column = 0;
        IfJsErrorThrow(PropertyHelpers::GetProperty(m_callFrameInfo.Get(), "column", &column));

        return column;
    }

    int DebuggerCallFrame::ContextId() const
    {
        return 1;
    }

    bool DebuggerCallFrame::IsAtReturn() const
    {
        int index;
        IfJsErrorThrow(PropertyHelpers::GetProperty(m_callFrameInfo.Get(), "index", &index));

        JsValueRef properties;
        IfJsErrorThrow(JsDiagGetStackProperties(index, &properties));

        bool hasProp;
        IfJsErrorThrow(PropertyHelpers::HasProperty(m_callFrameInfo.Get(), "returnValue", &hasProp));

        if (!hasProp) {
            return false;
        }

        JsValueRef propVal;
        IfJsErrorThrow(PropertyHelpers::GetProperty(m_callFrameInfo.Get(), "returnValue", &propVal));

        IfJsErrorThrow(PropertyHelpers::HasProperty(propVal, "handle", &hasProp));

        return hasProp;
    }

    std::unique_ptr<CallFrame> DebuggerCallFrame::ToProtocolValue() const
    {
        return CallFrame::create()
            .setCallFrameId(GetCallFrameId())
            .setFunctionLocation(GetFunctionLocation())
            .setFunctionName(GetFunctionName())
            .setLocation(GetLocation())
            .setReturnValue(GetReturnValue())
            .setScopeChain(GetScopeChain())
            .setThis(GetThis())
            .build();
    }

    String DebuggerCallFrame::GetCallFrameId() const
    {
        int callFrameIndex = 0;
        IfJsErrorThrow(PropertyHelpers::GetProperty(m_callFrameInfo.Get(), "index", &callFrameIndex));

        return "{\"ordinal\":" + String::fromInteger(callFrameIndex) + "}";
    }

    std::unique_ptr<Location> DebuggerCallFrame::GetFunctionLocation() const
    {
        int functionHandle = 0;
        IfJsErrorThrow(PropertyHelpers::GetProperty(m_callFrameInfo.Get(), "functionHandle", &functionHandle));

        JsValueRef funcObj = JS_INVALID_REFERENCE;
        IfJsErrorThrow(JsDiagGetObjectFromHandle(functionHandle, &funcObj));

        String scriptId;
        IfJsErrorThrow(PropertyHelpers::GetPropertyAsString(funcObj, "scriptId", &scriptId));
        
        int line = 0;
        IfJsErrorThrow(PropertyHelpers::GetProperty(funcObj, "line", &line));
        
        int column = 0;
        IfJsErrorThrow(PropertyHelpers::GetProperty(funcObj, "column", &column));

        return Location::create()
            .setColumnNumber(column)
            .setLineNumber(line)
            .setScriptId(scriptId)
            .build();
    }

    String DebuggerCallFrame::GetFunctionName() const
    {
        int functionHandle = 0;
        IfJsErrorThrow(PropertyHelpers::GetProperty(m_callFrameInfo.Get(), "functionHandle", &functionHandle));

        JsValueRef funcObj = JS_INVALID_REFERENCE;
        IfJsErrorThrow(JsDiagGetObjectFromHandle(functionHandle, &funcObj));

        String functionName;
        IfJsErrorThrow(PropertyHelpers::GetProperty(funcObj, "name", &functionName));

        return functionName;
    }

    std::unique_ptr<Location> DebuggerCallFrame::GetLocation() const
    {
        String scriptId;
        IfJsErrorThrow(PropertyHelpers::GetPropertyAsString(m_callFrameInfo.Get(), "scriptId", &scriptId));

        int line = 0;
        IfJsErrorThrow(PropertyHelpers::GetProperty(m_callFrameInfo.Get(), "line", &line));

        int column = 0;
        IfJsErrorThrow(PropertyHelpers::GetProperty(m_callFrameInfo.Get(), "column", &column));

        return Location::create()
            .setColumnNumber(column)
            .setLineNumber(line)
            .setScriptId(scriptId)
            .build();
    }

    std::unique_ptr<RemoteObject> DebuggerCallFrame::GetReturnValue() const
    {
        int callFrameIndex = 0;
        IfJsErrorThrow(PropertyHelpers::GetProperty(m_callFrameInfo.Get(), "index", &callFrameIndex));

        JsValueRef stackProperties = JS_INVALID_REFERENCE;
        IfJsErrorThrow(JsDiagGetStackProperties(callFrameIndex, &stackProperties));

        bool hasProperty = false;
        IfJsErrorThrow(PropertyHelpers::HasProperty(stackProperties, "returnValue", &hasProperty));

        if (hasProperty) {
            JsValueRef returnObj = JS_INVALID_REFERENCE;
            IfJsErrorThrow(PropertyHelpers::GetProperty(stackProperties, "returnValue", &returnObj));

            return ProtocolHelpers::WrapObject(returnObj);
        }

        return nullptr;
    }

    std::unique_ptr<Array<Scope>> DebuggerCallFrame::GetScopeChain() const
    {
        // TODO: Fill this in
        return Array<Scope>::create();
    }

    std::unique_ptr<RemoteObject> DebuggerCallFrame::GetThis() const
    {
        int callFrameIndex = 0;
        IfJsErrorThrow(PropertyHelpers::GetProperty(m_callFrameInfo.Get(), "index", &callFrameIndex));

        JsValueRef stackProperties = JS_INVALID_REFERENCE;
        IfJsErrorThrow(JsDiagGetStackProperties(callFrameIndex, &stackProperties));

        bool hasProperty = false;
        IfJsErrorThrow(PropertyHelpers::HasProperty(stackProperties, "thisObject", &hasProperty));

        if (hasProperty) {
            JsValueRef thisObject = JS_INVALID_REFERENCE;
            IfJsErrorThrow(PropertyHelpers::GetProperty(stackProperties, "thisObject", &thisObject));

            return ProtocolHelpers::WrapObject(thisObject);
        }

        // The protocol requires a "this" member, so create an undefined object to return.
        return ProtocolHelpers::GetUndefinedObject();
    }
}
