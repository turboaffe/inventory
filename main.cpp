// main.cpp

#include <libusb.h>
#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <string>

#include <sqlite3.h>


#include <QtWidgets/QApplication>
#include <QtWidgets/QTableView>
#include <QtSql/QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

#include "mymodel.h"


using std::cout;


int main(int argc, char *argv[])
{

    sqlite3 *db;

    // open db
    if(sqlite3_open("../inventory.db", &db))
    {
        cout << "cannot open sqlite db\n";
        return 0;
    }


//    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
//    db.setDatabaseName("../inventory.db");
//
//    if (!db.open()) {
//        qDebug("Error occurred opening the database.");
//        qDebug("%s.", qPrintable(db.lastError().text()));
//        return -1;
//    }


    // search for conencted barcode readers
    libusb_device **devs; //pointer to pointer of device, used to retrieve a list of devices
    libusb_context *ctx = NULL; //a libusb session
    libusb_device_handle *dev_handle;


    libusb_init(&ctx);  
    libusb_set_debug(ctx, 3);

    ssize_t count = 0;
    int rc = 0;
   
    count = libusb_get_device_list(ctx, &devs); //get the list of devices


    cout << "getting usb device list\n";

    for (size_t idx = 0; idx < count; idx++)
    {
        libusb_device *device = devs[idx];
        libusb_device_descriptor desc = {0};

        rc = libusb_get_device_descriptor(device, &desc);

        cout << "Vendor:Device = " << desc.idVendor << desc.idProduct << "\n";

    }   

    // open connection to barcode reader
    dev_handle = libusb_open_device_with_vid_pid(ctx, 0x05f9, 0x4601);

    if(dev_handle == 0)
    {
        cout << "cannot open scanner\n";
        exit(EXIT_FAILURE);
    }   
    else
    {
        cout << "opened scanner for coms\n";
    }

    libusb_free_device_list(devs, 1);


    //check if any driver is using this device

    if(libusb_kernel_driver_active(dev_handle, 0) == 1) { //find out if kernel driver is attached
        cout << "Kernel Driver Active\n";
        if(libusb_detach_kernel_driver(dev_handle, 0) == 0)
        {
            cout << "Kernel Driver Detached\n";
        }
        else
        { 
            cout << "failed to detach kernel driver\n";
        }
    }
    
    int r;
    r = libusb_claim_interface(dev_handle, 0); 
    if(r < 0) {
        cout << "cannot claim interface";
        exit(EXIT_FAILURE);
    }

    
    // scan a barcode

//  std::vector<unsigned char> data;
    unsigned char data[20];
    int actual_length = 0;

    r = libusb_interrupt_transfer(dev_handle, LIBUSB_ENDPOINT_IN | 3, &data[0], sizeof(data), &actual_length, 10000);

    for(int counter = 0; counter < actual_length; counter ++)
    {
        printf("data: %d\n", data[counter]);
    }

    r = libusb_interrupt_transfer(dev_handle, LIBUSB_ENDPOINT_IN | 3, &data[0], sizeof(data), &actual_length, 10000);

    for(int counter = 0; counter < actual_length; counter ++)
    {
        printf("data: %d\n", data[counter]);
    }


    r = libusb_bulk_transfer(dev_handle, LIBUSB_ENDPOINT_IN | 1, &data[0], sizeof(data), &actual_length, 10000);

    for(int counter = 0; counter < actual_length; counter ++)
    {
        printf("interrupt data: %d\n", data[counter]);
    }

    printf("size: %d\n", actual_length);
    printf("return: %d\n", r);


//  std::string str(data.begin(),data.end());
//    unsigned long long barcode = std::stoull(str, 0, 10);
//
//  cout << barcode;


    r = libusb_release_interface(dev_handle, 0); //release the claimed interface
    if(r!=0) {
        printf("Cannot Release Interface");
        return 1;
    }

    libusb_close(dev_handle); //close the device we opened
    libusb_exit(ctx); //needs to be called to end the

    
//    QSqlQuery query(db);
//
//  // write something to db
//    query.prepare("INSERT INTO tbl1 VALUES (null, ?)");
//    query.addBindValue("Some text");
//    if (!query.exec()) {
//        qDebug("Error occurred inserting.");
//        qDebug("%s.", qPrintable(db.lastError().text()));
//        return -1;
//    }

//    QApplication a(argc, argv);
//    QTableView tableView;
//    MyModel myModel(0);
//    tableView.setModel( &myModel );
//    tableView.show();
//    return a.exec();


    sqlite3_close(db);

}
