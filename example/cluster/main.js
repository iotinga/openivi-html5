
var destinationNumber = "+0123456789";


function Dial(jq_needle, minangle, maxangle, min, max) {
  this.jq_needle = jq_needle;
  this.minangle = minangle;
  this.maxangle = maxangle;
  this.min = min;
  this.max = max;
  this.value = min;
}

Dial.prototype.set = function (value) {
  var angle = this.minangle + (this.maxangle - this.minangle)/(this.max - this.min) * value;
  this.jq_needle.css("transform", "translateZ(0) rotate("+angle+"deg)");
  this.value = value;
};

Dial.prototype.fade = function (rate) {
  this.set(this.value * rate);
}

function update_phone_info() {
  if (window.phone.name) {
    document.getElementById("phone_name").textContent = window.phone.name;
  }
  if (window.phone.battery) {
    document.getElementById("phone_battery").textContent = window.phone.battery + " %";
  }
  if (window.phone.carrier) {
    document.getElementById("phone_op").textContent = window.phone.carrier;
  }
  if (window.phone.name) {
    document.getElementById("phone_signal").textContent = window.phone.signal;
  }
}

function update_call_info() {
  var call_info = "";
  var call_number = "";
  if (window.phone.number) {
    call_number = window.phone.number;
  }
  if (window.phone.status) {
    if (window.phone.status !== "none") {
      call_info = window.phone.status + " " + call_number;
    }
  }
  document.getElementById("phone_call").textContent = call_info;
}

function on_key_pressed() {
  if (window.phone.lastkey) {
    document.getElementById("last_key").textContent = (window.phone.lastkey).toString(16);
  }
}

function on_call_button_click() {
  if (window.phone) {
    if (window.phone.status === "incoming") {
      window.phone.OnCall(window.phone.number);
    } else {
      window.phone.OnCall(destinationNumber);
    }
  }
}

function on_hangup_button_click() {
  if (window.phone) {
    window.phone.OnHangup();
  }
}

function init() {
  var iat_dial = new Dial($('#iatNeedle'), -143, 143, 0, 200);
  var rpm_dial = new Dial($('#rpmNeedle'), 5, 180, 0, 10000);
  var range_dial = new Dial($('#loadNeedle'), -90, 90, 0, 150);
  var oil_temp_dial = new Dial($('#oilTempNeedle'), 228, 185, 0, 80);
  var engine_coolant_dial = new Dial($('#engineCoolantNeedle'), 275, 324, 40, 140);
  var vbat_field = document.getElementById("vbatValue");
  var gear_field = document.getElementById("gearValue");

  iat_dial.set(12);
  oil_temp_dial.set(50);
  engine_coolant_dial.set(85);
  var speed = $('#velocity');
  if (window.car) {
    window.car.refresh_data.connect(function () {
      var actualRpm = 0;
      var actualSpeed = 1;
      var actualVbat = 12.0;
      var actualGear = "N"
      if (window.car.rpm) {
        actualRpm = window.car.rpm;
      }
      rpm_dial.set(actualRpm);
      if (window.car.vbat) {
        actualVbat = window.car.vbat;
      }
      if (typeof vbat_field !== "undefined") {
        vbat_field.textContent = actualVbat.toFixed(1);
      }

      iat_dial.set(30);
      range_dial.set(50);
      if (window.car.tps) {
        actualSpeed = window.car.tps;
      }
      speed.text(Math.floor(actualSpeed));
      if (window.car.gear) {
        if (window.car.gear > 0) {
          actualGear = window.car.gear.toFixed(0);
        } else if (window.car.gear < 0) {
          actualGear = "R";
        }
      }
      // alert(actualGear);
      if (typeof gear_field !== "undefined") {
         gear_field.textContent = actualGear;
      }
    });
  } else if (window.console) {
    console.log("window.car not found");
  }
  // Phone connection
  if (window.phone) {
    update_phone_info();
    window.phone.refresh_phone_info.connect(function () {
      update_phone_info();
    });
    window.phone.update_call_status.connect(function () {
      update_call_info();
    });
    window.phone.key_pressed.connect(function(key_value) {
      // on_key_pressed();
      document.getElementById("last_key").textContent = Number(key_value).toString(16);
    });
  } else if (window.console) {
    console.log("window.phone not found");
  }
}

$(init);
/* vim: set expandtab tabstop=2 shiftwidth=2: */
