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