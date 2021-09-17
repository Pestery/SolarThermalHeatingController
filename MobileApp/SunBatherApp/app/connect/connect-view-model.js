import { Observable } from '@nativescript/core'
import { Bluetooth } from '@nativescript-community/ble';
import { competitorOptions }  from "../models/competitorModel";
var bluetooth = new Bluetooth();

// TODO 
// Add found counter
// Prioritise Arduino devices
// List devices
// disable connect button
// Enter WIFI stuff
// write to device
// Disconnect

function connectPageIntialize(viewModel, blueToothList) {
    viewModel.set('headerSelected', 4); // needed for underline in header
    viewModel.set('isBusy', false);
    viewModel.set('foundDevicesCount', 0);
    viewModel.set('showBluetoothList', false);
    var compareOptionList = competitorOptions();
    viewModel.set('compareOptions', blueToothList);
}

export function ConnectViewModel() {
    const viewModel = new Observable();
    var blueToothList = {};
    connectPageIntialize(viewModel, blueToothList);

    viewModel.connectController = () => {
        for (var i = 0; i < 4; i++)
        {
            blueToothList[i] = "Bluetooth Item: " + i;
        }
        console.log(blueToothList)
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
    }
    
    viewModel.stopScan = () => {
        bluetooth.stopScanning().then(function() {
            console.log("scanning stopped");
        });
    }

    viewModel.connect = () => {
        console.log('Connecting...')
        bluetooth.connect({
            UUID: 'C0:28:8D:4A:BC:AA',
            onConnected: function (peripheral) {
                console.log("Periperhal connected with UUID: " + peripheral.UUID);
          
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

    return viewModel
}
