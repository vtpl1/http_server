// function pausedEventListener(index) {
// 	if (this.paused == true) {
// 		if (window.g_hls_objects[index] != undefined && window.g_hls_objects[index] != null) {
// 			window.g_hls_objects[index].recoverMediaError();
// 		}
// 	}
// }

function delayedStartLoad(index) {
	// console.log("Starting " + index)
	// init_hls_object(index)
}

function sleep(delay) {
	var start = new Date().getTime();
	while (new Date().getTime() < start + delay);
}

function init_hls_object(id, index) {
	if (Hls.isSupported()) {
		window.g_hls_objects[index] = new Hls(config);
		if (window.g_hls_objects[index] != undefined && window.g_hls_objects[index] != null) {
			window.g_hls_objects[index].loadSource(window.g_source_list[index]);
			window.g_hls_objects[index].attachMedia(document.getElementById(id));
			window.g_hls_objects[index].on(Hls.Events.MANIFEST_PARSED, function () {
				console.log("Manifest Parsed");
				document.getElementById(id).play();
			});
			// window.g_hls_objects[index].on(Hls.Events.MEDIA_DETACHED, function () {
			// 	console.error("Media Detached");
			// 	window.g_hls_objects[index].attachMedia(document.getElementById(id));
			// });

			window.g_hls_objects[index].on(Hls.Events.ERROR, function (event, data) {
				if (data.fatal) {
					// console.error('Fatal error :' + data.details);
					switch (data.type) {
						case Hls.ErrorTypes.MEDIA_ERROR:
							console.error('MONOTOSH MEDIA_ERROR :' + data.details);
							window.g_hls_objects[index].recoverMediaError();
							break;
						case Hls.ErrorTypes.NETWORK_ERROR:
							console.error('MONOTOSH NETWORK_ERROR :' + data.details);
							// setTimeout(function(){console.log("Delaying...");}, 1000);
							sleep(10000);
							window.g_hls_objects[index].loadSource(window.g_source_list[index]);
							window.g_hls_objects[index].startLoad();
							break;
						default:
							console.error('An unrecoverable error occured');
							window.g_hls_objects[index].destroy();
							break;
					}
				} else {
					// console.error('Not Fatal error :' + data.details);
				}
			});
			document.getElementById(id).play();
			// document.getElementById(id).addEventListener('pause', pausedEventListener(index));
		}
	} else {
		console.error("HLS is not supported");
	}
}