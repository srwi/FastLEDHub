let currentAnimation = '';
let currentStatus = 0;
let currentColor = '';

let websocketReady = false;
let ws_uri = 'ws://' + (location.hostname ? location.hostname : 'localhost') + ':81/';
let connection = new WebSocket(ws_uri, ['arduino']);
connection.binaryType = 'arraybuffer';

connection.onopen = function(e)
{
  send_text('requesting_config');

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
    handle_json_data(JSON.parse(e.data));

    $('#connectingOverlayText').html('Success!');
    $('#connectingOverlay').fadeOut(140);
  } catch(f) {}
};

function handle_json_data(data)
{
  if(data.hasOwnProperty('animations'))
  {
    data.animations.forEach(animation => {
      // Fill animation dropdowns
      $('#alarm_animation').append($('<option>', { text: animation }));
      $('#post_alarm_animation').append($('<option>', { text: animation }));
      $('#sunset_animation').append($('<option>', { text: animation }));
      $('#startup_animation').append($('<option>', { text: animation }));

      // Add buttons to button collection
      if(animation != 'Color' && animation != 'Sunrise')
        $('<button type="button" class="btn btn-default" onClick="send_animation_button(\'' + animation + '\');">' + animation + '</button>').insertAfter($('#color_button'));
    })
  }

  // Update buttons list
  if(data.hasOwnProperty('status') && data.hasOwnProperty('current_animation'))
    update_buttons(data.status, data.current_animation);

  if(data.hasOwnProperty('alarm_animation'))
    $('#alarm_animation').val(data.alarm_animation != '' ? data.alarm_animation : 'Color');
  if(data.hasOwnProperty('post_alarm_animation'))
    $('#post_alarm_animation').val(data.post_alarm_animation != '' ? data.post_alarm_animation : 'Color');
  if(data.hasOwnProperty('sunset_animation'))
    $('#sunset_animation').val(data.sunset_animation != '' ? data.sunset_animation : 'Color');
  if(data.hasOwnProperty('startup_animation'))
    $('#startup_animation').val(data.startup_animation != '' ? data.startup_animation : 'Color');
  if(data.hasOwnProperty('time_zone'))
    time_zone.value = data.time_zone;
  if(data.hasOwnProperty('summer_time'))
    $('#summer_time').prop('checked', data.summer_time);
  if(data.hasOwnProperty('longitude'))
    longitude.value = data.longitude;
  if(data.hasOwnProperty('latitude'))
    latitude.value = data.latitude;
  if(data.hasOwnProperty('alarm_enabled'))
    $('#alarm_enabled').prop('checked', data.alarm_enabled);
  if(data.hasOwnProperty('alarm_duration'))
    alarm_duration.value = data.alarm_duration;
  if(data.hasOwnProperty('alarm_hour') && data.hasOwnProperty('alarm_minute'))
    $('#alarm_time').val((data.alarm_hour < 10 ? '0' + data.alarm_hour.toString() : data.alarm_hour) + ':' + (data.alarm_minute < 10 ? '0' + data.alarm_minute.toString() : data.alarm_minute));
  if(data.hasOwnProperty('sunset_enabled'))
    $('#sunset_enabled').prop('checked', data.sunset_enabled);
  if(data.hasOwnProperty('sunset_duration'))
    sunset_duration.value = data.sunset_duration;
  if(data.hasOwnProperty('sunset_offset'))
    sunset_offset.value = data.sunset_offset;
  if(data.hasOwnProperty('speed'))
    document.getElementById('speed').noUiSlider.set(data.speed);
  if(data.hasOwnProperty('saturation'))
    document.getElementById('saturation').noUiSlider.set(data.saturation);
  if(data.hasOwnProperty('color'))
  {
    $('#color_button').val(data.color);
    currentColor = data.color;
  }
}

function update_buttons(status, animation)
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
      $(this).removeClass('btn-success').removeClass('btn-warning').addClass('btn-default');
    }
  });
}

function send_config()
{
  let config = new Object();
  let time = $('#alarm_time').val().split(':');

  config.alarm_hour = time[0];
  config.alarm_minute = time[1];
  config.alarm_enabled = $('#alarm_enabled').is(':checked');
  config.alarm_duration = alarm_duration.value;
  config.alarm_animation = $('#alarm_animation').val();
  config.post_alarm_animation = $('#post_alarm_animation').val();
  config.time_zone = time_zone.value;
  config.summer_time = $('#summer_time').is(':checked');
  config.longitude = longitude.value;
  config.latitude = latitude.value;
  config.sunset_enabled = $('#sunset_enabled').is(':checked');
  config.sunset_duration = sunset_duration.value;
  config.sunset_offset = sunset_offset.value;
  config.sunset_animation = $('#sunset_animation').val();
  config.startup_animation = $('#startup_animation').val();
  config.speed = document.getElementById('speed').noUiSlider.get();
  config.saturation = document.getElementById('saturation').noUiSlider.get();
  config.color = currentColor;

  let json = JSON.stringify(config, null, 2);
  send_text(json);
}

function send_bytes()
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

function send_text(text)
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

function send_animation_button(animation)
{
  send_text('toggle ' + animation);
  update_buttons(-1, animation);
}


// Inputs
$('.clockpicker').clockpicker().find('input').change(function(){});
$('input[name="alarm_duration"]').TouchSpin({ min: 1, max: 1439, postfix: 'minutes' });
$('input[name="time_zone"]').TouchSpin({ prefix: 'GMT+', max: 23, min: -23 });
$('input[name="summer_time"]').TouchSpin({ max: 1 });
$('input[name="sunset_duration"]').TouchSpin({ min: 1, max: 1439, postfix: 'minutes' });
$('input[name="sunset_offset"]').TouchSpin({ min: -1439, max: 1439, postfix: 'minutes' });

// Color picker
let $customColorPicker = $('#color_button').colorPicker({
  opacity:false,
  preventFocus: true,
  buildCallback: function($elm) { $('#color_button').on('click', function(e) { e.preventDefault && e.preventDefault(); }); },
  renderCallback: function($elm, toggled) {
    if(toggled === true)
    {
      update_buttons(2, 'Color');
    }
    else if(toggled === false)
    {
      if(currentAnimation != 'Color' || currentStatus == 0)
      {
        $('#color_button').css('background-color','#464545');
        $('#color_button').css('color','white');
      }
    }
    let newColor = this.color.colors.HEX + ''; // dereference by appending ''
    if(currentColor != newColor || toggled === true)
    {
      let newColorRGB = this.color.colors.RND.rgb;
      send_bytes(0, newColorRGB.r, newColorRGB.g, newColorRGB.b);
      currentColor = newColor;
    }
  }
});

// Sliders
let speedStepSlider = document.getElementById('speed');
noUiSlider.create(speedStepSlider, { start: 128, step: 1, range: { 'min': 0, 'max': 255 } });
speedStepSlider.noUiSlider.on('update', function(values, handle) {
  send_bytes(1, values[handle]);
});
$('#speed .noUi-handle').html("Speed");
let saturationStepSlider = document.getElementById('saturation');
noUiSlider.create(saturationStepSlider, { start: 255, step: 1, range: {'min':110, 'max':255} });
saturationStepSlider.noUiSlider.on('update', function(values, handle) {
  send_bytes(5, values[handle]);
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