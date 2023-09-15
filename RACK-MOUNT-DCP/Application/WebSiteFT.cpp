/* $Header:   D:/databases/VMdb/archives/DCX/RackMount/DCP/Application/WebSiteFT.cpp_v   1.1   07 Jun 2016 12:15:24   GButron  $*/
/*****************************************************************************
$Log:   D:/databases/VMdb/archives/DCX/RackMount/DCP/Application/WebSiteFT.cpp_v  $
 * 
 *    Rev 1.1   07 Jun 2016 12:15:24   GButron
 * Old revisions removed
 * 
 *    Rev 1.0   06 Jun 2016 16:41:44   GButron
 * Initial revision.
*/

#include "WebSiteFT.h"

#define POSTREQUESTOFFSET                            1
#define URLSTRINGOFFSET                              1

const STR *WebSiteFT::HtmlTerminalPtr=
		"<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">"
		"<html xmlns=\"http://www.w3.org/1999/xhtml\">"
		"<head>"
		"<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\" />"
		"<title>BRANSON FACTORY COMMANDS</title>"
		"<script type=\"text/JavaScript\" language=\"javascript\" src=\"terminal.js\"></script>"
		"</head>"
		"<body style=\"background:black;color:yellow;font-family:monospace;\">"
		" <div id=\"prompt\">"
		"  <span id=\"cursor\">FT></span><input id=\"ft\" type=\"text\" name=\"ft\" style=\"font-family:monospace;width:95%;background-color:black;color:yellow;background-color:transparent;border:0px solid;outline:none;\" onkeyup=\"javascript:this.value=this.value.toUpperCase();\" onkeypress=\"window.terminal.cmd(event);\" />"
		" </div>"
		"</body>"
		"</html>";


const STR *WebSiteFT::JsTerminalPtr=
		"function Terminal() {"
		""
		" var that=this;"
		""
		" var ajaxSend=function sendAjaxXMLDoc(pCmd)"
		" {"
		"   var xmlhttp;"
		"   if (window.XMLHttpRequest) {"
		"	   xmlhttp=new XMLHttpRequest();"
		"   } else {"
		"	   xmlhttp=new ActiveXObject(\'Microsoft.XMLHTTP\');"
		"   };"
		"   xmlhttp.onreadystatechange=function() {"
		"	 if (xmlhttp.readyState==4 && xmlhttp.status==200) {"
		"		var prompt=document.getElementById(\'prompt\');"
		"		var responseElement= document.createElement(\'span\');"
		"		responseElement.innerHTML=xmlhttp.responseText;"
		"		prompt.parentNode.insertBefore(responseElement,prompt);"
		"		prompt.parentNode.insertBefore(document.createElement(\'br\'),prompt);"
		"	 };"
		"   };"
		"   xmlhttp.open(\'POST\',\'ft\',false);"
		"   xmlhttp.setRequestHeader(\'Content-type\',\'application/x-www-form-urlencoded\');"
		"   xmlhttp.send(pCmd);"
		" };"
		""
		" this.focus = function() {"
		"	 document.getElementById(\'ft\').focus();"
		" };"
        ""
		" this.cmd = function(event) {"
		"	 var keycode = (event.keyCode ? event.keyCode : event.which);"
		"	 var prompt=document.getElementById(\'prompt\');"
		"	 var cursor=document.getElementById(\'cursor\');"
		"	 var ft=document.getElementById(\'ft\');"
		""
		"	 var oldPrompt = document.createElement(\'span\');"
		"	 oldPrompt.innerHTML=cursor.innerHTML+ft.value;"
		"	 if(keycode==\'13\') {"
		"		 prompt.parentNode.insertBefore(oldPrompt,prompt);"
		"		 prompt.parentNode.insertBefore(document.createElement(\'br\'),prompt);"
		"		 if(ft.value!=\'\') {"
		"            if(ft.value=='?') {"
		"               for (var nCount=0;nCount<80;nCount++) {"
		"                  ajaxSend('ft=HE '+nCount);"
		"               }"
		"            } else {"
		"	           ajaxSend('ft='+ft.value);"
		"            }"
		"        }"
		"		 ft.value=\'\';"
		"		 window.scrollTo(0,document.body.scrollHeight);"
		"		 that.focus();"
		"	 };"
		" };"
        ""
		" document.onclick=function(){"
		"	 that.focus();"
		" };"
		""
		" window.onload=function() {"
		"	 that.focus();"
		" };"
		""
		"};"
        ""
		"delete window.terminal;"
		"window.terminal = new Terminal();";


