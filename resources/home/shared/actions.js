const ICONS_FOLDER = "assets/icons";
const ARROW_LEFT = 16777234;
const ARROW_UP = 16777235;
const ARROW_RIGHT = 16777236;
const ARROW_DOWN = 16777237;

function ActionsViewModel() {
  this.getButtons = () => {
    return document.getElementById("buttons").children;
  };

  this.currentFocusIndex = -1;

  this.focusButton = (index) => {
    if (this.currentFocusIndex == -1) {
      index = 0;
    }

    const focusableButton = this.getButtons().item(index);
    focusableButton.focus();
    this.currentFocusIndex = index;
  };

  this.getFocusedButton = () => {
    return this.getButtons().item(this.currentFocusIndex);
  };

  this.onKeyPressed = (key) => {
    const buttonsCount = this.getButtons().length;

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
        const focusedButton = this.getFocusedButton();
        if (focusedButton) {
          focusedButton.click();
        }
        break;
      case ARROW_LEFT:
        window.history.back();
        break;
    }
  };

  this.makeFocusable = (button) => {
    button.addEventListener("focus", () => button.classList.add("focused"));
    button.addEventListener("blur", () => button.classList.remove("focused"));
  };

  this.makeButton = () => {};

  for (let i = 0; i < this.getButtons().length; i++) {
    const button = this.getButtons().item(i);
    this.makeFocusable(button);
  }

  if (window.phone) {
    window.phone.key_pressed.connect(this.onKeyPressed);
  } else {
    const map = {
      ArrowUp: ARROW_UP,
      ArrowDown: ARROW_DOWN,
      ArrowLeft: ARROW_LEFT,
      ArrowRight: ARROW_RIGHT,
    };
    window.addEventListener("keydown", (e) => {
      const key = map[e.key];
      this.onKeyPressed(key);
    });
  }
}

const actions = new ActionsViewModel();
