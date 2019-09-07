let currentAnimation = '';
let currentStatus = 0;
let currentColor = '';

let websocketReady = false;
let ws_uri = 'ws://' + (location.hostname ? location.hostname : 'localhost') + ':81/';
let connection = new WebSocket(ws_uri, ['arduino']);
connection.binaryType = 'arraybuffer';

connection.onopen = function(e)
{
  sendText('requesting_config');

  $('#connectingOverlayText').html('Waiting for response...');
  $('#connectButton').hide();
};

connection.onerror = function(e)
{
  console.log('WebSocket Error', e);
};

connection.onclose = function(e)
{
  $('#connectingOverlayText').html('Connection interrupted.');
  $('#refreshButton').show();
  $('#connectingOverlay').fadeIn(140);
};

connection.onmessage = function(e)
{
  console.log('Received: ', e.data);

  websocketReady = true;
  try
  {
    handleJsonData(JSON.parse(e.data));

    $('#connectingOverlayText').html('Success!');
    $('#connectingOverlay').fadeOut(140);
  } catch(f) {}
};

function handleJsonData(data)
{
  if(data.hasOwnProperty('animations'))
  {
    data.animations.forEach(animation => {
      // Fill animation dropdowns
      $('#alarmAnimation').append($('<option>', { text: animation }));
      $('#postAlarmAnimation').append($('<option>', { text: animation }));
      $('#sunsetAnimation').append($('<option>', { text: animation }));
      $('#startupAnimation').append($('<option>', { text: animation }));

      // Add buttons to button collection
      if(animation != 'Color' && animation != 'Sunrise')
        $('<button type="button" class="btn btn-secondary" onClick="sendAnimationButton(\'' + animation + '\');">' + animation + '</button>').insertAfter($('#colorButton'));
    })
  }

  // Update buttons list
  if(data.hasOwnProperty('status') && data.hasOwnProperty('currentAnimation'))
    updateButtons(data.status, data.currentAnimation);

  if(data.hasOwnProperty('alarmAnimation'))
    $('#alarmAnimation').val(data.alarmAnimation != '' ? data.alarmAnimation : 'Color');
  if(data.hasOwnProperty('postAlarmAnimation'))
    $('#postAlarmAnimation').val(data.postAlarmAnimation != '' ? data.postAlarmAnimation : 'Color');
  if(data.hasOwnProperty('sunsetAnimation'))
    $('#sunsetAnimation').val(data.sunsetAnimation != '' ? data.sunsetAnimation : 'Color');
  if(data.hasOwnProperty('startupAnimation'))
  {
    $('#startupAnimation').val(data.startupAnimation != '' ? data.startupAnimation : 'Color');
    $('#useStartupAnimation').prop('checked', data.startupAnimation != '')
    $('#startupAnimation').prop('disabled', data.startupAnimation == '')
  }
  if(data.hasOwnProperty('timeZone'))
    timeZone.value = data.timeZone;
  if(data.hasOwnProperty('summerTime'))
    $('#summerTime').prop('checked', data.summerTime);
  if(data.hasOwnProperty('longitude'))
    longitude.value = data.longitude;
  if(data.hasOwnProperty('latitude'))
    latitude.value = data.latitude;
  if(data.hasOwnProperty('alarmEnabled'))
    $('#alarmEnabled').prop('checked', data.alarmEnabled);
  if(data.hasOwnProperty('alarmDuration'))
    alarmDuration.value = data.alarmDuration;
  if(data.hasOwnProperty('alarmHour') && data.hasOwnProperty('alarmMinute'))
    $('#alarmTime').val((data.alarmHour < 10 ? '0' + data.alarmHour.toString() : data.alarmHour) + ':' + (data.alarmMinute < 10 ? '0' + data.alarmMinute.toString() : data.alarmMinute));
  if(data.hasOwnProperty('sunsetEnabled'))
    $('#sunsetEnabled').prop('checked', data.sunsetEnabled);
  if(data.hasOwnProperty('sunsetDuration'))
    sunsetDuration.value = data.sunsetDuration;
  if(data.hasOwnProperty('sunsetOffset'))
    sunsetOffset.value = data.sunsetOffset;
  if(data.hasOwnProperty('speed'))
    document.getElementById('speed').noUiSlider.set(data.speed);
  if(data.hasOwnProperty('saturation'))
    document.getElementById('saturation').noUiSlider.set(data.saturation);
  if(data.hasOwnProperty('color'))
  {
    $('#colorButton').val(data.color);
    currentColor = data.color;
  }
}

function updateButtons(status, animation)
{
  currentStatus = status;
  currentAnimation = animation;

  // Cycle through all animation buttons and change class/style accordingly
  $('#animationButtons button').each(function()
  {
    if($(this).text() == animation)
    {
      if(animation == 'Color')
      {
        if(status == 2) // Running
        {
          let colorInstance = $customColorPicker.colorPicker.color;
          let colors = colorInstance.colors;
          colors.HEX = currentColor;
          colorInstance.setColor(null, 'HEX');
          $(this).val('#' + currentColor).css({'background-color': '#' + colors.HEX, 'color': colors.rgbaMixBGMixCustom.luminance > 0.22 ? '#222' : '#ddd'});
        }
      }
      else if(status == 1) // Paused
      {
        $(this).attr('class', 'btn btn-warning');
      }
      else if(status == 2) // Running
      {
        $(this).attr('class', 'btn btn-success');
      }
      else if(status == -1) // Toggle
      {
        if($(this).hasClass('btn-success'))
        {
          currentStatus = 1;
          $(this).addClass('btn-warning').removeClass('btn-success');
        }
        else
        {
          currentStatus = 2;
          $(this).addClass('btn-success').removeClass('btn-warning');
        }
      }
    }
    else if($(this).text() == 'Color')
    {
      $(this).css('background-color', '#464545');
      $(this).css('color', 'white');
    }
    else
    {
      $(this).removeClass('btn-success').removeClass('btn-warning').addClass('btn-secondary');
    }
  });
}

