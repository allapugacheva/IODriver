## PCI Driver / IO Driver

Custom PSI Driver to list all devices (their DeviceId and VendorId).

Also you may use it to interract with IO ports.

## Installation/uninstallation

- To install driver run script install.ps1 in folder [x64/Release](https://github.com/allapugacheva/PCIDriver/tree/main/x64/Release).
- To uninstall driver run script uninstall.ps1 in folder [x64/Release](https://github.com/allapugacheva/PCIDriver/tree/main/x64/Release).

To load the driver, [GDRVLoader](https://github.com/zer0condition/GDRVLoader/tree/master) was used.

## Reading PCI configuration

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
    DeviceIoControl(hDevice,
                    IOCTL_PCI_READ_CONFIG,
                    nullptr,
                    0,
                    &pciDevices,
                    sizeof(pciDevices),
                    &returned,
                    nullptr);
```

## Write data

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

- Initialize structure with data to write:
```CPP
    WRITE_ADDR_DATA wad;
    wad.addr = (unsigned long*)addr;
    wad.data = data;
```

- Write data:
```CPP
    DeviceIoControl(hDevice,
                    IOCTL_WRITE_DATA,
                    &wad,
                    sizeof(wad),
                    nullptr,
                    0,
                    &returned,
                    nullptr);
```

## Read data

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

- Initialize structure with data to write:
```CPP
    READ_ADDR ra;
    ra.addr = (unsigned long*)addr;

    READ_DATA rd;
```

- Write data:
```CPP
    DeviceIoControl(hDevice,
                    IOCTL_READ_DATA,
                    &ra,
                    sizeof(ra),
                    &rd,
                    sizeof(rd),
                    &returned,
                    nullptr);
```