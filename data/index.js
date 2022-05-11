"use strict";

let connection;
let queue;
let cooldownTimer;
let spectrogram; // eslint-disable-line no-unused-vars

function openWebsocket(uri) {
  const ws = new WebSocket(uri, ["arduino"]);
  ws.binaryType = "arraybuffer";
  return ws;
}

function openMainWebsocket() {
  connection = openWebsocket(`ws://${location.hostname}:81/`);
  connection.onopen = function () {
    displayConnectedState(true);
  };
  connection.onclose = function () {
    displayConnectedState(false);
  };
  connection.onmessage = function (e) {
    console.log("Received: ", e.data);
    handleJsonData(JSON.parse(e.data));
  };
  connection.onerror = function (e) {
    console.log("WebSocket Error", e);
  };
}

function setupSpinners() {
  const spinners = document.getElementsByClassName("spinner");
  [].forEach.call(spinners, (spinnerDiv) => {
    const inputElement = spinnerDiv.getElementsByTagName("input")[0];
    inputElement.classList.add("spinner-input");

    const buttonDec = document.createElement("span");
    buttonDec.classList.add("btn-spinner", "btn-spinner-dec");
    buttonDec.innerHTML = "&minus;";
    buttonDec.addEventListener("click", () => {
      const attrMin = inputElement.getAttribute("min");
      const attrStep = inputElement.getAttribute("step");
      const value = Number(inputElement.value);
      const step = attrStep ? Number(attrStep) : 1;
      const newValue = value - step;
      if (value !== newValue) {
        inputElement.value = attrMin
          ? Math.max(Number(attrMin), newValue)
          : newValue;
      }
    });

    const buttonInc = document.createElement("span");
    buttonInc.classList.add("btn-spinner", "btn-spinner-inc");
    buttonInc.innerHTML = "&plus;";
    buttonInc.addEventListener("click", () => {
      const attrMin = inputElement.getAttribute("max");
      const attrStep = inputElement.getAttribute("step");
      const value = Number(inputElement.value);
      const step = attrStep ? Number(attrStep) : 1;
      const newValue = value + step;
      if (value !== newValue) {
        inputElement.value = attrMin
          ? Math.min(Number(attrMin), newValue)
          : newValue;
      }
    });

    inputElement.insertAdjacentElement("beforebegin", buttonDec);
    inputElement.insertAdjacentElement("afterend", buttonInc);
  });
}

function displayConnectedState(connected) {
  document.getElementById("navbarTitle").innerHTML = connected ? "FastLEDHub" : "Disconnected...";
  document.getElementById("refreshButton").classList.toggle("d-none", connected);
  document.getElementById("spectrumButton").classList.toggle("d-none", !connected);
  document.getElementById("stopButton").classList.toggle("d-none", !connected);
  document.getElementById("settingsButton").classList.toggle("d-none", !connected);
}

function registerColorPicker(colorPicker, idx) {
  const template = document.getElementById("colorPickerTemplate").innerHTML;
  const colorPickerId = "colorPicker" + idx;
  const colorPickerButtonId = "colorPickerButton" + idx;
  const colorPickersWrapper = document.getElementById("colorPickersWrapper");
  colorPickersWrapper.insertAdjacentHTML(
    "beforeend",
    eval("`" + template + "`")
  );
  const cp = new iro.ColorPicker("#" + colorPickerId, {
    width: 250,
    color: colorPicker.value,
    display: "inline-block",
    layout: [
      {
        component: iro.ui.Wheel,
        options: {},
      },
    ],
  });
  cp.on("color:change", function (color) {
    document.getElementById(colorPickerButtonId).style.backgroundColor =
      color.hexString;
    sendBytes([0, idx, color.red, color.green, color.blue]);
  });
  document.getElementById(colorPickerButtonId).style.backgroundColor =
    cp.color.hexString;
}

// eslint-disable-next-line no-unused-vars
function registerAnimation(animation, idx) {
  document.getElementById("alarmAnimation").add(new Option(animation));
  document.getElementById("postAlarmAnimation").add(new Option(animation));
  document.getElementById("sunsetAnimation").add(new Option(animation));
  document.getElementById("startupAnimation").add(new Option(animation));

  const template = document.getElementById("animationButtonTemplate").innerHTML;
  const animationButtons = document.getElementById("animationButtons");
  animationButtons.lastElementChild.insertAdjacentHTML(
    "beforebegin",
    eval("`" + template + "`")
  );
}

// eslint-disable-next-line no-unused-vars
function registerSlider(slider, idx) {
  const template = document.getElementById("sliderTemplate").innerHTML;
  document.getElementById("slidersWrapper").insertAdjacentHTML("beforeend", eval("`" + template + "`"));
}

