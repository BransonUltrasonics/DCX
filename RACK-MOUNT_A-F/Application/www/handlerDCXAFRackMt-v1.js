var Func =
{
   First:0,
   SetupWeldGetDatahandler:1, 
   SetupWeldPostDatahandler:2,
   SetupLimitsGetDatahandler:3,
   SetupLimitsPostDatahandler:4,
   SetupSaveRecallGetDatahandler:5,
   SetupSaveRecallPostDatahandler:6,
   SetupAmplitudeGetDatahandler:7,
   SetupAmplitudePostDatahandler:8,
   SetupSeekPowerUpGetDatahandler:9,
   SetupSeekPowerUpPostDatahandler:10,
   SetupAdvancedGetDatahandler:11,
   SetupAdvancedPostDatahandler:12,
   ViewSystemInfoGetDatahandler:13,
   ProcessScanCommand:14,
   ViewAlarmLogGetDatahandler:15,
   ViewEventLogGetDatahandler:16,
   ViewHistoryGetDatahandler:17,
   ViewWeldGraphGetDatahandler:18,
   ViewWeldGraphPostDatahandler:19,
   DiagnosticTestGetDatahandler:20,
   DiagnosticTestPostDatahandler:21,
   DiagnosticHornSignatureGetDatahandler:22,
   DiagnosticHornSignaturePostDatahandler:23,
   DiagnosticUserIOGetDatahandler:24,
   DiagnosticUserIOPostDatahandler:25,
   DiagnosticFieldBusGetDatahandler:26,
   ConfigurationSystemGetDatahandler:27,
   ConfigurationSystemPostDatahandler:28,
   ConfigurationUserIOGetDatahandler:29,
   ConfigurationUserIOPostDatahandler:30,
   ConfigurationIPSetupGetDatahandler:31,
   ConfigurationIPSetupPostDatahandler:32,
   ConfigurationPasscodesGetDatahandler:33,
   ConfigurationPasscodesPostDatahandler:34,
   ConfigurationAlarmGetDatahandler:35,
   ConfigurationAlarmPostDatahandler:36,
   ConfigurationRFSwitchGetDatahandler:37,
   ConfigurationRFSwitchPostDatahandler:38,
   BransonSecretValuesGetDatahandler:39,
   BransonSecretValuesPostDatahandler:40,
   BransonSystemTestGetDatahandler:41,
   BransonBoardTestGetDatahandler:42,
   BransonBoardTestPostDatahandler:43,
   BransonFieldBusTestGetDatahandler:44,
   BransonFieldBusTestPostDatahandler:45,
   BransonAdvRnDTestGetDatahandler:46,
   BransonAdvRnDTestPostDatahandler:47,
   BransonFieldServiceGetDatahandler:48,
   BransonFieldServicePostDatahandler:49,
   BransonSpecialPresetGetDatahandler:50,
   BransonSpecialPresetPostDatahandler:51,
   SetLanguageAndUserID:52,
   SetupAdvancedDiagnosticGetDatahandler:53,
   SetupAdvancedDiagnosticPostDatahandler:54,
   GetWebToken:55
};
var Command =
{
	REQ_NONE:0,
	REQ_RESTORE:1,
	REQ_DCPEVENTDATA:2,
	REQ_WCEVENTDATA:3,
	REQ_DOWNLOAD_DCPEVENTDATA:4,
	REQ_DOWNLOAD_WCEVENTDATA:5,
	REQ_ALARMDATA:6,
	REQ_DOWNLOAD_ALARMDATA:7,
	REQ_SCAN_START:8,
	REQ_SCAN_POLL:9,
	REQ_SCAN_ABORT:10,
	REQ_SEEK_START:11,
	REQ_SEEK_POLL:12,
	REQ_SEEK_STOP:13,
	REQ_SEEK_RESULT:14,
	REQ_DIAG_START:15,
	REQ_DIAG_STOP:16,
	REQ_DIAG_POLL:17,
	REQ_DIAG_LOOPBACK_START:18,
	REQ_CLEAR_ALARMLOG:19,
	REQ_CLEAR_EVENTLOG:20,
	REQ_DIAG_LOOPBACK_POLL:21,
	REQ_SEARCH_WELDERS:22,
	REQ_BOARDTEST_LOGIN:23,
	REQ_BOARDTEST_POLL:24,
	REQ_BOARDTEST_REFRESH:25,
	REQ_MFONLOAD:26,
	REQ_DOWNLOAD_CYCLERESULTS:27,
	REQ_RESTOREDEFAULTS:28,
	REQ_CLEAR_ALARMLOGMF:29,
	REQ_CLEAR_EVENTLOGMF:30,
	REQ_RESET_ALARMCOUNTER:31,
	REQ_STARTSTOPCYCLE:32,
	REQ_MFPOLL:33,
	REQ_SAVE_SYSSETUP:34,
	REQ_MFTEST_START:35,
	REQ_MFTEST_STOP:36,
	REQ_MFTEST_POLL:37,
	REQ_SAVE_PRESET:38,
	REQ_VERIFY_PRESET:39,
	REQ_RECALL_PRESET:40,
	REQ_RESET_CYCLERUN:41,
	REQ_CURRENT_DATA:42,
	REQ_PHASE_DATA:43,
	REQ_AMPLITUDE_DATA:44,
	REQ_FREQUENCY_DATA:45,
	REQ_PRESET_DATA:46,
	REQ_TIME_DATA:47,
	REQ_POWER_DATA:48,
	REQ_PWMAMPLITUDE_DATA:49,
	REQ_WELDHISTORY:50,
	REQ_DOWNLOAD_WELDHISTORY:51,
	REQ_CLEAR_WELDHISTORY:52,
	REQ_DOWNLOAD_FRAM_CONFIGURATION:53,
	REQ_FANCONTROL:54,
	REQ_RESFREQ:55,
	REQ_SAVE:56,
	REQ_TOKEN:57,
	REQ_COLDSTART:58,
	REQ_SYSCALIBRATION:59,
	REQ_RUN_START:60,
	REQ_CLEAR_DCPEVENTLOG:61,
	REQ_CLEAR_MEMORYFREQ:62,
	REQ_DATETIME:63
};
function UseID(element){ return document.getElementById(element);}
function UseName(element){ return document.getElementsByName(element);}
var XPad;InvalidCode = "The User ID and/or User Passcode you entered is not recognized, Please try again!";
var strRestoreDefault = "Are you Sure you want to Restore Defaults?";
var DivPresent = false;
var Notoken = "Token Not Available.";
var NotokenWait = "The Token is not Available and Needs to be Released First";
var BusyInfo = "INFO : System is busy";
var GDInfo = "INFO : Getting Graph Data";
var TimePat23 = /^(\d{1,2})[.](\d{1,3})$/;
var EnergyPat31 = /^\d+(?:\.\d{1})?$/;
var FreqPat = /^-?\d{1,4}$/;
function EnableDiv() {
    var fade_height = $('#content').outerHeight();
	var fade_width = $('#content').outerWidth();
	window.scrollTo(0,220);	
    $('#fade_div').css('display','block');
    $('#info_div').css('display','block');
    $('#fade_div').css('height',fade_height);
	$('#fade_div').width(fade_width);
	$('#USERPASSCODE').focus();	
	DivPresent = true;
}
function DisableDiv() {
    UseID('fade_div').style.display = 'none';
    UseID('info_div').style.display = 'none';
	DivPresent = false;
}
function getCookie(c_name) {
    if (document.cookie.length > 0) {
        c_start = document.cookie.indexOf(c_name + "=");
        if (c_start != -1) {
            c_start = c_start + c_name.length + 1;
            c_end = document.cookie.indexOf(";", c_start);
            if (c_end == -1) c_end = document.cookie.length;
            return unescape(document.cookie.substring(c_start, c_end));
        }
    }
    return "";
}
function SendCookies(){
	return ("&lang=" + getCookie('LANGUAGE') + "&usrid=" + getCookie('USERID') + "&passcode=" + 
			getCookie('USERPASSCODE') + "&uniqueid=" + getCookie('UNIQUEID'));
}
function SubmitCookies(hrefpage){
	UseID("lang").value = getCookie('LANGUAGE');
	UseID("usrid").value = getCookie('USERID');
	UseID("passcode").value = getCookie('USERPASSCODE');
	UseID("uniqueid").value = getCookie('UNIQUEID');
	document.temp_form.action = hrefpage;
}
function CheckSession(Response){
	var SessionValid = true;
	if(Response == "IDMISMATCH"){
		SessionValid = false;
		location.href = "index.html";		
	}
	return SessionValid;
}
function getHTTPObject() {
    var xmlhttp;
    if (!xmlhttp && typeof XMLHttpRequest != 'undefined') {
        try {
            xmlhttp = new XMLHttpRequest();
        } catch (e) {
            xmlhttp = false;
        }
    }
    return xmlhttp;
}
function IsNumeric(strString) {
	var strValidChars = "0123456789";
	var strChar;
	var blnResult = true;
	if (strString.split('.').length > 2) {
		return (false);
	}
	if (strString.length == 0) return false;
	for (i = 0; i < strString.length && blnResult == true; i++) {
		if (strString[0] == '-')
			continue;
		strChar = strString.charAt(i);
		if (strValidChars.indexOf(strChar) == -1) {
			blnResult = false;
		}
	}
	return blnResult;
}
function GetToken(CallType) {
	var GetTokenString = "func=" + Func.GetWebToken + "cmd=" + Command.REQ_TOKEN + "?param=" + SendCookies();
	http.open("POST", GetTokenString, true);
	if(CallType == "Loc")
		http.onreadystatechange = GetTokenResponseHandlerLoc;
	else
		http.onreadystatechange = GetTokenResponseHandler;
	http.send(null);
}
function GetTokenResponseHandler() {
    if ((http.readyState == 4) && (http.responseText.length != 0) && (CheckSession(http.responseText) == true)) {
        var Token = parseFloat(http.responseText);
        if (Token)
            SetToken(1, false);
        else {
            SetToken(0, false);
            alert(NotokenWait);
        }
    }
}
function GetSystemType() {
	var GetSystemTypeString = "func=" + Func.GetWebToken + "cmd=" + Command.REQ_NONE + "?param=" + SendCookies();
	http.open("POST", GetSystemTypeString, true);
	http.onreadystatechange = GetSystemTypeResponseHandler;
	http.send(null);
}
function GetSystemTypeResponseHandler(){
    if ((http.readyState == 4) && (http.responseText.length != 0))
		SetSystemType(parseFloat(http.responseText));   
}
function setCookie(c_name, value, expiredays){
    var exdate = new Date();
    exdate.setDate(exdate.getDate() + expiredays);
    document.cookie = c_name + "=" + escape(value) + ((expiredays == null) ? "" : ";expires=" + exdate.toUTCString());
}
var sticky_offset;
var StickyPresent;
function InitStickyFooter(){
	if(DivPresent == false){
		if(UseID('contentButtonBank') != null){
			StickyPresent = true;
			var original_position_offset = $('#contentButtonBank').offset();
			var window_height = $(window).height();
			sticky_offset = original_position_offset.top;
			if(window_height < sticky_offset)
				$('#contentButtonBank').css('position', 'fixed');
		}
		else
			StickyPresent = false;
	}
	else
		StickyPresent = false;
}
$(window).scroll(function () {
	if(StickyPresent == true){
		var sticky_height = $('#contentButtonBank').outerHeight();
		var where_scroll = $(window).scrollTop();
		var window_height = $(window).height();
		if((where_scroll + window_height) > sticky_offset - 50) {
			$('#contentButtonBank').css('position', 'relative');
		}
		if((where_scroll + window_height) < (sticky_offset + sticky_height)){
			$('#contentButtonBank').css('position', 'fixed');
		}
	}
});
$(window).resize(function () {
	if(StickyPresent == true){
		var sticky_height = $('#contentButtonBank').outerHeight();
		var where_scroll = $(window).scrollTop();
		var window_height = $(window).height();
		if((where_scroll + window_height) > sticky_offset - 50) {
			$('#contentButtonBank').css('position', 'relative');
		}
		if((where_scroll + window_height) < (sticky_offset + sticky_height)){
			$('#contentButtonBank').css('position', 'fixed');
		}
	}
});
var PageHeaderStr = [	["Setup"		, "Weld"		, "Amplitude"		, "Save/Recall Preset"	, "Limit/Cutoff" , "Seek/Power Up"	, "Advanced Weld"	, "Adv Diagnostics" ],
						["View"			, "System Info"	, "Alarm Log"		, "Event Log"			, "History"		 , "Weld Graphs"	, ""				, ""				],
						["Diagnostics"	, "Seek"		, "Horn Signature"	, "User I/O"			, "Fieldbus"     , "Fieldbus Test"  , ""				, ""				],
						["Configuration", "System"		, "User I/O"		, "Communication"		, "Passcodes"    , "Alarms"			, "RF Switch"		, ""			 	],
						["Branson"		, "Secret Values", "System Test"	, "Board Test"			, "Adv R&D"		 , "Field Service"	, "Special Presets"	, ""				],
					];
