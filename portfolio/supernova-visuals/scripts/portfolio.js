function showPreview(divId, backgroundDivId)
{
	backgroundDivId.style.display ='block';
  	divId.style.display='block';
}

function hidePreviews()
{
	for (var i = 0; i < arguments.length; i++) 
	{
    	arguments[i].style.display='none';
    }
}

function pauseVideos()
{
	for (var i = 0; i < arguments.length; i++) 
	{
      if (!videojs(arguments[i]).paused())
    	  {videojs(arguments[i]).pause();}
  }
}

function playWindowVideo(videoId)
{
  videojs(videoId).play();
}

function setVideoVolume(videoId, vol)
{
  videojs(videoId).volume(vol);
}

$('body').keydown(function(e){
        if (e.which==27) /* Escape key. */
          {hideAndPause();}
      });