function handleJsonData(data) {
  data.animations?.forEach((animation, idx) => {
    registerAnimation(animation, idx);
  });
  data.sliders?.forEach((slider, idx) => {
    registerSlider(slider, idx);
  });
  data.colorPickers?.forEach((colorPicker, idx) => {
    registerColorPicker(colorPicker, idx);
  });
  if (Object.prototype.hasOwnProperty.call(data, "startupAnimation")) {
    document.getElementById("startupAnimation").value = data.startupAnimation;
    document.getElementById("useStartupAnimation").checked = data.startupAnimation !== "";
    if (data.startupAnimation !== "")
      new bootstrap.Collapse(
        document.getElementById("useStartupAnimationCollapse"),
        { toggle: false }
      ).show();
  }
  if (Object.prototype.hasOwnProperty.call(data, "alarmEnabled")) {
    document.getElementById("alarmEnabled").checked = data.alarmEnabled;
    if (data.alarmEnabled)
      new bootstrap.Collapse(document.getElementById("alarmEnabledCollapse"), {
        toggle: false,
      }).show();
  }
  if (Object.prototype.hasOwnProperty.call(data, "sunsetEnabled")) {
    document.getElementById("sunsetEnabled").checked = data.sunsetEnabled;
    if (data.sunsetEnabled)
      new bootstrap.Collapse(document.getElementById("sunsetEnabledCollapse"), {
        toggle: false,
      }).show();
  }
  if (Object.prototype.hasOwnProperty.call(data, "alarmAnimation"))
    document.getElementById("alarmAnimation").value = data.alarmAnimation;
  if (Object.prototype.hasOwnProperty.call(data, "postAlarmAnimation"))
    document.getElementById("postAlarmAnimation").value = data.postAlarmAnimation;
  if (Object.prototype.hasOwnProperty.call(data, "sunsetAnimation"))
    document.getElementById("sunsetAnimation").value = data.sunsetAnimation;
  if (Object.prototype.hasOwnProperty.call(data, "sunsetDuration"))
    document.getElementById("sunsetDuration").value = data.sunsetDuration;
  if (Object.prototype.hasOwnProperty.call(data, "sunsetOffset"))
    document.getElementById("sunsetOffset").value = data.sunsetOffset;
  if (Object.prototype.hasOwnProperty.call(data, "timeZone"))
    document.getElementById("timeZone").value = data.timeZone;
  if (Object.prototype.hasOwnProperty.call(data, "summerTime"))
    document.getElementById("summerTime").checked = data.summerTime;
  if (Object.prototype.hasOwnProperty.call(data, "longitude"))
    document.getElementById("longitude").value = data.longitude;
  if (Object.prototype.hasOwnProperty.call(data, "latitude"))
    document.getElementById("latitude").value = data.latitude;
  if (Object.prototype.hasOwnProperty.call(data, "extraSpectrogramDevices"))
    document.getElementById("extraSpectrogramDevices").value = data.extraSpectrogramDevices;
  if (Object.prototype.hasOwnProperty.call(data, "alarmDuration"))
    document.getElementById("alarmDuration").value = data.alarmDuration;
  if (
    Object.prototype.hasOwnProperty.call(data, "alarmHour") &&
    Object.prototype.hasOwnProperty.call(data, "alarmMinute")
  )
    document.getElementById("alarmTime").value =
      (data.alarmHour < 10 ? "0" + data.alarmHour.toString() : data.alarmHour) + ":" +
      (data.alarmMinute < 10 ? "0" + data.alarmMinute.toString() : data.alarmMinute);
  if (
    Object.prototype.hasOwnProperty.call(data, "status") &&
    Object.prototype.hasOwnProperty.call(data, "currentAnimation")
  ) {
    updateAnimationButtons(data.status, data.currentAnimation);
  }
}

function updateAnimationButtons(status, animation) {
  const animationButtons = document.querySelectorAll("#animationButtons button");
  animationButtons.forEach((btn, idx) => {
    const buttonIcon = btn.querySelector("div i");
    const buttonContent = btn.querySelector("div span").innerHTML;
    if (animation === buttonContent) {
      if (status === 2) {
        buttonIcon.classList = "bi bi-play-fill";
      } else if (status === 1) {
        buttonIcon.classList = "bi bi-pause";
      }
    } else if (idx === animationButtons.length - 1 && status === 0) {
      buttonIcon.classList = "bi bi-stop-fill";
    } else {
      buttonIcon.classList = "bi";
    }
  });
}

