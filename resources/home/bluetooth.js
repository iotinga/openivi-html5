function setupCallbacks() {
  window.bluetoothManager.deviceDiscovered.connect(onDeviceDiscovered);
}

function onDeviceDiscovered(device) {
  console.log(device);
}

function startScan() {
  window.bluetoothManager.startScan();
}

function init() {
  if (window.bluetoothManager) {
    setupCallbacks();
    startScan();
  }
}

init();