var TabArray = [	["SetupTab"	, "WeldTab"		, "AmplitudeTab"	, "SaveRecallTab" , "LimitCutoffTab"  , "SeekPowerUpTab" , "AdvancedTab" , "AdvancedDiagnosticsTab" ],
					["ViewTab"	, "SysInfoTab"	, "AlarmLogTab"		, "EventLogTab"	  , "HistoryTab"	  , "WeldGraphTab"   , ""			 , ""						],
					["DiagTab"  , "SeekTab"		, "HornSignatureTab", "UserIOTab"	  , "FieldBusTab"     , "FieldBusTestTab", ""			 , ""						],
					["ConfigTab", "systemTab"	, "ConfigUserIOTab"	, "IPSetupTab"	  , "PasscodesTab"    , "AlarmsTab"      , "RFSwitchTab" , ""			 			],
					["BrnsnTab" , "SecretValuesTab",  "SystemTab"	, "BoardTestTab"  , "AdvRnDTab"		  , "FieldServiceTab" , "SpecialPresetTab",""					],
				];
function SetTabAttribute(TabId,SubTabId){
	UseID(TabArray[TabId][0]).style.background = '#CCCCCC';
	UseID(TabArray[TabId][0]).style.color = 'black';
	UseID(TabArray[TabId][SubTabId]).style.background = '#CCCCCC';
	UseID(TabArray[TabId][SubTabId]).style.color = 'black';
	UseID('PageHeader').innerHTML = PageHeaderStr[TabId][0] + " - " + PageHeaderStr[TabId][SubTabId];
}
var sOption = "toolbar=0, location=0, directories=0, menubar=0, scrollbars=0, width=400, height=120, left=300, top=250";
var MainHeader =
"<div id=\"headerNav\">"
+"	<div id=\"headerLogo\"></div>"
+"	<div id=\"headerLinks\">"
+"		<span class=\"user\" id=\"CurrUser\">Current User: </span><a href=\"javascript:SubmitForm('index.html')\">Logout</a>"
+"	</div>"
+"</div>"
+"<div id=\"mainNav\">"
+"<ul class=\"sf-menu\" id=\"sfm\">"
+"	<li><a id=\"SetupTab\" href=\"javascript:SubmitForm('Weld.html')\">Setup</a>"
+"		<ul id=\"adtab\">"
+"			<li><a id=\"WeldTab\" href=\"javascript:SubmitForm('Weld.html')\">Weld</a></li>"
+"			<li><a id=\"AmplitudeTab\" href=\"javascript:SubmitForm('Amplitude.html')\">Amplitude</a></li>"
+"			<li><a id=\"SaveRecallTab\" href=\"javascript:SubmitForm('SaveRecall.html')\">Save/Recall Preset</a></li>"
+"			<li><a id=\"LimitCutoffTab\" href=\"javascript:SubmitForm('Limits.html')\">Limit/Cutoff</a></li>"
+"			<li><a id=\"SeekPowerUpTab\" href=\"javascript:SubmitForm('SavePowerUp.html')\">Seek/Power Up</a></li>"
+"			<li style=\"display: none\"><a id=\"AdvancedTab\" href=\"javascript:SubmitForm('Advanced.html')\">Advanced Weld</a></li>"
+"			<li style=\"display: none\"><a id=\"AdvancedDiagnosticsTab\" href=\"javascript:SubmitForm('AdvancedDiagnostics.html')\">Adv Diagnostics</a></li>"
+"		</ul>"
+"	</li>"
+"	<li><a id=\"ViewTab\" href=\"javascript:SubmitForm('SystemInfo.html')\">View</a>"
+"		<ul>"
+"			<li><a id=\"SysInfoTab\" href=\"javascript:SubmitForm('SystemInfo.html')\">System Info</a></li>"
+"			<li><a id=\"AlarmLogTab\" href=\"javascript:SubmitForm('AlarmLog.html')\">Alarm Log</a></li>"
+"			<li><a id=\"EventLogTab\" href=\"javascript:SubmitForm('EventLog.html')\">Event Log</a></li>"
+"			<li><a id=\"HistoryTab\" href=\"javascript:SubmitForm('History.html')\">History</a></li>"
+"			<li><a id=\"WeldGraphTab\" href=\"javascript:SubmitForm('WeldGraph.html')\">Weld Graphs</a></li>"
+"		</ul>"
+"	</li>"    
+"	<li><a id=\"DiagTab\"href=\"javascript:SubmitForm('Seek.html')\">Diagnostics</a>"
+"		<ul id=\"DiagnosticsTab\">"
+"			<li><a id=\"SeekTab\" href=\"javascript:SubmitForm('Seek.html')\">Seek</a></li>"
+"			<li><a id=\"HornSignatureTab\" href=\"javascript:SubmitForm('HornSignature.html')\">Horn Signature</a></li>"
+"			<li><a id=\"UserIOTab\" href=\"javascript:SubmitForm('UserIO.html')\">User I/O</a></li>"
+"			<li style=\"display:none\"><a id=\"FieldBusTab\" href=\"javascript:SubmitForm('FieldBus.html')\">Fieldbus</a></li>"
+"			<li style= \"display:none\"><a id=\"FieldBusTestTab\" href=\"javascript:SubmitForm('FieldBusTest.html')\">Fieldbus Test</a></li>"
+"		</ul>"
+"	</li>"
+"	<li><a id=\"ConfigTab\" href=\"javascript:SubmitForm('system.html')\">Configuration</a>"
+"		<ul>"  
+"			<li><a id=\"systemTab\" href=\"javascript:SubmitForm('system.html')\">System</a></li>"
+"			<li><a id=\"ConfigUserIOTab\" href=\"javascript:SubmitForm('ConfigUserIO.html')\">User I/O</a></li>"
+"			<li><a id=\"IPSetupTab\" href=\"javascript:SubmitForm('IPSetup.html')\">Communication</a></li>"
+"			<li><a id=\"PasscodesTab\" href=\"javascript:SubmitForm('Passcodes.html')\">Passcodes</a></li>"
+"			<li><a id=\"AlarmsTab\" href=\"javascript:SubmitForm('Alarms.html')\">Alarms</a></li>"
+"			<li><a id=\"RFSwitchTab\" href=\"javascript:SubmitForm('RFSwitch.html')\">RF Switch</a></li>"
+"		</ul>"
+"	</li>"   
+"	<li style=\"display:none\"><a id=\"BrnsnTab\" href=\"javascript:SubmitForm('SecretValues.html')\">Branson</a>"
+"		<ul id=\"BransonTab\">"
+"			<li><a id=\"SecretValuesTab\" href=\"javascript:SubmitForm('SecretValues.html')\">Secret Values</a></li>"
+"			<li><a id=\"SystemTab\"href=\"javascript:SubmitForm('SystemTest.html')\">System Test</a></li>"
+"			<li><a id=\"BoardTestTab\"href=\"javascript:SubmitForm('BoardTest.html')\">Board Test</a></li>"
+"			<li><a id=\"AdvRnDTab\" href=\"javascript:SubmitForm('AdvRnD.html')\">Adv R&D</a></li>"
+"			<li><a id=\"FieldServiceTab\" href=\"javascript:SubmitForm('FieldService.html')\">Field Service</a></li>"
+"			<li><a id=\"SpecialPresetTab\" href=\"javascript:SubmitForm('SpecialPreset.html')\">Special Presets</a></li>"
+"		</ul>"
+"	</li>"
+"</ul>"
+"<button type=\"button\" id=\"WebToken\" class=\"button\" onclick=\"GetToken()\"></button>"
+"</div>";

