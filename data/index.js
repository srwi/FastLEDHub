let currentAnimation = '';
let currentStatus = 0;
let currentColor = '';

let ws_pending_msg;
let ws_uri =  'ws://192.168.0.35:81/'// 'ws://' + (location.hostname ? location.hostname : 'localhost') + ':81/';
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
  handleJsonData(JSON.parse(e.data));
  connectingOverlayText.innerHTML = 'Success!';
  connectingOverlay.style.display = 'none';
};

function handleJsonData(data) {
  if (data.hasOwnProperty('animations')) {
    let buffer = '';
    let template = $('#animationButtonTemplate').html();
    data.animations.forEach((animation, idx) => {
      alarmAnimation.add(new Option(animation));
      postAlarmAnimation.add(new Option(animation));
      sunsetAnimation.add(new Option(animation));
      startupAnimation.add(new Option(animation));

      buffer += eval('`' + template + '`')
    });
    animationButtons = document.getElementById('animationButtons')
    animationButtons.innerHTML = buffer + animationButtons.innerHTML;
  }

  if (data.hasOwnProperty('sliders')) {
    let buffer = '';
    let template = $('#sliderTemplate').html();
    data.sliders.forEach((slider, idx) => {
      buffer += eval('`' + template + '`')
    });
    slidersWrapper = document.getElementById('slidersWrapper')
    slidersWrapper.innerHTML = buffer;
  }

  if (data.hasOwnProperty('alarmAnimation'))
    alarmAnimation.value = data.alarmAnimation;
  if (data.hasOwnProperty('postAlarmAnimation'))
    postAlarmAnimation.value = data.postAlarmAnimation;
  if (data.hasOwnProperty('sunsetAnimation'))
    sunsetAnimation.value = data.sunsetAnimation;
  if (data.hasOwnProperty('startupAnimation')) {
    startupAnimation.value = data.startupAnimation;
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
  if (data.hasOwnProperty('color')) {
    colorButton.style.backgroundColor = '#' + data.color;
    currentColor = data.color;
  }

  if (data.hasOwnProperty('status') && data.hasOwnProperty('currentAnimation'))
    updateButtons(data.status, data.currentAnimation);
}

function updateButtons(status, animation) {
  currentStatus = status;
  currentAnimation = animation;

  document.querySelectorAll('#animationButtons button').forEach(btn => {
    buttonIcon = btn.querySelector('div i')
    buttonContent = btn.querySelector('div span').innerHTML
    if (currentAnimation == buttonContent) {
      if (currentStatus == 2) {
        buttonIcon.classList = 'bi bi-play-fill'
      }
      else if (currentStatus == 1) {
        buttonIcon.classList = 'bi bi-pause'
      }
    }
    else if (buttonContent == 'Stop' && currentStatus == 0) {
      buttonIcon.classList = 'bi bi-stop-fill'
    }
    else {
      buttonIcon.classList = 'bi'
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

document.querySelector('#navbarSideCollapse').addEventListener('click', function () {
  document.querySelector('.settings-collapse').classList.toggle('open')
});

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
    }
  }
});

// Rate limit websocket
setInterval(sendBytes, 15);
