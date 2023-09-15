var formChangeMessage = "Your changes have not been saved.\r\nDo you want to continue?" 
// JScript source code
function IsFormModified(oForm)
{    
	var el, i = 0;		
	while (el = oForm.elements[i++]) 
	{	
		switch (el.nodeName.toLowerCase()) 
		{   
		    	
			// select boxes
			// select boxes
			case "select":			    
				if( el.selectedIndex  != el.getAttribute("data-def"))
					return true;							
				break;
			// input / textarea
			case "textarea":
			case "input":				
			    switch (el.type) 
				{
					case 'text' :							     
						if (!/^\s*$/.test(el.value) && el.value != el.getAttribute("data-def")) 						
						    return true;					
					    break;
					case 'checkbox' :
					case 'radio' :					    					   
						if (el.checked != (el.getAttribute("data-def"))) 						
						    return true;						
					    break;
				}
				break;
		}
	}
	return false;
}


function RefreshAttribute(oForm)
{
	var el, i = 0;
	while (el = oForm.elements[i++]) 
	{		
		switch (el.nodeName.toLowerCase()) 
		{
		
			// select boxes
			// select boxes
			case "select":
				el.setAttribute("data-def", el.selectedIndex);				
				break;
			// input / textarea
			case "textarea":
			case "input":						
				switch (el.type) 
				{

					case 'text' :
						el.setAttribute("data-def", el.value);
					break;
					case 'checkbox' :
					case 'radio' :
					    if(el.checked == true)
						    el.setAttribute("data-def", "1");
					    else if(el.checked == false)
					        el.setAttribute("data-def", "0");
						break;
				}
			break;
		}
	}
	return false;
}

