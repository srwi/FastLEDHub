var currentEffect = '';
var currentState = '';
var currentCustomColor = '';

var websocketReady = true;
var ws_uri = 'ws://' + (location.hostname ? location.hostname : 'localhost') + ':81/';
var connection = new WebSocket(ws_uri, ['arduino']);
connection.binaryType = 'arraybuffer';

connection.onopen = function(e)
{
	$('#connectingOverlayText').html('Waiting for device information...');
	$('#connectButton').hide();
};

connection.onerror = function(e)
{
	console.log('WebSocket Error', e);
};

connection.onclose = function(e)
{
	$('#connectingOverlayText').html('Connection closed.');
	$('#refreshButton').show();
	$('#connectingOverlay').fadeIn(140);
};

connection.onmessage = function(e)
{
	console.log('Received: ', e.data);

	if(e.data == 'pong')
	{
		websocketReady = true;
		return;
	}

	try
	{
		handle_json_data(JSON.parse(e.data));

		$('#connectingOverlayText').html('Success!');
		$('#connectingOverlay').fadeOut(140);
	}
	catch(f)
	{
		update_buttons_by_command(e.data);
	}
};

function handle_json_data(data)
{
	// time specific
	if(data.hasOwnProperty('time_zone'))
		time_zone.value = data.time_zone;
	if(data.hasOwnProperty('summer_time'))
		$('#summer_time').prop('checked', data.summer_time);
	if(data.hasOwnProperty('longitude'))
		longitude.value = data.longitude;
	if(data.hasOwnProperty('latitude'))
		latitude.value = data.latitude;
	// alarm
	if(data.hasOwnProperty('alarm_enabled'))
		$('#alarm_enabled').prop('checked', data.alarm_enabled);
	if(data.hasOwnProperty('alarm_duration'))
		alarm_duration.value = data.alarm_duration;
	if(data.hasOwnProperty('alarm_hour') && data.hasOwnProperty('alarm_minute'))
		$('#alarm_timepicker').timepicker('setTime', data.alarm_hour + ':' + data.alarm_minute);
	// sunset
	if(data.hasOwnProperty('sunset_enabled'))
		$('#sunset_enabled').prop('checked', data.sunset_enabled);
	if(data.hasOwnProperty('sunset_duration'))
		sunset_duration.value = data.sunset_duration;
	if(data.hasOwnProperty('sunset_offset'))
		sunset_offset.value = data.sunset_offset;
	// other
	if(data.hasOwnProperty('speed'))
		document.getElementById('speed').noUiSlider.set(data.speed);
	if(data.hasOwnProperty('custom_color'))
	{
		$('.color').val(data.custom_color);
		currentCustomColor = data.custom_color;
	}

	// Populate effect list selectors and button collection
	if(data.hasOwnProperty('effectList'))
	{
		for(var i in data.effectList)
		{
			$('#alarm_effect').append($('<option>', {
				text: data.effectList[i]
			}));
			$('#post_alarm_effect').append($('<option>', {
				text: data.effectList[i]
			}));
			$('#sunset_effect').append($('<option>', {
				text: data.effectList[i]
			}));

			// Add buttons to button collection except for custom color button
			if(data.effectList[i] != 'Custom Color')
				$('<button type="button" class="btn btn-default" onClick="send_effect_button(\'' + data.effectList[i] + '\');">' + data.effectList[i] + '</button>').insertAfter($('#custom_color_button'));
		}
		if(data.hasOwnProperty('alarm_effect'))
			$('#alarm_effect').val(data.alarm_effect);
		if(data.hasOwnProperty('post_alarm_effect'))
			$('#post_alarm_effect').val(data.post_alarm_effect);
		if(data.hasOwnProperty('sunset_effect'))
			$('#sunset_effect').val(data.sunset_effect);
	}
}

function update_buttons_by_command(cmd)
{
	// Parse command
	var command = '';
	var argument = '';
	if(cmd.indexOf(' ') != -1)
	{
		// Has argument
		command = cmd.substr(0, cmd.indexOf(' '));
		argument = cmd.substr(cmd.indexOf(' ') + 1);
	}
	else
	{
		// Doesn't have argument
		command = cmd;
	}
	
	currentState = command;
	currentEffect = argument;

	// Cycle through all effect buttons and change class/style accordingly
	$('#effectButtons button').each(function()
	{
		if($(this).text() == argument)
		{
			if(argument == 'Custom Color')
			{
				if(command == 'start')
				{
					var colorInstance = $customColorPicker.colorPicker.color;
					var colors = colorInstance.colors;
					colors.HEX = currentCustomColor;
					colorInstance.setColor(null, 'HEX');
					$(this).val('#' + currentCustomColor).css({'background-color': '#' + colors.HEX, 'color': colors.rgbaMixBGMixCustom.luminance > 0.22 ? '#222' : '#ddd'});
				}
			}
			else if(command == 'pause')
			{
				$(this).attr('class', 'btn btn-warning');
			}
			else if(command == 'start')
			{
				$(this).attr('class', 'btn btn-success');
			}
			else if(command == 'toggle')
			{
				if($(this).hasClass('btn-success'))
				{
					currentState = 'pause';
					$(this).addClass('btn-warning').removeClass('btn-success');
				}
				else
				{
					currentState = 'start';
					$(this).addClass('btn-success').removeClass('btn-warning');
				}
			}
		}
		else
		{
			// Reset button style except for Nox and custom color buttons
			if($(this).text() != 'Nox')
			{
				$(this).attr('class', 'btn btn-default');
			}
			if($(this).text() == 'Custom Color')
			{
				$(this).css('background-color', 'white');
				$(this).css('color', 'black');
			}
		}
	});
}

