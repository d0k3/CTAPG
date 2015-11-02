@echo off
md icons_png
cd icons
for %%x in (*.icn) do (
	ECHO .
	ECHO PROCESSING FILE %%x...
    ..\3DSicnConv.exe %%x %%~nx.bmp
    ..\nconvert -quiet -overwrite -out png -o "..\icons_png\%%~nx.png" %%~nx.bmp
    del %%~nx.bmp
	ECHO ...DONE!
	ECHO .
)
cd ..