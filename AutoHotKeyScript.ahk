#NoEnv  ; Recommended for performance and compatibility with future AutoHotkey releases.
; #Warn  ; Enable warnings to assist with detecting common errors.
SendMode Input  ; Recommended for new scripts due to its superior speed and reliability.

; run without spawning console window and return stdout
JEE_RunGetStdOut(vTarget, vSize:="")
{
    DetectHiddenWindows, On
    vComSpec := A_ComSpec ? A_ComSpec : ComSpec
    Run, % vComSpec,, Hide, vPID
    WinWait, % "ahk_pid " vPID
    DllCall("kernel32\AttachConsole", "UInt",vPID)
    oShell := ComObjCreate("WScript.Shell")
    oExec := oShell.Exec(vTarget)
    vStdOut := ""
    if !(vSize = "")
        VarSetCapacity(vStdOut, vSize)
    while !oExec.StdOut.AtEndOfStream
        vStdOut := oExec.StdOut.ReadAll()
    DllCall("kernel32\FreeConsole")
    Process, Close, % vPID
    return vStdOut
}

; Alt+E - send last explorer window path
<!e::
temp := JEE_RunGetStdOut(".\build\Release\ConEmuCD.exe") 
SendRaw cd %temp%
return