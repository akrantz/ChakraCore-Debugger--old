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
        IfJsErrorThrow(PropertyHelpers::GetProperty(m_callFrameInfo.Get(), "scriptId", &scriptId), "failed to get script ID value");

        return scriptId;
    }

    int DebuggerCallFrame::Line() const
    {
        int line = 0;
        IfJsErrorThrow(PropertyHelpers::GetProperty(m_callFrameInfo.Get(), "line", &line), "failed to get script ID value");

        return line;
    }

    int DebuggerCallFrame::Column() const
    {
        int column = 0;
        IfJsErrorThrow(PropertyHelpers::GetProperty(m_callFrameInfo.Get(), "column", &column), "failed to get script ID value");

        return column;
    }

    int DebuggerCallFrame::ContextId() const
    {
        return 1;
    }

    bool DebuggerCallFrame::IsAtReturn() const
    {
        int index;
        IfJsErrorThrow(
            PropertyHelpers::GetProperty(m_callFrameInfo.Get(), "index", &index),
            "failed to get index value");

        JsValueRef properties;
        IfJsErrorThrow(JsDiagGetStackProperties(index, &properties), "failed to get stack properties");

        bool hasProp;
        IfJsErrorThrow(
            PropertyHelpers::HasProperty(m_callFrameInfo.Get(), "returnValue", &hasProp),
            "failed to check return value property");

        if (!hasProp) {
            return false;
        }

        JsValueRef propVal;
        IfJsErrorThrow(
            PropertyHelpers::GetProperty(m_callFrameInfo.Get(), "returnValue", &propVal),
            "failed to get return value");

        IfJsErrorThrow(PropertyHelpers::HasProperty(propVal, "handle", &hasProp), "failed to check handle property");

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
        IfJsErrorThrow(
            PropertyHelpers::GetProperty(m_callFrameInfo.Get(), "index", &callFrameIndex),
            "failed to get call frame index");

        return "{\"ordinal\":" + String::fromInteger(callFrameIndex) + "}";
    }

    std::unique_ptr<Location> DebuggerCallFrame::GetFunctionLocation() const
    {
        int functionHandle = 0;
        IfJsErrorThrow(
            PropertyHelpers::GetProperty(m_callFrameInfo.Get(), "functionHandle", &functionHandle),
            "failed to get function handle property");

        JsValueRef funcObj = JS_INVALID_REFERENCE;
        IfJsErrorThrow(
            JsDiagGetObjectFromHandle(functionHandle, &funcObj),
            "failed to get object from handle");

        String scriptId;
        IfJsErrorThrow(
            PropertyHelpers::GetPropertyAsString(funcObj, "scriptId", &scriptId),
            "failed to get script ID property");
        
        int line = 0;
        IfJsErrorThrow(
            PropertyHelpers::GetProperty(funcObj, "line", &line),
            "failed to get line property");
        
        int column = 0;
        IfJsErrorThrow(
            PropertyHelpers::GetProperty(funcObj, "column", &column),
            "failed to get column property");

        return Location::create()
            .setColumnNumber(column)
            .setLineNumber(line)
            .setScriptId(scriptId)
            .build();
    }

    String DebuggerCallFrame::GetFunctionName() const
    {
        int functionHandle = 0;
        IfJsErrorThrow(
            PropertyHelpers::GetProperty(m_callFrameInfo.Get(), "functionHandle", &functionHandle),
            "failed to get function handle");

        JsValueRef funcObj = JS_INVALID_REFERENCE;
        IfJsErrorThrow(
            JsDiagGetObjectFromHandle(functionHandle, &funcObj),
            "failed to get object from handle");

        String functionName;
        IfJsErrorThrow(
            PropertyHelpers::GetProperty(funcObj, "name", &functionName),
            "failed to get name property");

        return functionName;
    }

    std::unique_ptr<Location> DebuggerCallFrame::GetLocation() const
    {
        String scriptId;
        IfJsErrorThrow(
            PropertyHelpers::GetPropertyAsString(m_callFrameInfo.Get(), "scriptId", &scriptId),
            "failed to get script ID property");

        int line = 0;
        IfJsErrorThrow(
            PropertyHelpers::GetProperty(m_callFrameInfo.Get(), "line", &line),
            "failed to get line property");

        int column = 0;
        IfJsErrorThrow(
            PropertyHelpers::GetProperty(m_callFrameInfo.Get(), "column", &column),
            "failed to get column property");

        return Location::create()
            .setColumnNumber(column)
            .setLineNumber(line)
            .setScriptId(scriptId)
            .build();
    }

    std::unique_ptr<RemoteObject> DebuggerCallFrame::GetReturnValue() const
    {
        int callFrameIndex = 0;
        IfJsErrorThrow(
            PropertyHelpers::GetProperty(m_callFrameInfo.Get(), "index", &callFrameIndex),
            "failed to get call frame index");

        JsValueRef stackProperties = JS_INVALID_REFERENCE;
        IfJsErrorThrow(
            JsDiagGetStackProperties(callFrameIndex, &stackProperties),
            "failed to get stack properties");

        bool hasProperty = false;
        IfJsErrorThrow(
            PropertyHelpers::HasProperty(stackProperties, "returnValue", &hasProperty),
            "failed to check return value property");

        if (hasProperty) {
            JsValueRef returnObj = JS_INVALID_REFERENCE;
            IfJsErrorThrow(
                PropertyHelpers::GetProperty(stackProperties, "returnValue", &returnObj),
                "failed to get return value property");

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
        IfJsErrorThrow(
            PropertyHelpers::GetProperty(m_callFrameInfo.Get(), "index", &callFrameIndex),
            "failed to get call frame index");

        JsValueRef stackProperties = JS_INVALID_REFERENCE;
        IfJsErrorThrow(
            JsDiagGetStackProperties(callFrameIndex, &stackProperties),
            "failed to get stack properties");

        bool hasProperty = false;
        IfJsErrorThrow(
            PropertyHelpers::HasProperty(stackProperties, "thisObject", &hasProperty),
            "failed to check thisObject property");

        if (hasProperty) {
            JsValueRef thisObject = JS_INVALID_REFERENCE;
            IfJsErrorThrow(
                PropertyHelpers::GetProperty(stackProperties, "thisObject", &thisObject),
                "failed to get thisObject property");

            return ProtocolHelpers::WrapObject(thisObject);
        }

        // The protocol requires a "this" member, so create an undefined object to return.
        return ProtocolHelpers::GetUndefinedObject();
    }
}
