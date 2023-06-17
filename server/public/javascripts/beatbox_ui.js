"use strict";
// Client-side interactions with the browser.

//Web sockets: automatically establish a socket with the server
var socket = io.connect();
var mode_bool = false;
var component_bool = false;
var mode = 0;
// Make connection to server when web page is fully loaded.
$(document).ready(function() {
	// Make the text-entry box have keyboard focus.
	var errorTimer= setTimeout(function(){
		console.log("timer: "+ errorTimer)
		socket.emit("error","No response from beat-box application. Is it running?")
	},5000)
	$('#error-box').hide();

	$('#modeNone').click(function(){
		highlight_mode(2);
		console.log("mode none clicked");
		sendModeCommand(2);
	});

	$('#modeRock1').click(function(){
		highlight_mode(0);
		console.log("mode Rock1 clicked");
		sendModeCommand(0);
	});
	$('#modeRock2').click(function(){
		highlight_mode(1);
		console.log("mode Rock2 clicked");
		sendModeCommand(1);
	});

	$('#baseDrum').click(function(){
		highlight_component(1);
		console.log("playing base drum");
		sendElementCommand(1);
	});

	$('#hi-hat').click(function(){
		highlight_component(2);
		console.log("playing hi-hat");
		sendElementCommand(2);
	});

	$('#snare').click(function(){
		highlight_component(3);
		console.log("playing snare");
		sendElementCommand(3);
	});

	$('#volumeDown').click(function(){
		console.log("volume down clicked");
		updateVolumeDisplayDown();
	});

	$('#volumeUp').click(function(){
		console.log("volume up clicked");
		updateVolumeDisplayUp();
	});

	$('#tempoDown').click(function(){
		console.log("tempo down clicked");
		updateTempoDisplayDown();
	});
	$('#tempoUp').click(function(){
		console.log("tempo up clicked");
		updateTempoDisplayUp();
	});
	
	$('#displayUpTime').ready(function(){
		var intervalId = setInterval(function() {
			updateUpTime();
		  }, 500);
		  
	});

	$('#volumeid').ready(function(){
		var intervalId = setInterval(function() {
			probeVolume();
		  }, 500);
		  
	});
	$('#tempoid').ready(function(){
		var intervalId = setInterval(function() {
			probeTempo();
		  }, 500);
		  
	});
	$('#modeid').ready(function(){
		var intervalId = setInterval(function() {
			probeMode();
		  }, 500);
		  
	});
	//handling data coming back
	// socket.on('commandReply',function(result){
	// 	log.console("result received back are: ", result);
	// 	$('#status').html('<h3>result</h3>');
	// });
	socket.on('timeReceived', function(result)
	{
		console.log("result= ", result)
		$('#displayUpTime').val(result)
	});
	socket.on('volumeReceived', function(result)
	{
		clearTimeout(errorTimer)
		$('#volumeid').val(result)
	});
	socket.on('tempoReceived', function(result)
	{
		
		$('#tempoid').val(result)
	});
	socket.on('trackModeReceived', function(result)
	{
		console.log("Mode received "+result)
		if(result==0){
			$('#modeid').html('Rock1')
		}
		else if(result==1)
		{
			$('#modeid').html('Custom beat')

		}
		else if(result=2)
		{
			$('#modeid').html('None')
		}
	});
	socket.on('error', function(data)
	{
		
		$('#error-box').show();
		$('#error-box').html(data)
	});
});
function probeMode()
{
	socket.emit('getCurrentBeat\n');
}
function highlight_mode(val){
	if(val == 2){
		if(mode_bool == false){
			$('#modeNone').css({"backgroundColor":"yellow"});
			mode_bool = true;
		}
		else{
			$('#modeNone').css({"backgroundColor":"transparent"});
			mode_bool = false;
		}
	}
	else if(val ==1){
		if(mode_bool == false){
			$('#modeRock1').css({"backgroundColor":"yellow"});
			mode_bool = true;
		}
		else{
			$('#modeRock1').css({"backgroundColor":"transparent"});
			mode_bool = false;

		}
	}
	else if (val ==2){
		if(mode_bool == false){
			$('#modeRock2').css({"backgroundColor":"yellow"});
			mode_bool = true;
		}
		else{
			$('#modeRock2').css({"backgroundColor":"transparent"});
			mode_bool = false;
		}
	}
}

function probeVolume()
{
	socket.emit('Volumeget\n');
}
function probeTempo()
{
	socket.emit('BMPget\n');
}
function highlight_component(val){
	if(val == 1){
		if(component_bool == false){
			$('#baseDrum').css({"backgroundColor":"yellow"});
			component_bool = true;
		}
		else{
			$('#baseDrum').css({"backgroundColor":"transparent"});
			component_bool = false;
		}
	}
	else if(val ==2){
		if(component_bool == false){
			$('#hi-hat').css({"backgroundColor":"yellow"});
			component_bool = true;
		}

		else{
			$('#hi-hat').css({"backgroundColor":"transparent"});
			component_bool = false;
		}
	}
	else if (val ==3 ){
		if(component_bool == false){
			$('#snare').css({"backgroundColor":"yellow"});
			component_bool = true;
		}
		else{
			$('#snare').css({"backgroundColor":"transparent"});
			component_bool = false;
		}

	}

}
function updateUpTime()
{
	socket.emit('displayUpTime\n');
	
}
function updateTempoDisplayDown(){
	
	sendTempoDownCommand();
	
	
}

function updateTempoDisplayUp(){
	sendTempoUpCommand();
	
	
}

function updateVolumeDisplayDown(){
	
	sendVolumeDownCommand();
		
}

function updateVolumeDisplayUp(){
	sendVolumeUpCommand();		
	
}

function sendVolumeUpCommand(){
	console.log("volume up command send");
	socket.emit('increaseVolume\n');
}
function sendVolumeDownCommand(){
	console.log("volume down command send");
	socket.emit('decreaseVolume\n');
}

function sendTempoUpCommand(){
	console.log("tempo up command send");
	socket.emit('increaseBMP\n');
}
function sendTempoDownCommand(){
	console.log("tempo down command send");
	socket.emit('decreaseBMP\n');
}

function sendModeCommand(mode){
	if(mode == 2){
		console.log("mode none command send");
		socket.emit('switchToNoBeat\n');
	}
	else if(mode ==0){
		console.log("modeRock1 command send");
		socket.emit('switchToStandardBeat\n');

	}
	else if(mode ==1){
		console.log("mode Rock2 command send");
		socket.emit('switchToCustomBeat\n');
	}
}

function sendElementCommand(element){
	if(element == 1){
		console.log("base drum command send");
		socket.emit('BasePlay\n');

	}
	else if(element ==2){
		console.log("hi-hat command send");
		socket.emit('HiHatPlay\n');

	}

	else if(element ==3 ){
		console.log("snare command send");
		socket.emit('SnarePlay\n');

	}

}

function sendUpTimeCommand() {
	console.log("uptime command send");
	socket.emit('displayUpTime\n');

}
