import { Observable, ApplicationSettings } from '@nativescript/core'
import { getBluetoothInstance } from '@nativescript-community/ble';
var bluetooth = getBluetoothInstance();

// This function has some uneccesary steps in it such as the for loop putting the array into another arry
// for some reason the only way to get the list to refresh is to create a new object array, this function does that
function getBlueToothList(viewModel) {
    var blueToothList = viewModel.get('devicesFound');
    var bluetoothRefreshList = [];
    for (var i = 0; i < blueToothList.length; i++) {
        bluetoothRefreshList[i] = { bluetoothID: blueToothList[i].bluetoothID, isSelected: false };
    }
    return bluetoothRefreshList;
}

// finds the selected item
function findSelectedDevice(viewModel) {
    var blueToothList = viewModel.get('devicesFound');
    // loop through devices and see if any are selected
    for (let blueToothDevices of blueToothList) {
        if (blueToothDevices.isSelected) { 
            return blueToothDevices;
        }
    }
    return  [{ isSelected: false }]; // if none found
}

// stores devices found in an array and prevents duplicates
function deviceFound(viewModel, peripheral) {
    // gets devices found array
    var blueToothList = viewModel.get('devicesFound');
    var bluetoothRefreshList = [];

    // if none found yet, initialise array
    if (blueToothList.length != 0) {
        // check if already exists
        if (!blueToothList.some(l => l.bluetoothID.UUID == peripheral.UUID)) {
            blueToothList.push({ bluetoothID: peripheral, isSelected: false });
        } 
        bluetoothRefreshList = blueToothList;
    } else {
        bluetoothRefreshList[0] = { bluetoothID: peripheral, isSelected: false };
    }

    // add back to view model, BLUETOOTHREFRESHLIST IS REQUIRED TO REFRESH LISTVIEW IN XML
    // it is unnesseasry but its the only way to refresh it
    viewModel.set('devicesFound', bluetoothRefreshList);
}

// change view to the scanning screen
// shows lists found
// shows stop search button
// shows connect button
function scanStartedView(viewModel) {
    var intialiseList = [];
    viewModel.set('devicesFound', intialiseList);
    viewModel.set('showBluetoothList', true);
    viewModel.set('findController', false);
    viewModel.set('stopController', true);
    viewModel.set('searchedDevices', true);
    viewModel.set('deviceSelected', false);
}

// change view to stop scanned screen
// will reset back to normal screen if none found else, will stay on current page and show devices
function scanStoppedView(viewModel) {
    viewModel.set('findController', true);
    viewModel.set('stopController', false); // to remove
    if (viewModel.get('devicesFound').length > 0) {
        viewModel.set('showBluetoothList', true);
    } else {
        viewModel.set('showBluetoothList', false)
        viewModel.set('searchedDevices', false);
    }
}

// change view to wifi details page
function wifiDetailsView(viewModel) {
    viewModel.set('WifiName', "");
    viewModel.set('WifiPassword', "");
    viewModel.set('isConnecting', false);
    viewModel.set('showWifiInput', true);
    viewModel.set('findController', false);
    viewModel.set('showBluetoothList', false);
    viewModel.set('showWriteLogo', false);
}

// used to stored bluetooth device info into internal sotrage. is required to keep session
function updateStoredDevice(appSet, pUUID, sUUID, cUUID) {
    appSet.setString("pUUID", pUUID);
    appSet.setString("sUUID", sUUID);
    appSet.setString("cUUID", cUUID);
}

// opening page for connect, just shows find controller button
function searchBluetoothViewModel(viewModel) {
    viewModel.set('findController', true);
    viewModel.set('showBluetoothList', false);
    viewModel.set('searchedDevices', false);
    viewModel.set('deviceSelected', false);
    viewModel.set('showWifiInput', false);
    viewModel.set('isConnecting', false);
    viewModel.set('isConnectingWifi', false);
    viewModel.set('showWriteLogo', false);
}

// for initiallising connect page, determines if device is already connect
function connectPageIntialize(viewModel, isConnected) {
    var getPUUID = isConnected.getString("pUUID"); // gets stored string
    viewModel.set('headerSelected', 4); // needed for underline in header

    // only try and see if connected if getPUUID has any value
    // if finds a device alreaady connected, then go straight to wifi details page, else go to normal page
    if (getPUUID != undefined && getPUUID != "") {
        bluetooth.isConnected({ 
            UUID: getPUUID
            }).then(connected => {
                console.log("Connected? " + connected)
                if (connected) {
                    wifiDetailsView(viewModel);
                } else {
                    searchBluetoothViewModel(viewModel);
                }
            }, error => {
                console.log(error);
                searchBluetoothViewModel(viewModel);
            });
    } else {
        searchBluetoothViewModel(viewModel);
    }
}

