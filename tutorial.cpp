// main.cpp

#include <libusb.h>
#include <stdio.h>
#include <unistd.h>
#include <iostream>

#include <QtWidgets/QApplication>
#include <QtWidgets/QTableView>
#include <QtSql/QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

#include "mymodel.h"

int main(int argc, char *argv[])
{

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("../inventory.db");

    if (!db.open()) {
        qDebug("Error occurred opening the database.");
        qDebug("%s.", qPrintable(db.lastError().text()));
        return -1;
    }


	// search for conencted barcode readers
    libusb_device **devs; //pointer to pointer of device, used to retrieve a list of devices
	libusb_context *ctx = NULL; //a libusb session
	libusb_device_handle *dev_handle;


	libusb_init(&ctx);	
	libusb_set_debug(ctx, 3);

	ssize_t count = 0;
	int rc = 0;
   
	count = libusb_get_device_list(ctx, &devs); //get the list of devices


	for (size_t idx; idx < count; idx++)
	{
        libusb_device *device = devs[idx];
        libusb_device_descriptor desc = {0};

        rc = libusb_get_device_descriptor(device, &desc);

        printf("Vendor:Device = %04x:%04x\n", desc.idVendor, desc.idProduct);

	}	

	// open connection to barcode reader
	//05f9:4601	
	dev_handle = libusb_open_device_with_vid_pid(ctx, 0x05f9, 0x4601);

	if(dev_handle == 0)
	{
//		cout << "cannot open scanner";
	}	
	else
	{
//		cout << "opened scanner for coms";
	}

	libusb_free_device_list(devs, 1);


	int r;
	if(libusb_kernel_driver_active(dev_handle, 0) == 1) { //find out if kernel driver is attached
	    printf("Kernel Driver Active");
	    if(libusb_detach_kernel_driver(dev_handle, 0) == 0) //detach it
	        printf("Kernel Driver Detached!");
	}
	r = libusb_claim_interface(dev_handle, 0); //claim interface 0 (the first) of device (mine had jsut 1)
	if(r < 0) {
	    printf("Cannot Claim Interface");
	    return 1;
	}

	
	// scan a barcode
//	unsigned char *data = new unsigned char[40]; //data to read

	unsigned char data[20];
	int actual_length = 0;

	r = libusb_interrupt_transfer(dev_handle, LIBUSB_ENDPOINT_IN | 3, data, sizeof(data), &actual_length, 10000);

	for(int counter = 0; counter < actual_length; counter ++)
	{
		printf("data: %d\n", data[counter]);
	}

	r = libusb_interrupt_transfer(dev_handle, LIBUSB_ENDPOINT_IN | 3, data, sizeof(data), &actual_length, 10000);

	for(int counter = 0; counter < actual_length; counter ++)
	{
		printf("data: %d\n", data[counter]);
	}


	r = libusb_bulk_transfer(dev_handle, LIBUSB_ENDPOINT_IN | 1, data, sizeof(data), &actual_length, 10000);

	for(int counter = 0; counter < actual_length; counter ++)
	{
		printf("data: %d\n", data[counter]);
	}


	printf("size: %d\n", actual_length);
	printf("return: %d\n", r);

    r = libusb_release_interface(dev_handle, 0); //release the claimed interface
    if(r!=0) {
        printf("Cannot Release Interface");
        return 1;
    }

//	libusb_reset_device(dev_handle);
	libusb_close(dev_handle); //close the device we opened
	libusb_exit(ctx); //needs to be called to end the

	
    QSqlQuery query(db);

	// write something to db
    query.prepare("INSERT INTO tbl1 VALUES (null, ?)");
    query.addBindValue("Some text");
    if (!query.exec()) {
        qDebug("Error occurred inserting.");
        qDebug("%s.", qPrintable(db.lastError().text()));
        return -1;
    }

    QApplication a(argc, argv);
    QTableView tableView;
    MyModel myModel(0);
    tableView.setModel( &myModel );
    tableView.show();
    return a.exec();
}
