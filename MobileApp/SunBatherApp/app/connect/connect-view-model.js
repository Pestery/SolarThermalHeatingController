import { Http, Observable } from '@nativescript/core'
var apiCall = require("../shared/apiConfig");
var commonFunction = require("../shared/commonFunctions");
import { Bluetooth } from '@nativescript-community/ble';
var bluetooth = new Bluetooth();

export function ConnectViewModel() {
    const viewModel = new Observable();
    viewModel.set('isBusy', false);
    viewModel.set('headerSelected', 4); // needed for underline in header

    viewModel.connectController = () => {
        console.log('connecting....');
        bluetooth.isBluetoothEnabled().then(enabled => {
            console.log("Enabled? " + enabled)
            if (!enabled) {
                alert("Please Turn on Bluetooth")
            } else {
                viewModel.set('isBusy', true);
                bluetooth.startScanning({
                    seconds: 4,
                    onDiscovered: function (peripheral) {
                        console.log("Periperhal found with UUID: " + peripheral.UUID);
                    }
                }).then(function() {
                    console.log("scanning complete");
                }, function (err) {
                    console.log("error while scanning: " + err);
                });
            }
        }, error => {
            console.log(error);
        });
    }
    
    viewModel.stopScan = () => {
        bluetooth.stopScanning().then(function() {
            console.log("scanning stopped");
        });
        viewModel.set('isBusy', false);
    }

    return viewModel
}