function send_config()
{
	var config = new Object();
	// time specific
	config.time_zone = time_zone.value;
	config.summer_time = $('#summer_time').is(':checked');
	config.longitude = longitude.value;
	config.latitude = latitude.value;
	// alarm
	config.alarm_enabled = $('#alarm_enabled').is(':checked');
	config.alarm_hour = $('#alarm_timepicker').data('timepicker').hour;
	config.alarm_minute = $('#alarm_timepicker').data('timepicker').minute;
	config.alarm_duration = alarm_duration.value;
	config.alarm_effect = $('#alarm_effect').val();
	config.post_alarm_effect = $('#post_alarm_effect').val();
	// sunset
	config.sunset_enabled = $('#sunset_enabled').is(':checked');
	config.sunset_duration = sunset_duration.value;
	config.sunset_offset = sunset_offset.value;
	config.sunset_effect = $('#sunset_effect').val();
	// other
	config.speed = document.getElementById('speed').noUiSlider.get();
	config.custom_color = currentCustomColor;

	var json = JSON.stringify(config, null, 2);
	send_text(json);
}

function send_bytes()
{
	if(connection.readyState == 1 && websocketReady)
	{
		var data = new Uint8Array(arguments.length);
		for(var i = 0; i < arguments.length; i++)
		{
			data[i] = arguments[i];
		}

		connection.send(data.buffer);
		websocketReady = false;
		console.log('Sent bytes: ' + data);
	}
	else
	{
		console.log('Not connected. Sending data failed!');
	}
}

function send_text(text)
{
	if(connection.readyState == 1)
	{
		connection.send(text);
		console.log('Sent text: ' + text);
		update_buttons_by_command(text);
	}
	else
	{
		console.log('Not connected. Sending data failed!');
	}
}

function send_color(r, g, b)
{
	send_bytes(0, r, g, b);
}

function send_effect_button(effect)
{
	send_text('toggle ' + effect);
}



$('#alarm_timepicker').timepicker({ showMeridian: false, minuteStep: 5 });
$('input[name="alarm_duration"]').TouchSpin({ min: 1, max: 1439, postfix: 'minutes' });
$('input[name="time_zone"]').TouchSpin({ prefix: 'GMT+', max: 23, min: -23 });
$('input[name="summer_time"]').TouchSpin({ max: 1 });
$('input[name="sunset_duration"]').TouchSpin({ min: 1, max: 1439, postfix: 'minutes' });
$('input[name="sunset_offset"]').TouchSpin({ min: -1439, max: 1439, postfix: 'minutes' });

var $customColorPicker = $('.color').colorPicker({
	opacity:false,
	preventFocus: true,
	buildCallback: function($elm) { $('.color').on('click', function(e) { e.preventDefault && e.preventDefault(); }); },
	renderCallback: function($elm, toggled) {
		if(toggled === true)
		{
			update_buttons_by_command('start Custom Color');
		}
		else if(toggled === false)
		{
			if(currentEffect != 'Custom Color' || currentState == 'stop')
			{
				$('#custom_color_button').css('background-color','white');
				$('#custom_color_button').css('color','black');
			}
		}
		var newColor = this.color.colors.HEX + ''; // dereference by appending ''
		if(currentCustomColor != newColor || toggled === true)
		{
			var newColorRGB = this.color.colors.RND.rgb;
			send_bytes(0, newColorRGB.r, newColorRGB.g, newColorRGB.b);
			currentCustomColor = newColor;
		}
	}
});

var stepSlider = document.getElementById('speed');
noUiSlider.create(stepSlider, { start: 20, step: 1, range: {'min':0, 'max':100} });
stepSlider.noUiSlider.on('update', function( values, handle ) {
	send_bytes(1, values[handle]);
});

var idleTime = 0;
var idleInterval = setInterval(function () {
	idleTime++;
	if(idleTime > 300)//s
		connection.close();
}, 1000);
$(this).mousemove(function (e) { idleTime = 0; });
$(this).keypress(function (e) { idleTime = 0; });