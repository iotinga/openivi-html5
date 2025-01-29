const ICONS_FOLDER = "assets/icons";
const ARROW_LEFT = 16777234;
const ARROW_UP = 16777235;
const ARROW_RIGHT = 16777236;
const ARROW_DOWN = 16777237;

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
        `Updating view: ${this.getName()}, ${this.getBattery()}, ${this.getCarrier()}, ${this.getSignal()}`
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

        batteryIcon.src = `${ICONS_FOLDER}/battery-${this.getBatteryLevelAsString()}.svg`;
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
  const buttonsCount = buttons.children.length;

  this.currentFocusIndex = -1;

  this.focusButton = (index) => {
    if (this.currentFocusIndex == -1) {
      index = 0;
    }

    const focusableButton = buttons.children.item(index);
    focusableButton.focus();
    this.currentFocusIndex = index;
  };

  this.getFocusedButton = () => {
    return buttons.children.item(this.currentFocusIndex);
  };

  this.onKeyPressed = (key) => {
    switch (key) {
      case ARROW_UP:
        this.focusButton(
          (buttonsCount + this.currentFocusIndex - 1) % buttonsCount
        );
        break;
      case ARROW_DOWN:
        this.focusButton((this.currentFocusIndex + 1) % buttonsCount);
        break;
      case ARROW_RIGHT:
        this.getFocusedButton().click();
        break;
    }
  };

  for (let i = 0; i < buttons.children.length; i++) {
    const button = buttons.children.item(i);
    button.addEventListener("focus", () => button.classList.add("focused"));
    button.addEventListener("blur", () => button.classList.remove("focused"));
  }

  window.phone.key_pressed.connect(this.onKeyPressed);
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

function openCamera() {
  window.controller.OnOpenCameraView();
}

function init() {
  updateHourAndMinute();
  setInterval(updateHourAndMinute, 1000);
  phoneVM.updateView();
}

init();
