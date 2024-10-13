#include <ntddk.h>
#include "PCI_DRIVER_HEADER.h"

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObject, PUNICODE_STRING registryPath);
NTSTATUS DriverUnload(PDRIVER_OBJECT pDriverObject);
NTSTATUS PCIDriverCreateClose(PDEVICE_OBJECT DeviceObject, PIRP Irp);
NTSTATUS PCIDriverDeviceControl(PDEVICE_OBJECT pDeviceObject, PIRP Irp);

NTSTATUS PCIDriverCreateClose(PDEVICE_OBJECT DeviceObject, PIRP Irp) {

	UNREFERENCED_PARAMETER(DeviceObject);

	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	return STATUS_SUCCESS;
}

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObject, PUNICODE_STRING registryPath) {

	UNREFERENCED_PARAMETER(registryPath);

	pDriverObject->DriverUnload = DriverUnload;
	pDriverObject->MajorFunction[IRP_MJ_CREATE] = PCIDriverCreateClose;
	pDriverObject->MajorFunction[IRP_MJ_CLOSE] = PCIDriverCreateClose;
	pDriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = PCIDriverDeviceControl;
	
	UNICODE_STRING devName = RTL_CONSTANT_STRING(L"\\Device\\PCIDriver");
	PDEVICE_OBJECT deviceObject;
	NTSTATUS status = IoCreateDevice(pDriverObject, 0, &devName, FILE_DEVICE_UNKNOWN, 0, FALSE, &deviceObject);

	if (!NT_SUCCESS(status))
		return status;

	UNICODE_STRING symLink = RTL_CONSTANT_STRING(L"\\??\\PCIDriver");
	IoDeleteSymbolicLink(&symLink);

	status = IoCreateSymbolicLink(&symLink, &devName);

	if (!NT_SUCCESS(status))
		IoDeleteDevice(deviceObject);
		return status;

	return STATUS_SUCCESS;
}

NTSTATUS DriverUnload(PDRIVER_OBJECT pDriverObject) {

	UNREFERENCED_PARAMETER(pDriverObject);

	UNICODE_STRING symLink = RTL_CONSTANT_STRING(L"\\??\\PCIDriver");
	IoDeleteSymbolicLink(&symLink);

	IoDeleteDevice(pDriverObject->DeviceObject);

	return STATUS_SUCCESS;
}

NTSTATUS PCIDriverDeviceControl(PDEVICE_OBJECT pDeviceObject, PIRP Irp) {

	UNREFERENCED_PARAMETER(pDeviceObject);

	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(Irp);
	NTSTATUS status = STATUS_SUCCESS;
	ULONG_PTR information = 0;

	switch (stack->Parameters.DeviceIoControl.IoControlCode) {

	case IOCTL_PCI_READ_CONFIG:
	{
		unsigned int bus, device, function;
		unsigned int wdata, rdata;
		unsigned short vendorID, deviceID;
		PCI_DEVICES* pciDevices = (PCI_DEVICES*)Irp->AssociatedIrp.SystemBuffer;
		int count = 0;

		for (bus = 0; bus < 256; bus++) {       
			for (device = 0; device < 32; device++) { 
				for (function = 0; function < 8; function++) { 
					wdata = 0x80000000 | (bus << 16) | (device << 11) | (function << 8) | (0x00);
					WRITE_PORT_ULONG(0xCF8, wdata);
					rdata = READ_PORT_ULONG(0xCFC);

					vendorID = rdata & 0xFFFF;
					deviceID = (rdata >> 16) & 0xFFFF;

					if (vendorID != 0xFFFF) {
						pciDevices->devices[count].deviceId = deviceID;
						pciDevices->devices[count].vendorId = vendorID;
						count++;
					}
				}
			}
		}

		pciDevices->count = count;
		information = sizeof(PCI_DEVICES);
		break;
	}
	case IOCTL_READ_DATA:
	{
		if (stack->Parameters.DeviceIoControl.InputBufferLength < sizeof(READ_ADDR)
			|| stack->Parameters.DeviceIoControl.OutputBufferLength < sizeof(READ_DATA))
		{
			status = STATUS_BUFFER_TOO_SMALL;
			break;
		}

		READ_ADDR* readAddr = (READ_ADDR*)Irp->AssociatedIrp.SystemBuffer;
		unsigned int data = READ_PORT_ULONG(readAddr->addr);
		
		READ_DATA* output = (READ_DATA*)Irp->AssociatedIrp.SystemBuffer;
		output->data = data;

		information = sizeof(READ_DATA);
		break;
	}
	case IOCTL_WRITE_DATA:
	{
		if (stack->Parameters.DeviceIoControl.InputBufferLength < sizeof(READ_ADDR))
		{
			status = STATUS_BUFFER_TOO_SMALL;
			break;
		}

		WRITE_ADDR_DATA* writeAddrData = (WRITE_ADDR_DATA*)Irp->AssociatedIrp.SystemBuffer;
		WRITE_PORT_ULONG(writeAddrData->addr, writeAddrData->data);

		information = 0;
		break;
	}
	default:
		information = 0;
		status = STATUS_INVALID_DEVICE_REQUEST;
		break;
	}

	Irp->IoStatus.Status = status;
	Irp->IoStatus.Information = information;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	return status;
}