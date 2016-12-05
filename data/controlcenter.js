var config;
var connection;
if(location.hostname != "")
	connection = new WebSocket('ws://' + location.hostname + ':81/', ['arduino']);
else
	connection = new WebSocket('ws://localhost');
connection.binaryType = "arraybuffer";

connection.onopen = function() {};
connection.onerror = function(error) { console.log('WebSocket Error ', error); };
connection.onmessage = function(e)
{
	console.log('Received: ', e.data);

	config = JSON.parse(e.data);
	time_zone.value = config.time_zone;
	sunset_offset.value = config.sunset_offset;
	summer_time.value = config.summer_time;
	wakeup_duration.value = config.wakeup_duration;
	longitude.value = config.longitude;
	latitude.value = config.latitude;
	red.value = config.wakeup_red;
	green.value = config.wakeup_green;
	blue.value = config.wakeup_blue;
};

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
		console.log("Sent: " + text);
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

function send_config()
{
	config.time_zone = time_zone.value;
	config.sunset_offset = sunset_offset.value;
	config.summer_time = summer_time.value;
	config.wakeup_duration = wakeup_duration.value;
	config.longitude = longitude.value;
	config.latitude = latitude.value;
	var json = JSON.stringify(config);

	send_text(json);
}

function send_wakeup_color()
{
	var wakeup_config = new Object();
	wakeup_config.wakeup_red = red.value;
	wakeup_config.wakeup_green = green.value;
	wakeup_config.wakeup_blue = blue.value;
	var json = JSON.stringify(wakeup_config);

	send_text(json);
}

$("input[name='wakeup_duration']").TouchSpin({ min: 5, max: 999999, postfix: 'minutes' });
$('#alarm_timepicker').timepicker({ showMeridian: false, minuteStep: 5 });

$("input[name='time_zone']").TouchSpin({ prefix: 'GMT+', max: 23, min: -23 });
$("input[name='summer_time']").TouchSpin({ max: 1 });
$("input[name='sunset_offset']").TouchSpin({ min: -1439, max: 1439, postfix: 'minutes' });

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
	}
});