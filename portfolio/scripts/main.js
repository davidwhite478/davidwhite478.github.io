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

$(".contents .projectLink").click(function(){
    $('html, body').animate({
        scrollTop: $( $(this).find("a").attr('href') ).offset().top-(document.body.offsetWidth*1/500)
    }, 800);
    return false;
});

$(".topArrow").click(function(){    
    $("html, body").animate({ scrollTop: "0px" }, 800);
});