/******************************************************************************************
 *  void WebSiteFT::InitWebSite(UINT8 Interface,UINT8 Port)                               *
 *                                                                                        *
 *  Purpose:                                                                              *
 *   	 This function Initialize the web factory commands web page                         *
 *  Entry condition:                                                                      *
 *	    Interface - Ethernet interface 0..n                                                *
 *		 Port - Port number                                                                 *
 *                                                                                        *
 *  Exit condition:                                                                       *
 *  	 None                                                                               *
 *****************************************************************************************/
void WebSiteFT::InitWebSite(UINT8 Interface,UINT8 Port)
{
	CyGOS::AddHttpGetHandler(Interface, Port, WebGetHandler);
	CyGOS::AddHttpPostHandler(Interface, Port, WebPostHandler);
}


/******************************************************************************************
 *  BOOLEAN WebSiteFT::WebPostHandler(STR *PostPtr,STR *UrlPtr, HTTPReplyHeader &Header)  *
 *                                                                                        *
 *  Purpose:                                                                              *
 *   	This function is used to process the WEB POST request by the client                 *
 *  Entry condition:                                                                      *
 *		PostPtr - String pointer to get the POST request.                                   *
 *		UrlPtr  - String pointer to determine the page name                                 *
 *		Header  - Header address to get the response                                        *
 *                                                                                        *
 *  Exit condition:                                                                       *
 *  	Boolean response flag.                                                              *
 *****************************************************************************************/
BOOLEAN WebSiteFT::WebPostHandler(STR *PostPtr,STR *UrlPtr, HTTPReplyHeader &Header)
{
	BOOLEAN RetValFlag = TRUE;
	File *FilePtr = 0;
	SINT32 ContentLengthCtr = 0;
	STR InputFTStr[COMMAND_INPUT_SIZE] = {0};
	STR OutPutFtStr[COMMAND_OUTPUT_SIZE] = {0};
	STR *WebPostValuePtr = NULL;
	BOOLEAN CommandOkFlag = FALSE;

	WebPostValuePtr = strchr(PostPtr,'=') + POSTREQUESTOFFSET;
	strcpy(InputFTStr,WebPostValuePtr);
	CommandOkFlag = CommandsFT::Execute(InputFTStr,OutPutFtStr);
	ContentLengthCtr = strlen(OutPutFtStr);
	Header.reqType = reqData;
	Header.file = FilePtr;
	Header.contentLength = ContentLengthCtr;
	Header.data = OutPutFtStr;
	Header.contentType=texthtml;
	return RetValFlag;
}


/****************************************************************************************
 *  BOOLEAN WebSiteFT::WebGetHandler(STR *UrlPtr, HTTPReplyHeader &Header)              *
 *                                                                                      *
 *  Purpose:                                                                            *
 *   	This function is use to process the WEB GET request by the client                 *
 *  Entry condition:                                                                    *
 *		UrlPtr - String pointer to determine the page name                                *
 *		Header - Header address to get the response                                       *
 *                                                                                      *
 *  Exit condition:                                                                     *
 *  	Boolean response flag.                                                            *
 ****************************************************************************************/
BOOLEAN WebSiteFT::WebGetHandler(STR *UrlPtr, HTTPReplyHeader &Header)
{
	BOOLEAN RetValFlag = FALSE;
	STR FileNameStr[50] = {0};
	File *FilePtr = NULL;

	strcpy(FileNameStr,"");
	if(strlen(UrlPtr + URLSTRINGOFFSET) > MAXFILELEN) {
	   pprintf("\n Get File Name Too Long\n");
		return FALSE;
	}
	strcat(FileNameStr, UrlPtr + URLSTRINGOFFSET);
	if(*(UrlPtr + URLSTRINGOFFSET)) {//Indicates there is a request from some specific html file
	   if(strstr(FileNameStr,".html")) {
	      strcpy(FileNameStr,"terminal.html");
	   }
   }
	else {
 	   strcpy(FileNameStr,"terminal.html");
   }
	if(strstr(FileNameStr,"terminal.html")) {
		Header.contentLength = strlen(HtmlTerminalPtr);
		Header.reqType = reqData;
		Header.data = (STR*)HtmlTerminalPtr;
		Header.contentType=texthtml;
		RetValFlag = TRUE;
	}
	if(strstr(FileNameStr,"terminal.js")) {
		Header.contentLength = strlen(JsTerminalPtr);
		Header.reqType = reqData;
		Header.data = (STR*)JsTerminalPtr;
		Header.contentType=textjs;
		RetValFlag = TRUE;
	}
	Header.file = FilePtr;
	Header.statusCode = httpOk;
	return RetValFlag;
}
