var currentEffect = '';
var currentStatus = 0;
var currentCustomColor = '';
var currentCustomColor2 = '';
var maxSpeed = 14;

var websocketReady = false;
var ws_uri = 'ws://' + (location.hostname ? location.hostname : 'localhost') + ':81/';
var connection = new WebSocket(ws_uri, ['arduino']);
connection.binaryType = 'arraybuffer';

connection.onopen = function(e)
{
	send_text('requesting_config');

	$('#connectingOverlayText').html('Warte auf Antwort...');
	$('#connectButton').hide();
};

connection.onerror = function(e)
{
	console.log('WebSocket Error', e);
};

connection.onclose = function(e)
{
	$('#connectingOverlayText').html('Verbindung unterbrochen.');
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

		$('#connectingOverlayText').html('Erfolg!');
		$('#connectingOverlay').fadeOut(140);
	}
	catch(f)
	{
	}
};

function handle_json_data(data)
{
	// Populate effect list selectors and button collection
	if(data.hasOwnProperty('effect_list'))
	{
		for(var i in data.effect_list)
		{
			$('#alarm_effect').append($('<option>', {
				text: data.effect_list[i]
			}));
			$('#post_alarm_effect').append($('<option>', {
				text: data.effect_list[i]
			}));
			$('#sunset_effect').append($('<option>', {
				text: data.effect_list[i]
			}));
			$('#startup_effect').append($('<option>', {
				text: data.effect_list[i]
			}));

			// Add buttons to button collection except for custom color button
			if(data.effect_list[i] != 'Color' && data.effect_list[i] != 'Color 2' && data.effect_list[i] != 'Sunrise')
			{
				if(data.effect_list[i] == 'Nox')
					$('<button type="button" class="btn btn-danger" onClick="send_effect_button(\'' + data.effect_list[i] + '\');">' + data.effect_list[i] + '</button>').insertAfter($('#custom_color2_button'));
				else
					$('<button type="button" class="btn btn-default" onClick="send_effect_button(\'' + data.effect_list[i] + '\');">' + data.effect_list[i] + '</button>').insertAfter($('#custom_color2_button'));
			}
		}
	}
	// Update buttons list
	if(data.hasOwnProperty('status') && data.hasOwnProperty('current_effect'))
	{
		update_buttons(data.status, data.current_effect);
	}
	// Select effects
	if(data.hasOwnProperty('alarm_effect'))
		$('#alarm_effect').val(data.alarm_effect != '' ? data.alarm_effect : 'Color');
	if(data.hasOwnProperty('post_alarm_effect'))
		$('#post_alarm_effect').val(data.post_alarm_effect != '' ? data.post_alarm_effect : 'Color');
	if(data.hasOwnProperty('sunset_effect'))
		$('#sunset_effect').val(data.sunset_effect != '' ? data.sunset_effect : 'Color');
	if(data.hasOwnProperty('startup_effect'))
		$('#startup_effect').val(data.startup_effect != '' ? data.startup_effect : 'Color');
	// Settings
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
		document.getElementById('speed').noUiSlider.set(maxSpeed - data.speed);
	if(data.hasOwnProperty('saturation'))
		document.getElementById('saturation').noUiSlider.set(data.saturation);
	if(data.hasOwnProperty('custom_color'))
	{
		$('#custom_color_button').val(data.custom_color);
		currentCustomColor = data.custom_color;
	}
	if(data.hasOwnProperty('custom_color2'))
	{
		$('#custom_color2_button').val(data.custom_color2);
		currentCustomColor2 = data.custom_color2;
	}
	if(data.hasOwnProperty('own_ip'))
		own_ip.value = data.own_ip;
}

