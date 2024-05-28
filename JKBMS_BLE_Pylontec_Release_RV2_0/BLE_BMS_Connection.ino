bool connectToBLEServer() {
  
    Serial.print("Forming a connection to ");
    Serial.println(myDevice->getAddress().toString().c_str());

    pClient->setClientCallbacks(new MyClientCallback());
    delay(500); // hope it helps against ->  lld_pdu_get_tx_flush_nb HCI packet count mismatch (0, 1)
    // Connect to the remove BLE Server.
    pClient->connect(myDevice);  // if you pass BLEAdvertisedDevice instead of address, it will be recognized type of peer device address (public or private)
    delay(500); // hope it helps against ->  lld_pdu_get_tx_flush_nb HCI packet count mismatch (0, 1)
    Serial.println(" - Connected to server");

    //Added next line VITAL TO STOP ESP32 S3 / LILYGO T-DISPLAY S3 Crashing at this point!   
    pClient->setMTU(517); //set client to request maximum MTU from server (default is 23 otherwise)


    // Obtain a reference to the service we are after in the remote BLE server.
    BLERemoteService* pRemoteService = pClient->getService(serviceUUID);
    if (pRemoteService == nullptr) {
      Serial.print("Failed to find our service UUID: ");
      Serial.println(serviceUUID.toString().c_str());
      pClient->disconnect();
      return false;
    }
    Serial.println(" - Found our service");

    // Obtain a reference to the characteristic in the service of the remote BLE server.
    pRemoteCharacteristic = pRemoteService->getCharacteristic(charUUID);
    if (pRemoteCharacteristic == nullptr) {
      Serial.print("Failed to find our characteristic UUID: ");
      Serial.println(charUUID.toString().c_str());
      pClient->disconnect();
      return false;
    }
    Serial.println(" - Found our characteristic");

    // Read the value of the characteristic.
    if(pRemoteCharacteristic->canRead()) {
      std::string value = pRemoteCharacteristic->readValue();
      Serial.print("The characteristic value was: ");
      Serial.println(value.c_str());
    }

     if(pRemoteCharacteristic->canNotify()) {
      pRemoteCharacteristic->registerForNotify(notifyCallback);
      Serial.println("Notify the characteristic");
    }

// Sending getdevice info
      pRemoteCharacteristic->writeValue(getdeviceInfo, 20);
      sendingtime = millis();
      Serial.println("Sending device Info");

    ble_connected = true;
    doConnect = false;
    Serial.println("We are now connected to the BLE Server.");
   
    return true;
}
