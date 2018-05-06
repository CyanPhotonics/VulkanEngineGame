rmdir %2 /s /q
mkdir %2
xcopy %1 %2 /E

rmdir %3 /s /q
mkdir %3
xcopy %1 %3 /E

rmdir %4 /s /q
mkdir %4
xcopy %1 %4 /E