Write-Host "Removing driver..."
Start-Process "GDRVLoader.exe" -ArgumentList "PCIDriver.sys", "unload"
Read-Host "Successful remove. Press enter..."