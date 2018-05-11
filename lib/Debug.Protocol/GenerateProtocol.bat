@if "debug"=="" echo off
setlocal

set DEPS=..\\..\\deps

if not exist "generated" mkdir generated

python "GenerateProtocol.py" --generator_script "%DEPS%\\inspector_protocol\\CodeGenerator.py" --jinja_dir "%DEPS%\\jinja\\" --markupsafe_dir "%DEPS%\\markupsafe\\" --output_base "generated\\" --config "inspector_protocol_config.json"