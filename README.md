# ota_ble_update_manager

This component is used for over the air updating using bluetooth for an esp32.



## Dependencies

In order for this component to function it requires the esp-nimble-cpp component that can be found here https://github.com/h2zero/esp-nimble-cpp. This library should be added as a component. 


## How to use 

1. Add this component to your project or componets directory for the esp-idf 
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




