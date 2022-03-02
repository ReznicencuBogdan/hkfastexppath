MaxByteCount = 4096
VarSetCapacity(ExplorerPath, MaxByteCount)
hkfastexppath = .\build\Release\ConEmuCD.dll
hkfastexppathwfunc = %hkfastexppath%\WriteLastExplorerPathBuffer
hmodule := DllCall("LoadLibrary", "str", hkfastexppath, "UInt")
if !hmodule
{
    MsgBox "Failed loading hkfastexppath library"
}


; Alt+E - send last explorer window path
<!e::
if !hmodule
{
    Exit
}
ReturnValue := DllCall(hkfastexppathwfunc, "WStr", ExplorerPath, "Int", MaxByteCount, "Int")
if ReturnValue > 0
{
    SendRaw cd %ExplorerPath%
}
return