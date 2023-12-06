write-host "Compiling code:`n"
./main.exe 
nasm -f win64 code.asm 
gcc -o code.exe code.obj -nostdlib -lkernel32 
./code.exe 
write-host "`n`nExit code:`n"
$lastExitCode