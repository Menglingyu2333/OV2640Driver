@REM This batch file has been generated by the IAR Embedded Workbench
@REM C-SPY Debugger, as an aid to preparing a command line for running
@REM the cspybat command line utility using the appropriate settings.
@REM
@REM Note that this file is generated every time a new debug session
@REM is initialized, so you may want to move or rename the file before
@REM making changes.
@REM
@REM You can launch cspybat by typing the name of this batch file followed
@REM by the name of the debug file (usually an ELF/DWARF or UBROF file).
@REM
@REM Read about available command line parameters in the C-SPY Debugging
@REM Guide. Hints about additional command line parameters that may be
@REM useful in specific cases:
@REM   --download_only   Downloads a code image without starting a debug
@REM                     session afterwards.
@REM   --silent          Omits the sign-on message.
@REM   --timeout         Limits the maximum allowed execution time.
@REM 


@echo off 

if not "%~1" == "" goto debugFile 

@echo on 

"C:\Program Files (x86)\IAR Systems\Embedded Workbench 8.2\common\bin\cspybat" -f "D:\2_Project\2_AT32_Data\04_Tool\Utilities\AT32F4xx_IAP_Demo_V1.2.0\SourceCode\IAP_Bootloader_IAR\IAR_v8.3\settings\Template.415_Project.general.xcl" --backend -f "D:\2_Project\2_AT32_Data\04_Tool\Utilities\AT32F4xx_IAP_Demo_V1.2.0\SourceCode\IAP_Bootloader_IAR\IAR_v8.3\settings\Template.415_Project.driver.xcl" 

@echo off 
goto end 

:debugFile 

@echo on 

"C:\Program Files (x86)\IAR Systems\Embedded Workbench 8.2\common\bin\cspybat" -f "D:\2_Project\2_AT32_Data\04_Tool\Utilities\AT32F4xx_IAP_Demo_V1.2.0\SourceCode\IAP_Bootloader_IAR\IAR_v8.3\settings\Template.415_Project.general.xcl" "--debug_file=%~1" --backend -f "D:\2_Project\2_AT32_Data\04_Tool\Utilities\AT32F4xx_IAP_Demo_V1.2.0\SourceCode\IAP_Bootloader_IAR\IAR_v8.3\settings\Template.415_Project.driver.xcl" 

@echo off 
:end