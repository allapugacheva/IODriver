## PCI Driver / IO Driver

IO Driver to get access to interact with ports on Windows

Driver supports next types of data: unsigned char, unsigned short, unsigned long

## Installation/uninstallation

- To install driver run script install.ps1 in folder [x64/Release](https://github.com/allapugacheva/PCIDriver/tree/main/x64/Release).
- To uninstall driver run script uninstall.ps1 in folder [x64/Release](https://github.com/allapugacheva/PCIDriver/tree/main/x64/Release).

To load the driver, [GDRVLoader](https://github.com/zer0condition/GDRVLoader/tree/master) was used.

## Write data

- Include file IO_DRIVER_HEADER.h in your project.

- Open driver handler:
```CPP
    HANDLE hDevice = CreateFile(L"\\\\.\\IODriver",
                                GENERIC_READ | GENERIC_WRITE,
                                0,
                                nullptr,
                                OPEN_EXISTING,
                                0,
                                nullptr);
```

- Initialize structure with data to write:
```CPP
    WRITE_UCHAR wd;
    wd.addr = (unsigned char*)addr;
    wd.data = data;
```

- Write data:
```CPP
    DeviceIoControl(hDevice,
                    IOCTL_WRITE_UCHAR,
                    &wd,
                    sizeof(wd),
                    nullptr,
                    0,
                    &returned,
                    nullptr);
```

## Read data

- Include file IO_DRIVER_HEADER.h in your project.

- Open driver handler:
```CPP
    HANDLE hDevice = CreateFile(L"\\\\.\\IODriver",
                                GENERIC_READ | GENERIC_WRITE,
                                0,
                                nullptr,
                                OPEN_EXISTING,
                                0,
                                nullptr);
```

- Initialize structure with data to write:
```CPP
    READ_UCHAR_ADDR ra;
    ra.addr = (unsigned char*)addr;

    READ_UCHAR_DATA rd;
```

- Write data:
```CPP
    DeviceIoControl(hDevice,
                    IOCTL_READ_UCHAR,
                    &ra,
                    sizeof(ra),
                    &rd,
                    sizeof(rd),
                    &returned,
                    nullptr);
```