#include <ntddk.h>
#include "IO_DRIVER_HEADER.h"

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObject, PUNICODE_STRING registryPath);
NTSTATUS DriverUnload(PDRIVER_OBJECT pDriverObject);
NTSTATUS IODriverCreateClose(PDEVICE_OBJECT pDeviceObject, PIRP Irp);
NTSTATUS IODriverDeviceControl(PDEVICE_OBJECT pDeviceObject, PIRP Irp);

NTSTATUS IODriverCreateClose(PDEVICE_OBJECT pDeviceObject, PIRP Irp) {

	UNREFERENCED_PARAMETER(pDeviceObject);

	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	return STATUS_SUCCESS;
}

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObject, PUNICODE_STRING registryPath) {

	UNREFERENCED_PARAMETER(registryPath);

	pDriverObject->DriverUnload = DriverUnload;
	pDriverObject->MajorFunction[IRP_MJ_CREATE] = IODriverCreateClose;
	pDriverObject->MajorFunction[IRP_MJ_CLOSE] = IODriverCreateClose;
	pDriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = IODriverDeviceControl;
	
	UNICODE_STRING devName = RTL_CONSTANT_STRING(L"\\Device\\IODriver");
	PDEVICE_OBJECT deviceObject;
	NTSTATUS status = IoCreateDevice(pDriverObject, 0, &devName, FILE_DEVICE_UNKNOWN, 0, FALSE, &deviceObject);

	if (!NT_SUCCESS(status))
		return status;

	UNICODE_STRING symLink = RTL_CONSTANT_STRING(L"\\??\\IODriver");
	IoDeleteSymbolicLink(&symLink);

	status = IoCreateSymbolicLink(&symLink, &devName);

	if (!NT_SUCCESS(status))
		IoDeleteDevice(deviceObject);
		return status;

	return STATUS_SUCCESS;
}

NTSTATUS DriverUnload(PDRIVER_OBJECT pDriverObject) {

	UNREFERENCED_PARAMETER(pDriverObject);

	UNICODE_STRING symLink = RTL_CONSTANT_STRING(L"\\??\\IODriver");
	IoDeleteSymbolicLink(&symLink);

	IoDeleteDevice(pDriverObject->DeviceObject);

	return STATUS_SUCCESS;
}

NTSTATUS IODriverDeviceControl(PDEVICE_OBJECT pDeviceObject, PIRP Irp) {

	UNREFERENCED_PARAMETER(pDeviceObject);

	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(Irp);
	NTSTATUS status = STATUS_SUCCESS;
	ULONG_PTR information = 0;

	switch (stack->Parameters.DeviceIoControl.IoControlCode) {

	case IOCTL_READ_ULONG:
	{
		if (stack->Parameters.DeviceIoControl.InputBufferLength < sizeof(READ_ULONG_ADDR)
			|| stack->Parameters.DeviceIoControl.OutputBufferLength < sizeof(READ_ULONG_DATA))
		{
			status = STATUS_BUFFER_TOO_SMALL;
			break;
		}

		READ_ULONG_ADDR* readAddr = (READ_ULONG_ADDR*)Irp->AssociatedIrp.SystemBuffer;
		unsigned long data = READ_PORT_ULONG(readAddr->addr);
		
		READ_ULONG_DATA* output = (READ_ULONG_DATA*)Irp->AssociatedIrp.SystemBuffer;
		output->data = data;

		information = sizeof(READ_ULONG_DATA);
		break;
	}
	case IOCTL_WRITE_ULONG:
	{
		if (stack->Parameters.DeviceIoControl.InputBufferLength < sizeof(WRITE_ULONG))
		{
			status = STATUS_BUFFER_TOO_SMALL;
			break;
		}

		WRITE_ULONG* writeAddrData = (WRITE_ULONG*)Irp->AssociatedIrp.SystemBuffer;
		WRITE_PORT_ULONG(writeAddrData->addr, writeAddrData->data);

		information = 0;
		break;
	}
	case IOCTL_READ_USHORT:
	{
		if (stack->Parameters.DeviceIoControl.InputBufferLength < sizeof(READ_USHORT_ADDR)
			|| stack->Parameters.DeviceIoControl.OutputBufferLength < sizeof(READ_USHORT_DATA))
		{
			status = STATUS_BUFFER_TOO_SMALL;
			break;
		}

		READ_USHORT_ADDR* readAddr = (READ_USHORT_ADDR*)Irp->AssociatedIrp.SystemBuffer;
		unsigned short data = READ_PORT_USHORT(readAddr->addr);

		READ_USHORT_DATA* output = (READ_USHORT_DATA*)Irp->AssociatedIrp.SystemBuffer;
		output->data = data;

		information = sizeof(READ_USHORT_DATA);
		break;
	}
	case IOCTL_WRITE_USHORT:
	{
		if (stack->Parameters.DeviceIoControl.InputBufferLength < sizeof(WRITE_USHORT))
		{
			status = STATUS_BUFFER_TOO_SMALL;
			break;
		}

		WRITE_USHORT* writeAddrData = (WRITE_USHORT*)Irp->AssociatedIrp.SystemBuffer;
		WRITE_PORT_USHORT(writeAddrData->addr, writeAddrData->data);

		information = 0;
		break;
	}
	case IOCTL_READ_UCHAR:
	{
		if (stack->Parameters.DeviceIoControl.InputBufferLength < sizeof(READ_UCHAR_ADDR)
			|| stack->Parameters.DeviceIoControl.OutputBufferLength < sizeof(READ_UCHAR_DATA))
		{
			status = STATUS_BUFFER_TOO_SMALL;
			break;
		}

		READ_UCHAR_ADDR* readAddr = (READ_UCHAR_ADDR*)Irp->AssociatedIrp.SystemBuffer;
		unsigned char data = READ_PORT_UCHAR(readAddr->addr);

		READ_UCHAR_DATA* output = (READ_UCHAR_DATA*)Irp->AssociatedIrp.SystemBuffer;
		output->data = data;

		information = sizeof(READ_UCHAR_DATA);
		break;
	}
	case IOCTL_WRITE_UCHAR:
	{
		if (stack->Parameters.DeviceIoControl.InputBufferLength < sizeof(WRITE_UCHAR))
		{
			status = STATUS_BUFFER_TOO_SMALL;
			break;
		}

		WRITE_UCHAR* writeAddrData = (WRITE_UCHAR*)Irp->AssociatedIrp.SystemBuffer;
		WRITE_PORT_USHORT(writeAddrData->addr, writeAddrData->data);

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