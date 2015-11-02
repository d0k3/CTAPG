@echo off
for %%x in (ContentInfoArchive*) do (
	ECHO .
	ECHO PROCESSING FILE %%x...
    extrContentInfo.exe %%x %%~nx.txt
	ECHO ...DONE!
	ECHO .
)
cd ..