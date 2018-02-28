# ChakraCore-Debugger
Debugging companion for the ChakraCore JavaScript engine

## Prerequisites

* Visual Studio 2017
* Python 2.7

## Building

ChakraCore-Debugger depends on ChakraCore from [NuGet](https://www.nuget.org/packages/Microsoft.ChakraCore.vc140).

1. Clone the project (and submodules)
```console
> git clone --recurse-submodules https://github.com/kfarnung/ChakraCore-Debugger
```
2. Launch the solution file
```console
> cd .\ChakraCore-Debugger\
> .\ChakraCoreDebugger.sln
```
3. Right-click the "Debug.Sample" project and click "Set as StartUp Project"
4. Choose the desired build configuration and platform, then hit `F5` to start debugging