function update_buttons(status, effect)
{	
	currentStatus = status;
	currentEffect = effect;

	// Cycle through all effect buttons and change class/style accordingly
	$('#effectButtons button').each(function()
	{
		if($(this).text() == effect)
		{
			if(effect == 'Nox')
			{
			
			}
			else if(effect == 'Color')
			{
				if(status == 2)
				{
					var colorInstance = $customColorPicker.colorPicker.color;
					var colors = colorInstance.colors;
					colors.HEX = currentCustomColor;
					colorInstance.setColor(null, 'HEX');
					$(this).val('#' + currentCustomColor).css({'background-color': '#' + colors.HEX, 'color': colors.rgbaMixBGMixCustom.luminance > 0.22 ? '#222' : '#ddd'});
				}
			}
			else if(effect == 'Color 2')
			{
				if(status == 2)
				{
					var colorInstance = $customColor2Picker.colorPicker.color; // TODO
					var colors = colorInstance.colors;
					colors.HEX = currentCustomColor2;
					colorInstance.setColor(null, 'HEX');
					$(this).val('#' + currentCustomColor2).css({'background-color': '#' + colors.HEX, 'color': colors.rgbaMixBGMixCustom.luminance > 0.22 ? '#222' : '#ddd'});
				}
			}
			else if(status == 1)
			{
				$(this).attr('class', 'btn btn-warning');
			}
			else if(status == 2)
			{
				$(this).attr('class', 'btn btn-success');
			}
			else if(status == -1)
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
		else
		{
			// Reset button style except for Nox and custom color buttons
			if($(this).text() != 'Nox')
			{
				$(this).attr('class', 'btn btn-default');
			}
			if($(this).text() == 'Color')
			{
				$(this).css('background-color', '#464545');
				$(this).css('color', 'white');
			}
			if($(this).text() == 'Color 2')
			{
				$(this).css('background-color', '#464545');
				$(this).css('color', 'white');
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
	var time = $('#alarm_time').val().split(':');
	config.alarm_hour = time[0];
	config.alarm_minute = time[1];
	config.alarm_duration = alarm_duration.value;
	config.alarm_effect = $('#alarm_effect').val();
	config.post_alarm_effect = $('#post_alarm_effect').val();
	// sunset
	config.sunset_enabled = $('#sunset_enabled').is(':checked');
	config.sunset_duration = sunset_duration.value;
	config.sunset_offset = sunset_offset.value;
	config.sunset_effect = $('#sunset_effect').val();
	// other
	config.startup_effect = $('#startup_effect').val();
	config.speed = maxSpeed - document.getElementById('speed').noUiSlider.get();
	config.saturation = document.getElementById('saturation').noUiSlider.get();
	config.custom_color = currentCustomColor;
	config.custom_color2 = currentCustomColor2;

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
	}
	else
	{
		console.log('Not connected. Sending data failed!');
	}
}

function send_effect_button(effect)
{
	send_text('toggle ' + effect);
	update_buttons(-1, effect);
}



$('.clockpicker').clockpicker().find('input').change(function(){});
$('input[name="alarm_duration"]').TouchSpin({ min: 1, max: 1439, postfix: 'minutes' });
$('input[name="time_zone"]').TouchSpin({ prefix: 'GMT+', max: 23, min: -23 });
$('input[name="summer_time"]').TouchSpin({ max: 1 });
$('input[name="sunset_duration"]').TouchSpin({ min: 1, max: 1439, postfix: 'minutes' });
$('input[name="sunset_offset"]').TouchSpin({ min: -1439, max: 1439, postfix: 'minutes' });

var $customColorPicker = $('#custom_color_button').colorPicker({
	opacity:false,
	preventFocus: true,
	buildCallback: function($elm) { $('#custom_color_button').on('click', function(e) { e.preventDefault && e.preventDefault(); }); },
	renderCallback: function($elm, toggled) {
		if(toggled === true)
		{
			update_buttons(2, 'Color');
		}
		else if(toggled === false)
		{
			if(currentEffect != 'Color' || currentStatus == 0)
			{
				$('#custom_color_button').css('background-color','#464545');
				$('#custom_color_button').css('color','white');
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
var $customColor2Picker = $('#custom_color2_button').colorPicker({
	opacity:false,
	preventFocus: true,
	buildCallback: function($elm) { $('#custom_color2_button').on('click', function(e) { e.preventDefault && e.preventDefault(); }); },
	renderCallback: function($elm, toggled) {
		if(toggled === true)
		{
			update_buttons(2, 'Color 2');
		}
		else if(toggled === false)
		{
			if(currentEffect != 'Color 2' || currentStatus == 0)
			{
				$('#custom_color2_button').css('background-color','#464545');
				$('#custom_color2_button').css('color','white');
			}
		}
		var newColor = this.color.colors.HEX + ''; // dereference by appending ''
		if(currentCustomColor2 != newColor || toggled === true)
		{
			var newColorRGB = this.color.colors.RND.rgb;
			send_bytes(8, newColorRGB.r, newColorRGB.g, newColorRGB.b);
			currentCustomColor2 = newColor;
		}
	}
});

var speedStepSlider = document.getElementById('speed');
noUiSlider.create(speedStepSlider, { start: 10, step: 1, range: {'min':0, 'max':maxSpeed} });
speedStepSlider.noUiSlider.on('update', function( values, handle ) {
	send_bytes(1, maxSpeed - values[handle]);
});
$('#speed .noUi-handle').html("Geschw.");
var saturationStepSlider = document.getElementById('saturation');
noUiSlider.create(saturationStepSlider, { start: 255, step: 1, range: {'min':110, 'max':255} });
saturationStepSlider.noUiSlider.on('update', function( values, handle ) {
	send_bytes(5, values[handle]);
});
$('#saturation .noUi-handle').html("S&auml;tt.");

var idleTime = 0;
setInterval(function ()
{
	idleTime++;
	if(idleTime > 300)//s
		connection.close();
}, 1000);
$(this).mousemove(function (e) { idleTime = 0; });
$(this).keypress(function (e) { idleTime = 0; });

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