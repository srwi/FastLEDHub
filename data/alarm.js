var alarm_hour = 0;
var alarm_minute = 0;

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
		showConfirmation();
		return;
	}

	try
	{
		var data = JSON.parse(e.data);

		$('#connectingOverlayText').html('Success!');
		$('#connectingOverlay').fadeOut(140);

		if(data.hasOwnProperty('alarm_enabled'))
			$('#disable_alarm_button').css('display', data.alarm_enabled ? 'block' : 'none');
		if(data.hasOwnProperty('alarm_hour') && data.hasOwnProperty('alarm_minute'))
			$('#alarm_time').val((data.alarm_hour < 10 ? '0' + data.alarm_hour.toString() : data.alarm_hour) + ':' + (data.alarm_minute < 10 ? '0' + data.alarm_minute.toString() : data.alarm_minute));
	}
	catch(f)
	{
	}
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

function send_alarm()
{
	time = $('#alarm_time').val().split(':');
	send_bytes(2, time[0], time[1]);
}

function showConfirmation()
{
	$('#confirmationOverlayText').html('See you at ' + $('#alarm_time').val() + '! :)');
	$('#confirmationOverlay').fadeIn(140);
	connection.close();
}


$('.clockpicker').clockpicker().find('input').change(function(){});