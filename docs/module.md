## Debugging on Windows

* procdump (generate crash dump)
* WinDbg
* dumpbin (require Visual Studio command prompt)

### Start Visual Studio prompt 

`cmd /k VsDevCmd.bat` start the VS prompt. The `VsDevCmd.bat` script should be
available in the VS installation directory (e.g. "C:\Program Files\Microsoft Visual Studio 14.0\Common7\Tools\VsDevCmd.bat").

### List DLL dependencies

* Start VS prompt
* `dumpbin /DEPENDENTS file`
