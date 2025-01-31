function HeaderViewModel() {
  this.isConnected = () => window.phone.name != "Disconnected";
  this.getName = () => window.phone.name;
  this.getBattery = () => window.phone.battery;
  this.getCarrier = () => window.phone.carrier;
  this.getSignal = () => window.phone.signal;
  this.getBatteryLevelAsString = () => {
    const level = this.getBattery();
    const levels = [20, 30, 50, 60, 80, 90];
    const upper = levels.find((l) => l > level);
    if (upper == null) {
      return "full";
    }

    return String(upper);
  };

  this.updateHourAndMinute = () => {
    const now = new Date();
    const hours = String(now.getHours()).padStart(2, "0");
    const minutes = String(now.getMinutes()).padStart(2, "0");
    const timeHourElements = document.getElementsByClassName("time-hour");
    const timeMinuteElements = document.getElementsByClassName("time-minute");

    for (let i = 0; i < timeHourElements.length; i++) {
      timeHourElements.item(i).textContent = `${hours}`;
    }

    for (let i = 0; i < timeMinuteElements.length; i++) {
      timeMinuteElements.item(i).textContent = `${minutes}`;
    }
  };

  this.updateView = () => {
    console.log(
      `Updating view: ${this.getName()}, ${this.getBattery()}, ${this.getCarrier()}, ${this.getSignal()}`
    );
    const notConnectedElement = document.getElementById("phone-not-connected");
    const connectedElement = document.getElementById("phone-connected");

    if (this.isConnected()) {
      connectedElement.style["display"] = "flex";
      notConnectedElement.style["display"] = "none";

      const batteryIcon = connectedElement.children.item(0);
      const notificationIcon = connectedElement.children.item(1);

      batteryIcon.src = `${ICONS_FOLDER}/battery-${this.getBatteryLevelAsString()}.svg`;
    } else {
      connectedElement.style["display"] = "none";
      notConnectedElement.style["display"] = "flex";
    }
  };

  this.updateHourAndMinute();
  setInterval(this.updateHourAndMinute, 1000);
  window.phone.refresh_phone_info.connect(this.updateView);
}

function makeHeaderViewModel() {
  if (window.phone) {
    return new HeaderViewModel();
  }

  return {
    updateView: () => {},
  };
}

function init() {
  const vm = makeHeaderViewModel();
  vm.updateView();
}

init();
