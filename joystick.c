#include <stdio.h>
#include <libusb-1.0/libusb.h>

#define VENDOR_ID 0x068e   // Ersetzen Sie VendorID durch die ID Ihres Joysticks
#define PRODUCT_ID 0x0105 // Ersetzen Sie ProductID durch die ID Ihres Joysticks

int main() {
    libusb_device_handle *dev_handle;
    int error;

    // Initialisieren Sie libusb
    error = libusb_init(NULL);
    if (error < 0) {
        fprintf(stderr, "Fehler beim Initialisieren von libusb: %s\n", libusb_strerror(error));
        return 1;
    }

    // Öffnen Sie das USB-Gerät anhand von Vendor ID und Product ID
    dev_handle = libusb_open_device_with_vid_pid(NULL, VENDOR_ID, PRODUCT_ID);
    if (!dev_handle) {
        fprintf(stderr, "Joystick nicht gefunden.\n");
        libusb_exit(NULL);
        return 1;
    }

    // Setzen Sie die Konfiguration des USB-Geräts (in der Regel 1 für HID-Geräte)
    error = libusb_set_configuration(dev_handle, 1);
    if (error < 0) {
        fprintf(stderr, "Fehler beim Setzen der Konfiguration: %s\n", libusb_strerror(error));
        libusb_close(dev_handle);
        libusb_exit(NULL);
        return 1;
    }

    // Kernel-Treiber abhängen (falls erforderlich)
    libusb_detach_kernel_driver(dev_handle, 0);

    // Clipping-Flags löschen (falls erforderlich)
    error = libusb_claim_interface(dev_handle, 0);
    if (error < 0) {
        fprintf(stderr, "Fehler beim Beanspruchen der Schnittstelle: %s\n", libusb_strerror(error));
        libusb_close(dev_handle);
        libusb_exit(NULL);
        return 1;
    }

    // Endlosschleife zum Lesen von Joystick-Daten
    while (1) {
        unsigned char data[8];
        int transferred;

        error = libusb_interrupt_transfer(dev_handle, 0x81, data, sizeof(data), &transferred, 1000);

        if (error == 0 && transferred == sizeof(data)) {
            // Daten erfolgreich gelesen, hier können Sie die Daten auswerten und ausgeben
            printf("Joystick-Daten: %02x %02x %02x %02x %02x %02x %02x %02x\n",
                   data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7]);
        } else if (error != LIBUSB_ERROR_TIMEOUT) {
            fprintf(stderr, "Fehler beim Lesen der Joystick-Daten: %s\n", libusb_strerror(error));
        }
    }

    // Aufräumen und Gerätefreigabe
    libusb_close(dev_handle);
    libusb_exit(NULL);

    return 0;
}
