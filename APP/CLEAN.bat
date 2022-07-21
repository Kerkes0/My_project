@echo off
del /f /q .\*.uvguix.*
del /f /q .\*.scvd
del /f /q .\JLinkLog.txt
del /f /q .\JLinkSettings.ini
del /f /q .\DebugConfig\*
rd  /q .\DebugConfig
del /f /q .\DEBUG\LST\*
del /f /q .\DEBUG\OBJ\*