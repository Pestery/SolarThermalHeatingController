import { Observable, ApplicationSettings } from '@nativescript/core'
import { Bluetooth, getBluetoothInstance } from '@nativescript-community/ble';
//var bluetooth = new Bluetooth();
var bluetooth = getBluetoothInstance();

// TODO 
// Add found counter
// Prioritise Arduino devices
// List devices
// IF Device found stay
// de-saible connect and disconnet if no results
// disable connect button
// Enter WIFI stuff
// write to device
// Disconnect



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
    var bluetoothDevice = [{ isSelected: false }];
    for (let blueToothDevices of blueToothList) {
        if (blueToothDevices.isSelected) { 
            return blueToothDevices;
        }
    }
    return bluetoothDevice;
}

// stores devices found in an array
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

function scanStarted(viewModel) {
    var intialiseList = [];
    viewModel.set('devicesFound', intialiseList);
    viewModel.set('showBluetoothList', true);
    viewModel.set('findController', false);
    viewModel.set('stopController', true);
    viewModel.set('searchedDevices', true);
    viewModel.set('deviceSelected', false);
}

function scanStopped(viewModel) {
    viewModel.set('findController', true);
    viewModel.set('stopController', false); // to remove
    if (viewModel.get('devicesFound').length > 0) {
        viewModel.set('showBluetoothList', true);
    } else {
        viewModel.set('showBluetoothList', false)
        viewModel.set('searchedDevices', false);
    }
}

function stopScanning(viewModel) {
    bluetooth.stopScanning().then(function() {
        scanStopped(viewModel);
        console.log("scanning stopped");
    });
}

function wifiDetailsViewModel(viewModel) {
    viewModel.set('WifiName', "");
    viewModel.set('WifiPassword', "");
    viewModel.set('isConnecting', false);
    viewModel.set('showWifiInput', true);
    viewModel.set('findController', false);
    viewModel.set('showBluetoothList', false);
}

function updateStoredDevice(appSet, pUUID, sUUID, cUUID) {
    appSet.setString("pUUID", pUUID);
    appSet.setString("sUUID", sUUID);
    appSet.setString("cUUID", cUUID);
}

function searchBluetoothViewModel(viewModel) {
    var blueToothList = [];
    viewModel.set('findController', true);
    viewModel.set('showBluetoothList', false);
    viewModel.set('devicesFound', blueToothList);
    viewModel.set('searchedDevices', false);
    viewModel.set('deviceSelected', false);
    viewModel.set('showWifiInput', false);
    viewModel.set('isConnecting', false);
    viewModel.set('isConnectingWifi', false);
}

function connectPageIntialize(viewModel, isConnected) {
    var getPUUID = isConnected.getString("pUUID"); // gets stored string
    viewModel.set('headerSelected', 4); // needed for underline in header
    console.log(getPUUID)
    if (getPUUID != undefined && getPUUID != "") {
        bluetooth.isConnected({ 
            UUID: getPUUID
            }).then(connected => {
                console.log("Connected? " + connected)
                if (connected) {
                    wifiDetailsViewModel(viewModel);
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

export function ConnectViewModel() {
    const viewModel = new Observable();
    const appSet = ApplicationSettings;
    const delay = ms => new Promise(res => setTimeout(res, ms)); // for delays
    connectPageIntialize(viewModel, appSet);

    viewModel.connectController = () => {
        bluetooth.isBluetoothEnabled().then(enabled => {
            console.log("Enabled? " + enabled)
            if (!enabled) {
                alert("Please Turn on Bluetooth")
            } else {
                scanStarted(viewModel);
                bluetooth.startScanning({
                    filters: [{deviceName:'HMSoft'}], // MY DEFAULT BLUETOOTH NAME
                    seconds: 4,
                    onDiscovered: function (peripheral) {
                        console.log("Periperhal found with UUID: " + peripheral.UUID );
                        console.log("Periperhal found with Name: " + peripheral.name );
                        deviceFound(viewModel, peripheral);
                    }
                }).then(function() {
                    console.log("scanning complete");
                    scanStopped(viewModel);
                }, function (err) {
                    console.log("error while scanning: " + err);
                    scanStopped(viewModel);
                });
            }
        }, error => {
            console.log(error);
        });
    }
    
    viewModel.stopScan = () => {
        stopScanning(viewModel);
    }

    viewModel.onItemTap = (args) => {
        var deviceList = getBlueToothList(viewModel);
        deviceList[args.index].isSelected = true; // setting to true highlights list item
        viewModel.set('devicesFound', deviceList);
        viewModel.set('deviceSelected', true);
    }

    viewModel.connect = () => {
        var uuidID = findSelectedDevice(viewModel);
        //console.log(uuidID);

        if (uuidID.isSelected) {
            stopScanning(viewModel);
            console.log('Connecting...');
            viewModel.set('isConnecting', true);

            bluetooth.connect({
                UUID: uuidID.bluetoothID.UUID,
                onConnected: function (peripheral) {
                    console.log("Periperhal connected with UUID: " + peripheral.UUID);
                    console.log("sUUDD: " + peripheral.services[2].UUID)
                    console.log("CUUDD: " + peripheral.services[2].characteristics[0].UUID)
                    updateStoredDevice(appSet, peripheral.UUID, peripheral.services[2].UUID, peripheral.services[2].characteristics[0].UUID);
                    wifiDetailsViewModel(viewModel);
                    // the peripheral object now has a list of available services:
                    peripheral.services.forEach(function(service) {
                        console.log("service found: " + JSON.stringify(service));
                    });
                },
                onDisconnected: function (peripheral) {
                    console.log("Periperhal disconnected with UUID: " + peripheral.UUID);
                }
            });
        }
    }

    viewModel.disConnect = () => {
        console.log(appSet.getString("pUUID"));
        var getUUID = appSet.getString("pUUID");

        bluetooth.disconnect({
            UUID: getUUID
        }).then(function() {
            console.log("disconnected successfully");
            updateStoredDevice(appSet, "", "", "");
            searchBluetoothViewModel(viewModel);
        }, function (err) {
            // in this case you're probably best off treating this as a disconnected peripheral though
            console.log("disconnection error: " + err);
        });
    }

    viewModel.wifiPasswordUpdate = async () => {
        var name = viewModel.get('WifiName');
        var Password = viewModel.get('WifiPassword');
        var transmissionString = "w" + name + "+" + Password + "\n";
        var pUUID = appSet.getString("pUUID");
        var sUUID = appSet.getString("sUUID");
        var cUUID = appSet.getString("cUUID");

        viewModel.set('isConnectingWifi', true);
        var i = 0;
        console.log(transmissionString);
        bluetooth.write({
            peripheralUUID: pUUID,
            serviceUUID: sUUID,
            characteristicUUID: cUUID,
            value: transmissionString
            }).then(async function(result) {
                console.log("value written");
                // USEFUL WHEN DOING FEED BACK
                /*while (i<5) {
                    i++
                    console.log('attempt: ' + i);
                    await delay(2000)
                }*/
            }, function (err) {
                console.log("write error: " + err);
        });

    }

    return viewModel
}
