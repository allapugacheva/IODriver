Write-Host "Installing driver..."
Start-Process "GDRVLoader.exe" -ArgumentList "PCIDriver.sys", "load"
Read-Host "Successful installation. Press enter..."