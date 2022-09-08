# ota_ble_update_manager

This component is used for over the air updating using bluetooth for an esp32.


## Dependencies

In order for this component to function it requires the esp-nimble-cpp component that can be found here https://github.com/h2zero/esp-nimble-cpp. This library should be added as a component. 

* esp-nimble-cpp
* app_update
* freertos


## How to use 

1. Add this component to your project or componets directory for the esp-idf 
    ```c++
        git clone <ssh repo url>
    ```
2. Include "OtaUpdateManager.h" 
3. Create a new instance of the OtaUpdateManager class. 
    ```c++ 
    OtaUpdateManager *otaUpdateManager = new OtaUpdateManager();
    ```
4. Set the delegate class that will recieve all messages from the OtaUpdateManager class. And set a custom start and ending byte for the data transfer if needed. Default is 0xF1 and 0xFF.

    ```c++
    //the this keyword denotes that the current class will recieve all messages
    otaUpdateManager->setDelegate(this);
    //the bytes that identify the start and end of the data transfer from the sending device.
    otaUpdateManager->setOtaFirmwareStartByte(0xF1);
    otaUpdateManager->setOtaFirmwareEndByte(0xFF);
    ```

5. Extend the abstract class that will act as an interface in the receiving class

    ```c++
    class EFSDevice : public I_OtaUpdateManager{ }
    ```

6. Override the abstract class functions in your .h file to recieve the messages in your application. 

    ```c++
    //ota update interface 
    void otaUpdateDidStart() override;
    void otaUpdateDidFinish() override;
    void otaUpdateDidFail() override;
    ```

7. Finally implement the the ota delegate callbacks in your .cpp file. 

    ```c++
    //OTA Update Manager Delegate Callbacks
    void EFSDevice::otaUpdateDidStart(){
        std::cout << "OTA Update did start" << std::endl;
    }
    void EFSDevice::otaUpdateDidFinish(){
        std::cout << "OTA Update did Finish " << std::endl;
    }
    void EFSDevice::otaUpdateDidFail(){
        std::cout << "OTA Update did fail " << std::endl;
    }
    ```


### Trobleshooting 
Flow Control

Flash Size needs to be correct

Two OTA partitions

NimBLE enabled

1. Make sure NimBLE is enabled menuconfig -> Component config -> Bluetooth -> Bluetooth Host (NimBLE - BLE only)
2. Flow Control needs to be enabled in NimBLE options menuconfig -> Component config -> Bluetooth -> NimBLE Options -> Flow Control
3. Two Ota parameters need to be selected since we need both for ota. menuconfig -> Partition Table -> two ota
4. For IOS there is an intresting issue that occurs when sending data, the fix for it is to apply a parameter update on the onConnect callback. This is handled in your actual application if you are using the esp-nimble-cpp library.

    ```c++
    //when the server connects to the device
    void Device::onConnect(NimBLEServer* pServer, ble_gap_conn_desc *desc){
        //update the connection parameters here so that it doesnt randomly disconnect, mainly for IOS
        this->server->updateConnParams(desc->conn_handle, 0x10, 0x20, 0x00, 400);
    }
    ```