$('#footer').ready(function () {
	UseID('footer').innerHTML =  "<p> &copy;2013 Branson, All Rights Reserved<br />"
								+"<img src=\"Emerson-IA.jpg\" alt=\"Emersin Industrial Automation\" /></p>";
});
function SetUserType(UserID){	
	UseID("CurrUser").innerHTML = "Current User: " + getCookie('USERID');
	if (UserID != '3') return;
	UseID("adtab").children[5].style.display = "inline";
	UseID("adtab").children[6].style.display = "inline";
	UseID("sfm").children[4].style.display = "inline";
}
function SetToken(TokenFlag,RestFlag){	
	if(TokenFlag == '1')
		UseID("WebToken").style.backgroundImage = "url(greenDot.gif)";
	else
		UseID("WebToken").style.backgroundImage = "url(redDot.gif)";
	if ((RestFlag == true) && (TokenFlag != '1'))
		RestoreMsg = "Token Not Available.";
}
function SetSystemType(SysID){
	if (SysID == '2' || SysID == '4'){
		UseID("WebToken").style.display = "inline";
		UseID("DiagnosticsTab").children[3].style.display = "inline";
		UseID("DiagnosticsTab").children[4].style.display = "inline";
	}
}
function FormatTime(TimeVal){ return (parseInt(TimeVal,10) / 1000).toFixed(3); }
function ToHrsMin(TimeVal){
	var Hrs = parseInt(TimeVal / (3600000), 10).toString();
	var mins = parseInt(TimeVal % (3600000), 10);
	mins = parseInt((mins / 60000), 10).toString();
	if (Hrs.length < 2)
        Hrs = "0" + Hrs;
	if (mins.length < 2) 
		mins = "0" + mins;
	return Hrs + ":" + mins;
}
function ToSecMsec(TimeVal){
	var temp1,secs = (parseInt(TimeVal,10) / 1000).toFixed(3);
	temp1 = (parseInt(secs,10)).toString();
	return ((temp1.length < 2)? '0':'') + secs;
}
function IfValid(ElementName,ElementID){
	var temp, returnstr = "";
	var minmax;
	temp = UseID(ElementID).value;
	if((UseID(ElementID).value.length != 0) && (!isNaN(temp))){
		minmax = UseID(ElementID + "MaxMin").value.split(',');
		
		if(isInteger(temp)){		
			if(UseID(ElementID).value < parseInt(minmax[0]) || UseID(ElementID).value > parseInt(minmax[1]))
				returnstr = "* " + ElementName + " should be between " + minmax[0] + " to " + minmax[1] + "\n";
		}
		else{
			var len = minmax[1].toString().length;
			var format = "";
			for(var i = 0; i < len; i++){
				 format += "X";
			}
			returnstr = "* " + ElementName + " format should be " + format + " \n";
		}			
	}
	else
		returnstr = "* Invalid " + ElementName + "\n";
	return returnstr;
}
function IfValidFreq(ElementName,ElementID){
	var temp, returnstr = "";
	var minmax;
	temp = UseID(ElementID).value;
	if((UseID(ElementID).value.length != 0) && (!isNaN(temp))){	
		var matchArr = temp.toString().match(FreqPat);
		if (matchArr == null) 
			returnstr = "* " + ElementName + " format should be (+/-)XXXX \n";
		else{
			minmax = UseID(ElementID + "MaxMin").value.split(',');
			if(UseID(ElementID).value < parseFloat(minmax[0]) || UseID(ElementID).value > parseInt(minmax[1]))
			returnstr = "* " + ElementName + " should be between " + minmax[0] + " to " + minmax[1] + "\n";
		}			
	}
	else
		returnstr = "* Invalid " + ElementName + "\n";
	return returnstr;
}
function IfValidEnergy(ElementName,ElementID){
	var temp, returnstr = "";
	var minmax;
	temp = UseID(ElementID).value;
	if((UseID(ElementID).value.length != 0) && (!isNaN(temp))){
		if(temp >= 1000){
			if(isInteger(temp)){
				minmax = UseID(ElementID + "MaxMin").value.split(',');
				if(UseID(ElementID).value < parseFloat(minmax[0]) || UseID(ElementID).value > parseInt(minmax[1]))
				returnstr = "* " + ElementName + "(J) should be between " + minmax[0] + " to " + minmax[1] + "\n";
			}
			else
				returnstr = "* Please enter value in XXXX format for " + ElementName + "(J) in range 1000 to 9999 " + "\n";
		}
		else{
			var matchArr = temp.toString().match(EnergyPat31);
			if (matchArr == null) 
				returnstr = "* " + ElementName + "(J) format should be X.Y\n";
			else{
				minmax = UseID(ElementID + "MaxMin").value.split(',');
				if(UseID(ElementID).value < parseFloat(minmax[0]) || UseID(ElementID).value > parseInt(minmax[1]))
				returnstr = "* " + ElementName + " should be between " + minmax[0] + " to " + minmax[1] + "\n";
			}
		}
	}
	else
		returnstr = "* Invalid " + ElementName + "\n";
	return returnstr;
}


