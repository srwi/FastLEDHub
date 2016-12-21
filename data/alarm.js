var alarm_hour = 0;
var alarm_minute = 0;

var ws_uri = 'ws://' + (location.hostname ? location.hostname : 'localhost') + ':81/';
var connection = new WebSocket(ws_uri, ['arduino']);
connection.binaryType = 'arraybuffer';

connection.onopen = function(e)
{
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

	if(e.data == 'alarm set')
	{
		showConfirmation();
		return;
	}
	else if(e.data == 'alarm disabled')
	{
		$('#disable_alarm_button').addClass('disabled');
		return;
	}

	try
	{
		var data = JSON.parse(e.data);

		$('#connectingOverlayText').html('Erfolg!');
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

function disable_alarm()
{
	send_bytes(3);
}

function send_alarm()
{
	time = $('#alarm_time').val().split(':');
	send_bytes(2, time[0], time[1]);
}

function showConfirmation()
{
	$('#confirmationOverlayText').html($('#alarm_time').val() + ' Uhr');
	$('#confirmationOverlay').fadeIn(140);
	connection.close();
}


$('.clockpicker').clockpicker().find('input').change(function(){});

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