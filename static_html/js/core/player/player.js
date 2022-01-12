window.g_hls_objects = new Array();
function goLan() {
	//window.navigator.connection.addEventListener('change', networkChangedEventListener()); // Does not work in firefox
	var startNo = document.getElementById('startNo').value;
	var endtNo = document.getElementById('endtNo').value;	
	var videos = document.getElementById('videos');
	// remove all childs of videos
	while (videos.firstChild) {
		videos.removeChild(videos.lastChild);
	}
	for (var i = 0; i < window.g_hls_objects.length; i++) {
		window.g_hls_objects[i].destroy();
	}
	window.g_hls_objects = null;
	window.g_hls_objects = new Array();

	if (startNo < 0) startNo = 0;
	if (endtNo > window.g_source_list.length - 1) endtNo = window.g_source_list.length - 1;
	for (var i = startNo; i <= endtNo ; i++) {		
		var id = 'v_' + i;
		var video = document.createElement('video');
		video.setAttribute("id", id);
		video.setAttribute("controls", "true");
		video.autoplay = true;
		videos.appendChild(video);
		init_hls_object(id, i);
	}
}
function goFS() {
	// element which needs to enter full-screen mode
	var element = document.querySelector("#videos");

	// make the element go to full-screen mode
	element.requestFullscreen()
		.then(function() {
			// element has entered fullscreen mode successfully
		})
		.catch(function(error) {
			// element could not enter fullscreen mode
		});
}