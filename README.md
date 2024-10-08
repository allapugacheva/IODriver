## PCI Driver

Custom PSI Driver to list all devices (their DeviceId and VendorId).

## Installation/uninstallation

- To install driver run script install.ps1 in folder [x64/Release](https://github.com/allapugacheva/PCIDriver/tree/main/x64/Release).
- To uninstall driver run script uninstall.ps1 in folder [x64/Release](https://github.com/allapugacheva/PCIDriver/tree/main/x64/Release).

To load the driver, [GDRVLoader](https://github.com/zer0condition/GDRVLoader/tree/master) was used.

## Usage

- Include file PCI_DRIVER_HEADER.h in your project.

- Open driver handler:
```CPP
    HANDLE hDevice = CreateFile(L"\\\\.\\PCIDriver",
                                GENERIC_READ | GENERIC_WRITE,
                                0,
                                nullptr,
                                OPEN_EXISTING,
                                0,
                                nullptr);
```

- Initialize sctucture for receiving data.
```CPP
    PCI_DEVICES pciDevices;
```

- Read PCI devices information:
```CPP
    BOOL success = DeviceIoControl(hDevice,
                                    IOCTL_PCI_READ_CONFIG,
                                    nullptr,
                                    0,
                                    &pciDevices,
                                    sizeof(pciDevices),
                                    &returned,
                                    nullptr);
```