function sendConfig() {
  const time = alarmTime.value.split(":");
  const json = JSON.stringify(
    {
      alarmHour: time[0],
      alarmMinute: time[1],
      alarmEnabled: document.getElementById("alarmEnabled").checked,
      alarmDuration: document.getElementById("alarmDuration").value,
      alarmAnimation: document.getElementById("alarmAnimation").value,
      postAlarmAnimation: document.getElementById("postAlarmAnimation").value,
      timeZone: document.getElementById("timeZone").value,
      summerTime: document.getElementById("summerTime").checked,
      longitude: document.getElementById("longitude").value,
      latitude: document.getElementById("latitude").value,
      extraSpectrogramDevices: document.getElementById("extraSpectrogramDevices").value,
      sunsetEnabled: document.getElementById("sunsetEnabled").checked,
      sunsetDuration: document.getElementById("sunsetDuration").value,
      sunsetOffset: document.getElementById("sunsetOffset").value,
      sunsetAnimation: document.getElementById("sunsetAnimation").value,
      startupAnimation: document.getElementById("useStartupAnimation").checked
        ? document.getElementById("startupAnimation").value
        : "",
    },
    null,
    2
  );
  sendText(json);
}

function sendBytes(bytes) {
  queue = bytes;

  if (cooldownTimer) return;

  sendBytesQueue();
  cooldownTimer = setTimeout(() => {
    cooldownTimer = null;
    sendBytesQueue();
  }, 15);
}

function sendBytesQueue() {
  if (!queue || connection.readyState !== 1) return;

  const data = Uint8Array.from(queue);
  connection.send(data.buffer);
  console.log("Sent bytes: " + data);

  queue = null;
}

function sendText(text) {
  if (connection.readyState !== 1) {
    console.log("Not connected. Text not sent!");
    return;
  }

  connection.send(text);
  console.log("Sent text: " + text);
}

class Spectrogram {
  openConnections() {
    this.connections = [connection];
    const extraDevices = document.getElementById("extraSpectrogramDevices").value.split(",");
    extraDevices.forEach((uri) => {
      uri = uri.trim();
      if (uri !== "") this.connections.push(openWebsocket(`ws://${uri}:81/`));
    });
  }

  begin() {
    this.bins = 16;
    this.fftRangeBit = 10;
    this.fftRange = 1024;
    this.connections = [];
    this.audioCtx = new (window.AudioContext || window.webkitAudioContext)();
    this.analyser = this.audioCtx.createAnalyser();
    this.analyser.fftSize = 2048;
    this.analyser.minDecibels = -90;
    this.analyser.maxDecibels = -10;
    this.analyser.smoothingTimeConstant = 0.7;

    this.openConnections();

    if (navigator.mediaDevices.getUserMedia) {
      const constraints = { audio: true };
      navigator.mediaDevices
        .getUserMedia(constraints)
        .then((stream) => {
          this.microphoneStream = stream;
          this.source = this.audioCtx.createMediaStreamSource(stream);
          this.source.connect(this.analyser);
          this.sendTimer = setInterval(this.tick.bind(this), 5);
          spectrumButton.classList.add("text-white");
        })
        .catch((e) => {
          console.log("The following error occured in getUserMedia: " + e);
        });
    } else {
      console.log("GetUserMedia not supported.");
    }
  }

  end() {
    clearInterval(this.sendTimer);
    this.audioCtx.close();
    this.microphoneStream.getAudioTracks().forEach((track) => {
      track.stop();
    });
    spectrumButton.classList.remove("text-white");
  }

  tick() {
    const fft = new Uint8Array(this.analyser.frequencyBinCount);
    this.analyser.getByteFrequencyData(fft);

    const spectrumData = new Array(this.bins);
    let left = 0;
    for (let line = 0; line < this.bins; line++) {
      const right = Math.round(
        (Math.pow(2, (this.fftRangeBit * line) / (this.bins - 1)) /
          this.fftRange) *
          (this.fftRange - 1)
      );
      let peak = 0;
      for (; left < right; left++) {
        if (fft[left + 1] / 255 > peak) {
          peak = fft[left + 1] / 255;
        }
      }
      let intensity = Math.pow(peak, 1.2);
      intensity = Math.min(Math.max(intensity, 0), 1);
      spectrumData[line] = intensity * 255;
    }

    this.transmit(spectrumData);
    this.visualize(spectrumData);
  }

  transmit(data) {
    this.connections.forEach((c) => {
      if (c.readyState === 1) c.send(Uint8Array.from([30].concat(data)).buffer);
    });
  }

  visualize(data) {
    const spectrumDisplay = document.getElementById("spectrumDisplay")
    while (spectrumDisplay.children.length < this.bins)
      spectrumDisplay.appendChild(document.createElement("div"));
    data.forEach((d, idx) => {
      spectrumDisplay.children.item(idx).style.height = (d / 255) * 100 + "px";
    });
  }

  toggle() {
    if (this.audioCtx?.state === "running") {
      this.end();
    } else {
      this.begin();
    }
  }
}

window.onload = () => {
  openMainWebsocket();
  setupSpinners();
  spectrogram = new Spectrogram();
  document.getElementById("settingsOffcanvas").addEventListener("hidden.bs.offcanvas", function () {
    sendConfig();
  });
};