function startScanningFunction(viewModel) {
    // first check if bluetooth is enabled
    bluetooth.isBluetoothEnabled().then(enabled => {
        if (!enabled) {
            alert("Please Turn on Bluetooth")
        } else {
            // if bluetooth enabled, switch to the scanning page and start search
            scanStartedView(viewModel);

            // will search for all bluetooth devices locally, ondiscovered will print device name and UUID to console
            // and will also update the list on the phone through the deviceFound Function
            bluetooth.startScanning({
                filters: [{deviceName:'HMSoft'}], // MY DEFAULT BLUETOOTH NAME, THIS MAYBE DIFFERENT
                seconds: 4, // time scanning
                onDiscovered: function (peripheral) {
                    console.log("Periperhal found with UUID: " + peripheral.UUID );
                    console.log("Periperhal found with Name: " + peripheral.name );
                    deviceFound(viewModel, peripheral);
                }
            }).then(function() {
                console.log("scanning complete");
                scanStoppedView(viewModel);
            }, function (err) {
                console.log("error while scanning: " + err);
                scanStoppedView(viewModel);
            });
        }
    }, error => {
        console.log(error);
    });
}

// stop scanning
function stopScanningFunction(viewModel) {
    bluetooth.stopScanning().then(function() {
        scanStoppedView(viewModel); // change view 
        console.log("scanning stopped");
    });
}

// connects to bluetooth device
function connectFunction(viewModel, appSet) {
    // returns true if a device was selected
    var device = findSelectedDevice(viewModel); 

    if (device.isSelected) {
        stopScanningFunction(viewModel); // change view to stop scanning
        console.log('Connecting...');
        viewModel.set('isConnecting', true); // show loading icon

        bluetooth.connect({
            UUID: device.bluetoothID.UUID,
            onConnected: function (peripheral) {
                console.log("Periperhal connected with UUID: " + peripheral.UUID);

                // store device details in internal storage, required for other functions
                // update view
                updateStoredDevice(appSet, peripheral.UUID, peripheral.services[2].UUID, peripheral.services[2].characteristics[0].UUID); 
                wifiDetailsView(viewModel); 

                // keep for debugging
                // the peripheral object now has a list of available services, useful for debugging:
                //peripheral.services.forEach(function(service) {
                //    console.log("service found: " + JSON.stringify(service));
                //});
            },
            onDisconnected: function (peripheral) {
                console.log("Periperhal disconnected with UUID: " + peripheral.UUID);
            }
        });
    }
}

// dis-connects from bluetooth device (THIS FUNCTION CAN ONLY BE ACCESSED ONCE A DEVICE IS CONNNECTED)
function disConnectFunction(viewModel, appSet) {
    var getUUID = appSet.getString("pUUID");

    bluetooth.disconnect({
        UUID: getUUID
    }).then(function() {
        console.log("disconnected successfully");
        // reset stored details and change view
        updateStoredDevice(appSet, "", "", "");
        searchBluetoothViewModel(viewModel);
    }, function (err) {
        // in this case you're probably best off treating this as a disconnected peripheral though
        console.log("disconnection error: " + err);
    });
}

// used to update wifi details sending stuff
function updateWifiLoadView(viewModel, sending, failed, success, connection) {
    viewModel.set('isSuccessLogo', success);
    viewModel.set('isFailedLogo', failed);
    viewModel.set('isSendingLogo', sending);
    viewModel.set('isConnectingWifi', connection);
}

function sendWifiDetails(viewModel, appSet) {
    const delay = ms => new Promise(res => setTimeout(res, ms)); // for delays
    var name = viewModel.get('WifiName');
    var Password = viewModel.get('WifiPassword');

    // set transmission string
    var transmissionString = "w" + name + "+" + Password + "\n";
    console.log(transmissionString);

    viewModel.set('showWriteLogo', true);
    updateWifiLoadView(viewModel, true, false, false, true);

    bluetooth.write({
        peripheralUUID: appSet.getString("pUUID"),
        serviceUUID: appSet.getString("sUUID"),
        characteristicUUID: appSet.getString("cUUID"),
        value: transmissionString
        }).then(async function(result) {
            console.log("value written");
            console.log(result)
            await delay(1000) // used just to similate the ticking icon
            updateWifiLoadView(viewModel, false, false, true, false);

            // USEFUL WHEN DOING FEED BACK
            /*while (i<5) {
                i++
                console.log('attempt: ' + i);
                await delay(2000)
            }*/
        }, function (err) {
            console.log("write error: " + err);
            updateWifiLoadView(viewModel, false, true, false, false);
    });
}

export function ConnectViewModel() {
    const viewModel = new Observable();
    const appSet = ApplicationSettings;
    connectPageIntialize(viewModel, appSet);

    viewModel.connectController = () => {
        startScanningFunction(viewModel);
    }
    
    viewModel.stopScan = () => {
        stopScanningFunction(viewModel);
    }

    viewModel.onItemTap = (args) => {
        var deviceList = getBlueToothList(viewModel); // get bluetooth list, this also puts the existing list in a new list, required for resfreshing list (NS BUG)
        deviceList[args.index].isSelected = true; // isSelected to true highlights list item
        viewModel.set('devicesFound', deviceList); // reset list, needs to be done to refresh list
        viewModel.set('deviceSelected', true); // show connect button from opaque to full color when item is selected
    }

    viewModel.connect = () => {
        connectFunction(viewModel, appSet);
    }

    viewModel.disConnect = () => {
        disConnectFunction(viewModel, appSet);
    }

    viewModel.wifiPasswordUpdate = () => {
        sendWifiDetails(viewModel, appSet)
    }

    return viewModel
}
