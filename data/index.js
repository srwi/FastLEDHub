let currentAnimation = '';
let currentStatus = 0;
let currentColor = '';

let ws_pending_msg;
let ws_uri = 'ws://' + (location.hostname ? location.hostname : 'localhost') + ':81/';
let connection = new WebSocket(ws_uri, ['arduino']);
connection.binaryType = 'arraybuffer';

connection.onopen = function (e) {
  ws_pending_msg = [30]
  connectingOverlayText.innerHTML = 'Waiting for response...';
  refreshButton.style.display = 'none';
};

connection.onerror = function (e) {
  console.log('WebSocket Error', e);
};

connection.onclose = function (e) {
  connectingOverlayText.innerHTML = 'Connection interrupted.';
  refreshButton.style.display = '';
  connectingOverlay.style.display = '';
};

connection.onmessage = function (e) {
  console.log('Received: ', e.data);

  try {
    handleJsonData(JSON.parse(e.data));

    connectingOverlayText.innerHTML = 'Success!';
    connectingOverlay.style.display = 'none';
  } catch (e) { }
};

function handleJsonData(data) {
  if (data.hasOwnProperty('animations')) {
    data.animations.forEach((animation, idx) => {
      // Fill animation dropdowns
      alarmAnimation.add(new Option(animation));
      postAlarmAnimation.add(new Option(animation));
      sunsetAnimation.add(new Option(animation));
      startupAnimation.add(new Option(animation));

      // Add buttons to button collection
      if (animation != 'Color') {
        let btn = document.createElement('button');
        btn.classList.add('btn-secondary', 'btn');
        btn.innerHTML = animation;
        btn.onclick = () => { sendAnimationButton(idx); };
        animationButtons.insertBefore(btn, stopButton);
      }
    })
  }

  if (data.hasOwnProperty('alarmAnimation'))
    alarmAnimation.value = data.alarmAnimation || 'Color';
  if (data.hasOwnProperty('postAlarmAnimation'))
    postAlarmAnimation.value = data.postAlarmAnimation || 'Color';
  if (data.hasOwnProperty('sunsetAnimation'))
    sunsetAnimation.value = data.sunsetAnimation || 'Color';
  if (data.hasOwnProperty('startupAnimation')) {
    startupAnimation.value = data.startupAnimation || 'Color';
    useStartupAnimation.checked = data.startupAnimation != '';
    startupAnimation.disabled = data.startupAnimation == '';
  }
  if (data.hasOwnProperty('timeZone'))
    timeZone.value = data.timeZone;
  if (data.hasOwnProperty('summerTime'))
    summerTime.checked = data.summerTime;
  if (data.hasOwnProperty('longitude'))
    longitude.value = data.longitude;
  if (data.hasOwnProperty('latitude'))
    latitude.value = data.latitude;
  if (data.hasOwnProperty('alarmEnabled'))
    alarmEnabled.checked = data.alarmEnabled;
  if (data.hasOwnProperty('alarmDuration'))
    alarmDuration.value = data.alarmDuration;
  if (data.hasOwnProperty('alarmHour') && data.hasOwnProperty('alarmMinute'))
    alarmTime.value = (data.alarmHour < 10 ? '0' + data.alarmHour.toString() : data.alarmHour) + ':' + (data.alarmMinute < 10 ? '0' + data.alarmMinute.toString() : data.alarmMinute);
  if (data.hasOwnProperty('sunsetEnabled'))
    sunsetEnabled.checked = data.sunsetEnabled;
  if (data.hasOwnProperty('sunsetDuration'))
    sunsetDuration.value = data.sunsetDuration;
  if (data.hasOwnProperty('sunsetOffset'))
    sunsetOffset.value = data.sunsetOffset;
  if (data.hasOwnProperty('speed'))
    speed.noUiSlider.set(data.speed);
  if (data.hasOwnProperty('saturation'))
    saturation.noUiSlider.set(data.saturation);
  if (data.hasOwnProperty('color')) {
    colorButton.value = data.color;
    currentColor = data.color;
  }

  // Update buttons list
  if (data.hasOwnProperty('status') && data.hasOwnProperty('currentAnimation'))
    updateButtons(data.status, data.currentAnimation);
}

function updateButtons(status, animation) {
  currentStatus = status;
  currentAnimation = animation;

  // Cycle through all animation buttons and change class/style accordingly
  document.querySelectorAll('#animationButtons button:not(:last-child)').forEach(btn => {
    if (currentAnimation == btn.innerHTML) {
      if (currentStatus == 2) {
        // Running
        if (btn.innerHTML == 'Color') {
          let colorInstance = $customColorPicker.colorPicker.color;
          let colors = colorInstance.colors;
          colors.HEX = currentColor;
          colorInstance.setColor(null, 'HEX');
          btn.value = '#' + colors.HEX;
          btn.style.backgroundColor = '#' + colors.HEX;
          btn.style.borderColor = '#' + colors.HEX;
          btn.style.color = colors.rgbaMixBGMixCustom.luminance > 0.22 ? '#222' : '#ddd';
        }
        else {
          btn.classList.value = 'btn btn-success';
        }
      }
      else if (currentStatus == 1) {
        // Paused
        btn.classList.value = 'btn btn-warning';
      }
    }
    else {
      btn.classList.value = 'btn btn-secondary';
      btn.style.backgroundColor = '';
      btn.style.borderColor = '';
      btn.style.color = '';
    }
  });
}

