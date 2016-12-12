var connection;
if(location.hostname != '')
	connection = new WebSocket('ws://' + location.hostname + ':81/', ['arduino']);
else
	connection = new WebSocket('ws://localhost');
connection.binaryType = 'arraybuffer';

connection.onopen = function() {};
connection.onerror = function(error) { console.log('WebSocket Error ', error); };
connection.onmessage = function(e)
{
	console.log('Received: ', e.data);

	try
	{
		// Parse JSON
		var config = JSON.parse(e.data);

		// time specific
		if(config.hasOwnProperty('time_zone'))
			time_zone.value = config.time_zone;
		if(config.hasOwnProperty('summer_time'))
			$('#summer_time').prop('checked', config.summer_time);
		if(config.hasOwnProperty('longitude'))
			longitude.value = config.longitude;
		if(config.hasOwnProperty('latitude'))
			latitude.value = config.latitude;
		// alarm
		if(config.hasOwnProperty('alarm_enabled'))
			$('#alarm_enabled').prop('checked', config.alarm_enabled);
		if(config.hasOwnProperty('alarm_duration'))
			alarm_duration.value = config.alarm_duration;
		if(config.hasOwnProperty('alarm_hour') && config.hasOwnProperty('alarm_minute'))
			$('#alarm_timepicker').timepicker('setTime', config.alarm_hour + ':' + config.alarm_minute);
		// sunset
		if(config.hasOwnProperty('sunset_enabled'))
			$('#sunset_enabled').prop('checked', config.sunset_enabled);
		if(config.hasOwnProperty('sunset_duration'))
			sunset_duration.value = config.sunset_duration;
		if(config.hasOwnProperty('sunset_offset'))
			sunset_offset.value = config.sunset_offset;
		// other
		if(config.hasOwnProperty('speed'))
			document.getElementById('speed').noUiSlider.set(config.speed);

		// Populate effect list selectors and button collection
		if(config.hasOwnProperty('effectList'))
		{
			for(var i in config.effectList)
			{
				$('#alarm_effect').append($('<option>', {
					text: config.effectList[i]
				}));
				$('#post_alarm_effect').append($('<option>', {
					text: config.effectList[i]
				}));
				$('#sunset_effect').append($('<option>', {
					text: config.effectList[i]
				}));

				// Add buttons to button collection except for custom color button
				if(config.effectList[i] != 'Custom Color')
					$('<button type="button" class="btn btn-default" onClick="send_effect_button(\'' + config.effectList[i] + '\');">' + config.effectList[i] + '</button>').insertAfter($('#custom_color_button'));
			}
			if(config.hasOwnProperty('alarm_effect'))
				$('#alarm_effect').val(config.alarm_effect);
			if(config.hasOwnProperty('post_alarm_effect'))
				$('#post_alarm_effect').val(config.post_alarm_effect);
			if(config.hasOwnProperty('sunset_effect'))
				$('#sunset_effect').val(config.sunset_effect);
		}
	}
	catch(f)
	{
		// Parse command
		var command = "";
		var argument = "";
		if(e.data.indexOf(' '))
		{
			command = e.data.substr(0, e.data.indexOf(' '));
			argument = e.data.substr(e.data.indexOf(' ') + 1);
		}
		else
		{
			command = e.data;
		}

		// Cycle through all effect buttons and change class/style accordingly
		$('#effectButtons button').each(function()
		{
			if($(this).text() == argument)
			{
				if((command == "pause" || command == "stop") && argument == "Custom Color")
				{
					$(this).css("background-color","white");
					$(this).css("foreground","black");
				}
				else if(command == "pause")
				{
					$(this).attr('class', 'btn btn-warning');
				}
				else if(command == "start")
				{
					$(this).attr('class', 'btn btn-success');
				}
			}
			else
			{
				if($(this).text() != "Nox")
				{
					$(this).attr('class', 'btn btn-default');
				}
				if($(this).text() == "Custom Color")
				{
					$(this).css("background-color","white");
					$(this).css("color","black");
				}
			}
		});
	}	
};

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

	var json = JSON.stringify(config, null, 2);
	send_text(json);
}

function send_bytes()
{
	if(connection.readyState == 1)
	{
		var data = new Uint8Array(arguments.length);
		for(var i = 0; i < arguments.length; i++)
		{
			data[i] = arguments[i];
		}

		connection.send(data.buffer);
		console.log("Sent bytes: " + data);
	}
	else
	{
		console.log("Not connected. Sending data failed!");
	}
}

function send_text(text)
{
	if(connection.readyState == 1)
	{
		connection.send(text);
		console.log("Sent text: " + text);
	}
	else
	{
		console.log("Not connected. Sending data failed!");
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




var stepSlider = document.getElementById('speed');
noUiSlider.create(stepSlider, { start: 20, step: 1, range: {'min':0, 'max':100} });
$('#alarm_timepicker').timepicker({ showMeridian: false, minuteStep: 5 });
$('input[name="alarm_duration"]').TouchSpin({ min: 1, max: 1439, postfix: 'minutes' });
$('input[name="time_zone"]').TouchSpin({ prefix: 'GMT+', max: 23, min: -23 });
$('input[name="summer_time"]').TouchSpin({ max: 1 });
$('input[name="sunset_duration"]').TouchSpin({ min: 1, max: 1439, postfix: 'minutes' });
$('input[name="sunset_offset"]').TouchSpin({ min: -1439, max: 1439, postfix: 'minutes' });

$('.color').colorPicker({
	opacity:false,
	preventFocus: true,
	animationSpeed: 0,
	buildCallback: function($elm) {
		$('.color').on('click', function(e) {
			e.preventDefault && e.preventDefault();
		});
	},
	renderCallback: function($elm, toggled) {
		var colors = this.color.colors;
		send_bytes(0, colors.RND.rgb.r, colors.RND.rgb.g, colors.RND.rgb.b);
	}
});

stepSlider.noUiSlider.on('update', function( values, handle ) {
	send_bytes(1, values[handle]);
});