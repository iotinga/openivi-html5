function PhoneViewModel() {
  if (window.phone) {
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

    this.updateView = () => {
      console.log(
        `Updating view: ${this.getName()}, ${this.getBattery()}, ${this.getCarrier()}, ${this.getSignal()}, ${this.getBatteryLevelAsString()}`
      );
      const notConnectedElement = document.getElementById(
        "phone-not-connected"
      );
      const connectedElement = document.getElementById("phone-connected");

      if (this.isConnected()) {
        connectedElement.style["display"] = "flex";
        notConnectedElement.style["display"] = "none";

        const batteryIcon = connectedElement.children.item(0);
        const notificationIcon = connectedElement.children.item(1);

        batteryIcon.src = `assets/icons/battery-${this.getBatteryLevelAsString()}.svg`;
      } else {
        connectedElement.style["display"] = "none";
        notConnectedElement.style["display"] = "flex";
      }
    };

    window.phone.refresh_phone_info.connect(this.updateView);
  } else {
    this.updateView = () => {};
  }
}

function ActionsViewModel() {
  const buttons = document.getElementById("buttons");

  this.focusButton = (index) => {
    const focusableButton = buttons.children.item(index);
    focusableButton.focus();
  };
}

const phoneVM = new PhoneViewModel();
const actionsVM = new ActionsViewModel();

function updateHourAndMinute() {
  const now = new Date();
  const hours = String(now.getHours()).padStart(2, "0");
  const minutes = String(now.getMinutes()).padStart(2, "0");
  document.getElementById("time-hour").textContent = `${hours}`;
  document.getElementById("time-minute").textContent = `${minutes}`;
}

function init() {
  updateHourAndMinute();
  setInterval(updateHourAndMinute, 1000);
  actionsVM.focusButton(0);
  phoneVM.updateView();
}

init();