function sendConfig()
{
  let config = new Object();
  let time = $('#alarmTime').val().split(':');

  config.alarmHour = time[0];
  config.alarmMinute = time[1];
  config.alarmEnabled = $('#alarmEnabled').is(':checked');
  config.alarmDuration = alarmDuration.value;
  config.alarmAnimation = $('#alarmAnimation').val();
  config.postAlarmAnimation = $('#postAlarmAnimation').val();
  config.timeZone = timeZone.value;
  config.summerTime = $('#summerTime').is(':checked');
  config.longitude = longitude.value;
  config.latitude = latitude.value;
  config.sunsetEnabled = $('#sunsetEnabled').is(':checked');
  config.sunsetDuration = sunsetDuration.value;
  config.sunsetOffset = sunsetOffset.value;
  config.sunsetAnimation = $('#sunsetAnimation').val();
  config.startupAnimation = $('#useStartupAnimation').is(':checked') ? $('#startupAnimation').val() : '';
  config.speed = document.getElementById('speed').noUiSlider.get();
  config.saturation = document.getElementById('saturation').noUiSlider.get();
  config.color = currentColor;

  let json = JSON.stringify(config, null, 2);
  sendText(json);
}

function sendBytes()
{
  if(connection.readyState == 1 && websocketReady)
  {
    let data = new Uint8Array(arguments.length);
    for(let i = 0; i < arguments.length; i++)
    {
      data[i] = arguments[i];
    }
    connection.send(data.buffer);
    websocketReady = false;
    console.log('Sent bytes: ' + data);
  }
  else
  {
    console.log('Not connected. Data not sent!');
  }
}

function sendText(text)
{
  if(connection.readyState == 1)
  {
    connection.send(text);
    console.log('Sent text: ' + text);
  }
  else
  {
    console.log('Not connected. Text not sent!');
  }
}

function sendAnimationButton(animation)
{
  sendText('toggle ' + animation);
  updateButtons(-1, animation);
}


// Inputs
$('.clockpicker').clockpicker().find('input').change(function(){});
$('input[name="alarmDuration"]').TouchSpin({ min: 1, max: 1439, postfix: 'minutes' });
$('input[name="timeZone"]').TouchSpin({ prefix: 'GMT+', max: 23, min: -23 });
$('input[name="summerTime"]').TouchSpin({ max: 1 });
$('input[name="sunsetDuration"]').TouchSpin({ min: 1, max: 1439, postfix: 'minutes' });
$('input[name="sunsetOffset"]').TouchSpin({ min: -1439, max: 1439, postfix: 'minutes' });

// Color picker
let $customColorPicker = $('#colorButton').colorPicker({
  opacity:false,
  preventFocus: true,
  buildCallback: function($elm) { $('#colorButton').on('click', function(e) { e.preventDefault && e.preventDefault(); }); },
  renderCallback: function($elm, toggled) {
    if(toggled === true)
    {
      updateButtons(2, 'Color');
    }
    else if(toggled === false)
    {
      if(currentAnimation != 'Color' || currentStatus == 0)
      {
        $('#colorButton').css('background-color','#464545');
        $('#colorButton').css('color','white');
      }
    }
    let newColor = this.color.colors.HEX + ''; // dereference by appending ''
    if(currentColor != newColor || toggled === true)
    {
      let newColorRGB = this.color.colors.RND.rgb;
      sendBytes(0, newColorRGB.r, newColorRGB.g, newColorRGB.b);
      currentColor = newColor;
    }
  }
});

// Sliders
let speedStepSlider = document.getElementById('speed');
noUiSlider.create(speedStepSlider, { start: 128, step: 1, range: { 'min': 0, 'max': 255 } });
speedStepSlider.noUiSlider.on('update', function(values, handle) {
  sendBytes(1, values[handle]);
});
$('#speed .noUi-handle').html("Speed");
let saturationStepSlider = document.getElementById('saturation');
noUiSlider.create(saturationStepSlider, { start: 255, step: 1, range: {'min':110, 'max':255} });
saturationStepSlider.noUiSlider.on('update', function(values, handle) {
  sendBytes(5, values[handle]);
});
$('#saturation .noUi-handle').html("Sat.");

// Close connection after being ianctive for 5 minutes
let idleTime = 0;
setInterval(function ()
{
  idleTime++;
  if(idleTime > 300)//s
    connection.close();
}, 1000);
$(this).mousemove(function (e) { idleTime = 0; });
$(this).keypress(function (e) { idleTime = 0; });

// Close connection on mobile when page loses focus
document.addEventListener('visibilitychange', function()
{
  if( /Android|webOS|iPhone|iPad|iPod|BlackBerry|IEMobile|Opera Mini/i.test(navigator.userAgent) )
  {
    if(document.hidden)
      connection.close();
    else
      location.reload();
  }
})