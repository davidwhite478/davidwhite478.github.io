var speedScaler = 550;

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

function calculateSpeed(element)
{
	return parseInt(element.attr('data-pos'))*speedScaler;
}

$(".contents .projectLink").click(function(){
		var aTag = $(this).find("a");
    $('html, body').animate({
        scrollTop: $(aTag.attr('href')).offset().top-(document.body.offsetWidth*1/500)
    }, calculateSpeed(aTag));
    return false;
});

$(".topArrow").click(function(){
    $("html, body").animate({ scrollTop: "0px" }, calculateSpeed($(this)));
});