function sendConfig() {
  let time = alarmTime.value.split(':');
  let json = JSON.stringify({
    alarmHour: time[0],
    alarmMinute: time[1],
    alarmEnabled: alarmEnabled.checked,
    alarmDuration: alarmDuration.value,
    alarmAnimation: alarmAnimation.value,
    postAlarmAnimation: postAlarmAnimation.value,
    timeZone: timeZone.value,
    summerTime: summerTime.checked,
    longitude: longitude.value,
    latitude: latitude.value,
    sunsetEnabled: sunsetEnabled.checked,
    sunsetDuration: sunsetDuration.value,
    sunsetOffset: sunsetOffset.value,
    sunsetAnimation: sunsetAnimation.value,
    startupAnimation: useStartupAnimation.checked ? startupAnimation.value : '',
    speed: speed.noUiSlider.get(),
    saturation: saturation.noUiSlider.get(),
    color: currentColor
  }, null, 2);
  sendText(json);
}

function sendBytes() {
  if (connection.readyState == 1 && ws_pending_msg) {
    let data = new Uint8Array(ws_pending_msg.length);
    for (let i = 0; i < ws_pending_msg.length; i++) {
      data[i] = ws_pending_msg[i];
    }
    ws_pending_msg = undefined;
    connection.send(data.buffer);
    console.log('Sent bytes: ' + data);
  }
}

function sendText(text) {
  if (connection.readyState == 1) {
    connection.send(text);
    console.log('Sent text: ' + text);
  }
  else {
    console.log('Not connected. Text not sent!');
  }
}

function sendAnimationButton(animation) {
  ws_pending_msg = [1, animation];
}


// Inputs
$('.clockpicker').clockpicker();
$('#alarmDuration').TouchSpin({ min: 1, max: 1439, postfix: 'minutes' });
$('#timeZone').TouchSpin({ prefix: 'GMT+', max: 23, min: -23 });
$('#sunsetDuration').TouchSpin({ min: 1, max: 1439, postfix: 'minutes' });
$('#sunsetOffset').TouchSpin({ min: -1439, max: 1439, postfix: 'minutes' });

// Color picker
let $customColorPicker = $('#colorButton').colorPicker({
  opacity: false,
  preventFocus: true,
  buildCallback: function ($elm) { $('#colorButton').on('click', function (e) { e.preventDefault && e.preventDefault(); }); },
  renderCallback: function ($elm, toggled) {
    if (toggled === true) {
      updateButtons(2, 'Color');
    }
    else if (toggled === false) {
      if (currentAnimation != 'Color' || currentStatus == 0) {
        colorButton.style.backgroundColor = '';
        colorButton.style.borderColor = '';
        colorButton.style.color = '';
      }
    }
    let newColor = this.color.colors.HEX + ''; // dereference by appending ''
    if (currentColor != newColor || toggled === true) {
      let newColorRGB = this.color.colors.RND.rgb;
      ws_pending_msg = [0, newColorRGB.r, newColorRGB.g, newColorRGB.b];
      currentColor = newColor;
      document.querySelector('#colorButton').style.borderColor = '#' + newColor;
    }
  }
});

// Sliders
noUiSlider.create(speed, { start: 128, step: 1, range: { 'min': 0, 'max': 255 } });
noUiSlider.create(saturation, { start: 255, step: 1, range: { 'min': 110, 'max': 255 } });
speed.noUiSlider.on('update', (values, handle) => { ws_pending_msg = [20, values[handle]]; });
saturation.noUiSlider.on('update', (values, handle) => { ws_pending_msg = [21, values[handle]]; });
document.querySelector('#speed .noUi-handle').innerHTML = 'Speed';
document.querySelector('#saturation .noUi-handle').innerHTML = 'Sat.';

// Close connection after being ianctive for 5 minutes
let idleTime = 0;
setInterval(() => {
  idleTime++;
  if (idleTime > 300) // in s
    connection.close();
}, 1000);

// Rate limit websocket
setInterval(sendBytes, 15);

document.addEventListener('mousemove', e => { idleTime = 0; });
document.addEventListener('keypress', e => { idleTime = 0; });
document.addEventListener('visibilitychange', () => {
  // Close connection on mobile when page loses focus
  if (/Android|webOS|iPhone|iPad|iPod|BlackBerry|IEMobile|Opera Mini/i.test(navigator.userAgent)) {
    if (document.hidden)
      connection.close();
    else
      location.reload();
  }
})