/*
if (isInteger(loginID)) {
		var str = parseInt(loginID, 10);
		if (str <= max32bitNumber)
			exitvalue2 = true;
		else
			exitvalue2 = false;
	}
*/	

function IfValidTime(ElID,Msg){
	var returnstr = true;
	var TimeStr = UseID(ElID).value;
	var temp = UseID(ElID + "MaxMin").value.split(",");
	var minVal = FormatTime(parseInt(temp[0], 10));
	var maxVal = FormatTime(parseInt(temp[1], 10));
	matchArray = TimeStr.match(TimePat23);
	if(!Msg) Msg = ElID;
	if (matchArray == null) 
		returnstr = "* " + Msg + "(s) format should be XX.YYY\n";
	else {
		currVal = (TimeStr * 1000);
		if ((currVal < (minVal * 1000)) || (currVal > (maxVal * 1000)))
			returnstr = "* " + Msg + "(s) should be between " + minVal + " and " + maxVal + "\n";
	}
	return returnstr;
}
function trim(value) {
	var re = /\s*((\S+\s*)*)/;
	value = value.replace(re, "$1");
	re = /((\s*\S+)*)\s*/;
	value = value.replace(re, "$1");
	return value;
}
function isInteger(s) {
    if (s.length <= 0) return false;
    for (var i = 0; i < s.length; i++) {
        var c = s.charAt(i);
        if (((c < "0") || (c > "9")))
            return false;
    }
    return true;
}
function IsAlphaNumeric(str) {
    if ((str == "") || (str == null))
        return false;
    for (var j = 0; j < str.length; j++) {
        var alphaa = str.charAt(j);
        var hh = alphaa.charCodeAt(0);
        if (!((hh > 47 && hh < 58) || (hh > 64 && hh < 91) || (hh > 96 && hh < 123)))
            return false;
    }
    return true;
}
function pad2(number){return (number < 10 ? '0' : '') + parseInt(number,10);}
function getXPixel(val, max) {
    return ((graphwidth - xPadding) / max) * val + (xPadding * 1.5) - XPad;
}
function getYPixel(val, max) {
    return graphheight - (((graphheight - yPadding) / max) * val) - yPadding;
}
function PlotGraph(c,Arr,MaxY,Color,YOffset,StartTime,Start4Freq,YPad){
	y1Padding = y1Padding + YPad;
	var Diff = MaxY / 10;           
	MaxY = MaxY + (Diff - (Diff % 10) + 10);
	c.lineWidth = 1;
	c.strokeStyle = Color;
	c.beginPath();
	c.moveTo(xPadding - y1Padding, yPadding);
	c.lineTo(xPadding - y1Padding, graphheight - yPadding);
	c.stroke();
	c.textAlign = "right";
	c.textBaseline = "middle";
	c.beginPath();
	for (var i = 0; i < MaxY - YOffset; i += Diff) {
		if (i % Diff == 0) {
			c.fillText(i + Start4Freq, xPadding - y1Padding - 5, getYPixel(i, MaxY - YOffset));
			c.moveTo(xPadding - y1Padding + 5, getYPixel(i, MaxY - YOffset));
			c.lineTo(xPadding - y1Padding, getYPixel(i, MaxY - YOffset));
		}
	}
	c.stroke();
	c.strokeStyle = Color;
	c.beginPath();
	FrequencyStep = 1;
	c.moveTo(getXPixel(0, MaxX), getYPixel(0, MaxY - YOffset));
	c.moveTo(getXPixel(0, MaxX), getYPixel(Arr[StartTime] - Start4Freq, MaxY - YOffset));
	for (var i = 1, j = StartTime; i < DataSize; i += 1, j += 1) {
		c.lineTo(getXPixel(i, MaxX / FrequencyStep), getYPixel(Arr[j] - Start4Freq, MaxY - YOffset));
	}
	c.stroke();
}
function PlotPhase(c,Arr,MaxY,Color,YOffset,Start,MOff,YPad){
	y1Padding = y1Padding + YPad;
	var temp = MaxY / 5;
	MinY = -MaxY; 
	DiffPhase = MaxY / 10;
	MaxY = MaxY + (DiffPhase - (DiffPhase % 10) + 10);
	c.lineWidth = 1;
	c.strokeStyle = "Black";
	c.beginPath();
	c.moveTo(xPadding, yPadding);
	c.lineTo(xPadding, graphheight - yPadding); 
	c.moveTo(xPadding, graphheight / 2 + 3);
	c.lineTo(graphwidth - MOff, graphheight / 2 + 3); 
	c.stroke();
	c.textAlign = "right";
	c.textBaseline = "middle";
	c.strokeStyle = Color;
	if ((MinY + MaxY) < 0)
		MaxY = -MinY;
	c.beginPath();
	for (var i = 0; i < MaxY - 2; i += DiffPhase, MinY += temp) {
		if (i % DiffPhase == 0) {
			c.fillText(MinY, xPadding - 8, getYPixel(i, MaxY));
			c.moveTo(xPadding + 2, getYPixel(i, MaxY));
			c.lineTo(xPadding - 3, getYPixel(i, MaxY));
		}
	}
	c.stroke();
	c.strokeStyle = Color;
	c.beginPath();
	FrequencyStep = 1;
	c.moveTo(getXPixel(0, MaxX), getYPixel(0, MaxY) - 207);
	c.moveTo(getXPixel(0, MaxX), getYPixel(Phase[Start] / 2, MaxY) - YOffset);
	for (var i = 1, j = Start; i <= DataSize; i += 1, j += 1)
		c.lineTo(getXPixel(i, MaxX / FrequencyStep), getYPixel(Phase[j] / 2, MaxY) - YOffset);
	c.stroke();
}
function GetWeldData() {
    if (w)
        if (!w.closed) w.window.close();
    clearInterval(timerid);
    timerid = 0;
    UseID("Level2Lbl").innerHTML = GDInfo;
	var Commd = 0;
    EnableDiv();
    switch (datastate) {
        case 1: Commd = Command.REQ_TIME_DATA; break;
        case 2: Commd = Command.REQ_CURRENT_DATA; break;
        case 3: Commd = Command.REQ_PHASE_DATA; break;
        case 4: Commd = Command.REQ_AMPLITUDE_DATA; break;
        case 5: Commd = Command.REQ_POWER_DATA; break;
        case 6: Commd = Command.REQ_PWMAMPLITUDE_DATA; break;
        case 7: Commd = Command.REQ_FREQUENCY_DATA; break;
        case 8: Commd = Command.REQ_PRESET_DATA; break;
        default:
            DefaultSetting();
    }
	if (Commd != 0){
		var GetUri = "func=" + Func.ViewWeldGraphGetDatahandler + "cmd=" + Commd + "?param=" + SendCookies();
		http.open("POST", GetUri, true);
		http.onreadystatechange = UpdateWeldData;
		http.send(null);
	}
}
function UpdateWeldData() {
if ((http.readyState == 4) && (http.responseText.length != 0) && (CheckSession(http.responseText) == true)) {
        var i, j, results;
        results = http.responseText.split(",");
        if (results[0] == "Time")
            for (i = 0; i < results.length; i++)
                Time[i] = parseFloat(results[i]);
        else if (results[0] == "Phase")
            for (i = 0; i < results.length; i++)
                Phase[i] = ((parseFloat(results[i]) * 180) / MaxAdcRawVal).toFixed(3);
        else if (results[0] == "Current")
            for (i = 0; i < results.length; i++)
                Current[i] = ((parseFloat(results[i]) > 120) ? 120 : parseFloat(results[i]));
        else if (results[0] == "Amplitude")
            for (i = 0; i < results.length; i++)
                Amplitude[i] = ((parseFloat(results[i]) > 100) ? 100 : parseFloat(results[i]));
        else if (results[0] == "Power")
            for (i = 0; i < results.length; i++)
                Power[i] = ((parseFloat(results[i]) > 120) ? 120 : parseFloat(results[i]));
        else if (results[0] == "PWMAmplitude")
            for (i = 0; i < results.length; i++)
                PWMAmplitude[i] = ((parseFloat(results[i]) > 120) ? 120 : parseFloat(results[i]));
        else if (results[0] == "Frequency") {
            for (i = 0; i < results.length; i++){  
                if(parseFloat(results[i]) < StartFreq)              
                	Frequency[i] = StartFreq;
                else if(parseFloat(results[i]) > EndFreq)
                	Frequency[i] = EndFreq;
                else
                	Frequency[i] = parseFloat(results[i]);
            }
            if ((i - 1) % 10 == 0)
                DefaultDataSize = i;
            else
                DefaultDataSize = i - (i % 10) + 10;
            DataSize = DefaultDataSize;
            if (DataSize > 5000)
                DataSize = DefaultDataSize = 5000;
            DataLength = i - 1;
        }
        else if (results[0] == "PresetData") {
            StartFreq = parseInt(results[1]);
            EndFreq = parseInt(results[2]);
        }
        datastate = datastate + 1;
        GetWeldData();
    }
}
function DisplayWeldGraphValue() {
    if (w)
        if (!w.closed) w.window.close();
    var value = parseFloat(UseID('XValue').value);
    var i;
    if (UseID('SelectedGraph').selectedIndex != 0) {
        if (value > 0 && value <= DataLength) {
            switch (UseID('SelectedGraph').selectedIndex) {
                case 1: UseID('YValue').value = Phase[value];
                    break;
                case 2: UseID('YValue').value = Current[value];
                    break;
                case 3: UseID('YValue').value = Amplitude[value];
                    break;
                case 4: UseID('YValue').value = Power[value];
                    break;
                case 5: UseID('YValue').value = PWMAmplitude[value];
                    break;
                case 6: UseID('YValue').value = Frequency[value];
                    break;
            }
        }
        else
            alert("Enter correct Time");
    }
    else
        alert("Select Graph");
}
function RedrawWeldGraph() {
    if (w)
        if (!w.closed) w.window.close();
	var NoGraphSelected = false;
	for (var i = 0; i < UseName("CB").length; i++)
		if(UseName("CB").item(i).checked == true)
			NoGraphSelected = true;
	if(NoGraphSelected == false){
		alert("Please select graph data to display");
		return;
	}
    var StartTime = parseFloat(UseID("StartTime:tt").value);
    var EndTime = parseFloat(UseID("EndTime:tt").value);
    if (StartTime < 0)
        StartTime = 0;
    if (EndTime > 5000)
        EndTime = 5000;
    UseID("StartTime:tt").value = StartTime;
    UseID("EndTime:tt").value = EndTime;
    if ((EndTime - StartTime) >= 100) {
        if ((StartTime % 10) != 0)
            UseID("StartTime:tt").value = StartTime - (StartTime % 10);
        if ((EndTime % 10) != 0)
            UseID("EndTime:tt").value = EndTime + (10 - (EndTime % 10));
        c.clearRect(0, 0, 900, 500);
        DrawWeldGraph();
    }
    else
        alert("Minimum Time difference should be of 100ms");
}
function ExportWeldDataToExcel() {
    if (!Phase[1]) {
        alert("   No Graph Data   ");
        return;
    }
    if (w)
        if (!w.closed) return;
    clearInterval(timerid);
    timerid = 0;
    w = window.open('UserComment.html', '', sOption);
    var temp = "";
    var FileHeader = "\n\nTIME(ms),CURRENT(\%),AMPLITUDE(\%),POWER(%),PWM AMPLITUDE(\%),PHASE(Degree),FREQUENCY(Hz)\n";
    temp += FileHeader;
    for (j = 1; j < DataLength; j++) {
        if (!Frequency[j])
            break;
        temp += j + ',' + Current[j] + "," + Amplitude[j] + "," + Power[j] + "," + PWMAmplitude[j] + "," + Phase[j] + "," + Frequency[j] + "\n";
    }
    UseID("tempdata").value = temp;
}
function DrawWeldGraph() {
    UseID("Level2Lbl").innerHTML = BusyInfo;
    DisableDiv();
    graph = $('#graph');
    c = graph[0].getContext('2d');
    c.clearRect(0, 0, 900, 500);
    datastate = 1;
    xPadding = 292;
    yPadding = 40;
    y1Padding = -45;
    StartTime = parseFloat(UseID("StartTime:tt").value);
    EndTime = parseFloat(UseID("EndTime:tt").value);
    DataSize = EndTime - StartTime;
    MaxX = EndTime - StartTime; 
    if ((MaxX % 10) != 0)
        MaxX = MaxX - (MaxX % 10) + 10;
    DiffTime = MaxX / 10;
    MaxX += DiffTime;
    graphheight = graph.height();
    graphwidth = graph.width();
    if (DataLength < DataSize)
        DataSize = DataLength + 1;
    c.font = '8pt arial';
    c.textAlign = "center";
    c.stroke();
    c.strokeStyle = "Black";
    c.lineWidth = 1;
    c.beginPath();
    c.moveTo(0, 0);
    c.moveTo(xPadding, graphheight - yPadding);             
    c.lineTo(graphwidth - 50, graphheight - yPadding);
    c.moveTo(xPadding, yPadding);
    c.lineTo(graphwidth - 50, yPadding);          
    c.lineTo(graphwidth - 50, graphheight - yPadding); 
    c.stroke();
    c.fillStyle = "Black";
    c.beginPath();
    c.lineWidth = .5;
    for (var i = 0, j = StartTime; i < MaxX; i += DiffTime, j += DiffTime) {
        if (i % DiffTime == 0) {
            c.fillText(j, getXPixel(i, MaxX), graphheight - yPadding + 20);
            c.moveTo(getXPixel(i, MaxX), graphheight - yPadding - 2);
            c.lineTo(getXPixel(i, MaxX), graphheight - yPadding + 3);
        }
    }
    c.stroke();
    StartTime++;
    if (UseID("Phase:cb").checked == true) 
		PlotPhase(c,Phase,180,'Black',207,StartTime,50,45);
	if (UseID("Current:cb").checked == true) 
		PlotGraph(c,Current,120,'Blue',8,StartTime,0,45);
    if (UseID("Amplitude:cb").checked == true)
		PlotGraph(c,Amplitude,120,'Green',8,StartTime,0,45);
    if (UseID("Power:cb").checked == true)
		PlotGraph(c,Power,120,'Red',8,StartTime,0,45);
    if (UseID("PWMAmplitude:cb").checked == true) 
		PlotGraph(c,PWMAmplitude,120,'Gray',8,StartTime,0,45);
	if (UseID("Frequency:cb").checked == true)
		PlotGraph(c,Frequency,EndFreq - StartFreq,'Purple',10,StartTime,StartFreq,45);
	y1Padding = 305;
    c.textAlign = "left";
    c.font = 'Bold 10pt sans-serif';
    c.fillText("TIME (ms)", 500, 495);
    c.rotate(-90 * Math.PI / 180);
    if (UseID("Phase:cb").checked == true) {
        y1Padding = y1Padding - 45;
        c.fillStyle = "Black";
        c.fillText("PHASE (Deg)", -300, y1Padding);
    }
    if (UseID("Current:cb").checked == true) {
        y1Padding = y1Padding - 42;
        c.fillStyle = "Blue";
        c.fillText("CURRENT (%)", -300, y1Padding);
    }
    if (UseID("Amplitude:cb").checked == true) {
        y1Padding = y1Padding - 42;
        c.fillStyle = "Green";
        c.fillText("AMPLITUDE (%)", -300, y1Padding);
    }
    if (UseID("Power:cb").checked == true) {
        y1Padding = y1Padding - 45;
        c.fillStyle = "Red";
        c.fillText("POWER (%)", -300, y1Padding);
    }
    if (UseID("PWMAmplitude:cb").checked == true) {
        y1Padding = y1Padding - 45;
        c.fillStyle = "Gray";
        c.fillText("PWM AMPLITUDE (%)", -300, y1Padding);
    }
    if (UseID("Frequency:cb").checked == true) {
        y1Padding = y1Padding - 58;
        c.fillStyle = "Purple";
        c.fillText("FREQUENCY (Hz)", -300, y1Padding);
    }
    c.rotate(90 * Math.PI / 180);
    c.stroke();
    c.strokeStyle = "Black";
    c.lineWidth = .5;
    c.beginPath();
    c.moveTo(0, 0);
    c.moveTo(xPadding, yPadding);
    c.lineTo(xPadding + 1, yPadding + 1);
    c.stroke();
}
function DefaultSetting() {
    if (w)
        if (!w.closed) w.window.close();
    UseID("Phase:cb").checked = true;
    UseID("Current:cb").checked = true;
    UseID("Amplitude:cb").checked = true;
    UseID("Power:cb").checked = true;
    UseID("Frequency:cb").checked = true;
    UseID("StartTime:tt").value = 0;
    UseID("EndTime:tt").value = DefaultDataSize;
    UseID("XValue").value = 1;
    UseID("YValue").value = 0;
    UseID('SelectedGraph').selectedIndex = 0;
    DrawWeldGraph();
}

function is_empty(x)
{
    return (                                                           //don't put newline after return
        (typeof x == 'undefined')
              ||
        (x == null)
              ||
        (x == false)        //same as: !x
              ||
        (x.length == 0)
              ||
        (x == 0)            // note this line, you might not need this. 
              ||
        (x == "")
              ||
        (x.replace(/\s/g,"") == "")
              ||
        (!/[^\s]/.test(x))
              ||
        (/^\s*$/.test(x))
    );
}
