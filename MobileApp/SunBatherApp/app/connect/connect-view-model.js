import { Observable } from '@nativescript/core'
import { Bluetooth } from '@nativescript-community/ble';
import { selectedIndexProperty } from '@nativescript/core/ui/list-picker/list-picker-common';
var bluetooth = new Bluetooth();

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

function connectPageIntialize(viewModel, blueToothList) {
    viewModel.set('headerSelected', 4); // needed for underline in header
    viewModel.set('findController', true);
    viewModel.set('stopController', false);
    viewModel.set('foundDevicesCount', 0);
    viewModel.set('showBluetoothList', false);
    viewModel.set('devicesFound', blueToothList);
    viewModel.set('searchedDevices', false);
    viewModel.set('deviceSelected', false);
    viewModel.set('showWifiInput', false);
    viewModel.set('isConnecting', false);
    viewModel.set('WifiName', "");
    viewModel.set('WifiPassword', "");
    viewModel.set('isConnectingWifi', false);
}

// This function has some uneccesary steps in it such as the for loop putting the array into another arry
// for some reason the only way to get the list to refresh is to create a new object array, this function does that
function getBlueToothList(viewModel) {
    var blueToothList = viewModel.get('devicesFound');
    var bluetoothRefreshList = []
    for (var i = 0; i < blueToothList.length; i++) {
        bluetoothRefreshList[i] = { bluetoothID: blueToothList[i].bluetoothID, isSelected: false };
    }
    return bluetoothRefreshList;
}

function changeViewModel(viewModel) {
    viewModel.set('isConnecting', false);
    viewModel.set('showWifiInput', true);
    viewModel.set('findController', false);
    viewModel.set('stopController', false); // to remove
    console.log('function entered')
}

export function ConnectViewModel() {
    const viewModel = new Observable();
    connectPageIntialize(viewModel, blueToothList);
    var blueToothList = [];

    viewModel.connectController = () => {
        /*bluetooth.isBluetoothEnabled().then(enabled => {
            console.log("Enabled? " + enabled)
            if (!enabled) {
                alert("Please Turn on Bluetooth")
            } else {
                viewModel.set('isBusy', true);
                bluetooth.startScanning({
                    filters: [{serviceUUID:'61fe'}],
                    seconds: 4,
                    onDiscovered: function (peripheral) {
                        console.log("Periperhal found with UUID: " + peripheral.UUID );
                        console.log("Periperhal found with Name: " + peripheral.name );
                    }
                }).then(function() {
                    console.log("scanning complete");
                    viewModel.set('isBusy', false);
                }, function (err) {
                    console.log("error while scanning: " + err);
                    viewModel.set('isBusy', false);
                });
            }
        }, error => {
            console.log(error);
        });*/
        for (var i = 0; i < 10; i++)
        {
            blueToothList[i] = { bluetoothID: "Bluetooth Item: " + i, isSelected: false };        
        }
        console.log(blueToothList)
        viewModel.set('devicesFound', blueToothList);
        viewModel.set('showBluetoothList', true);
        viewModel.set('findController', false);
        viewModel.set('stopController', true);
        viewModel.set('searchedDevices', true);
        viewModel.set('deviceSelected', false);
    }
    
    viewModel.stopScan = () => {
        viewModel.set('findController', true);
        viewModel.set('stopController', false); // to remove
        if (viewModel.get('devicesFound').length > 0) {
            viewModel.set('showBluetoothList', true);
        } else {
            viewModel.set('showBluetoothList', false)
            viewModel.set('searchedDevices', false);
        }
        bluetooth.stopScanning().then(function() {
            console.log("scanning stopped");
        });
    }

    viewModel.onItemTap = (args) => {
        var deviceList = getBlueToothList(viewModel);
        deviceList[args.index].isSelected = true; // setting to true highlights list item
        viewModel.set('devicesFound', deviceList);
        viewModel.set('deviceSelected', true);
    }

    viewModel.connect = () => {
        viewModel.set('isConnecting', true);
        viewModel.set('showBluetoothList', false);

        if (viewModel.get('deviceSelected')) {
            console.log('Connecting...');
            changeViewModel(viewModel);
            viewModel.set('wifiName', "Enter Password for 'Perhaps'");
            bluetooth.connect({
                UUID: 'C0:28:8D:4A:BC:AA',
                onConnected: function (peripheral) {
                    console.log("Periperhal connected with UUID: " + peripheral.UUID);
                    //changeViewModel(viewModel);
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
        console.log('Disconnecting...')
        bluetooth.disconnect({
            UUID: 'C0:28:8D:4A:BC:AA'
        }).then(function() {
            console.log("disconnected successfully");
        }, function (err) {
            // in this case you're probably best off treating this as a disconnected peripheral though
            console.log("disconnection error: " + err);
        });
    }

    viewModel.wifiPasswordUpdate = () => {
        var name = viewModel.get('WifiName');
        var Password = viewModel.get('WifiPassword');
        viewModel.set('isConnectingWifi', true);

        console.log(name)
        console.log(Password)
        bluetooth.write({
            peripheralUUID: '34134-5453-4453-54545',
            serviceUUID: '180e',
            characteristicUUID: '3424-45234-34324-2343',
            value: [1] 
          }).then(function(result) {
            console.log("value written");
          }, function (err) {
            console.log("write error: " + err);
          });
    }

    return viewModel
}
