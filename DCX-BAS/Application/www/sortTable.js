///////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Generic Table sort. 
//
// Basic Concept: 
//                 A table can be sorted by clicking on the title of any column in the table, toggling
//                 between ascending and descending sorts.             
//
// Assumptions: 
//              The first row of the table contains column titles that are "clicked" to sort the table
//
function sortTable(td_element,ignoreLastLines) {
	ignoreLastLines = (typeof(ignoreLastLines)=='undefined') ? 0 : ignoreLastLines;
	var sortImages =['desc.gif','asc.gif','none.gif','sorting.gif']; 
	var sortColImage = td_element.getElementsByTagName('img')[0];
	var sortAscending = false;
	var newSortColImage = "";
	if (sortColImage.getAttribute('src').indexOf(sortImages[1])>-1 || 
		sortColImage.getAttribute('src').indexOf(sortImages[2])>-1) {
		newSortColImage = sortImages[0];
		sortAscending = false;
	} 
	else {
		newSortColImage = sortImages[1];
		sortAscending = true;
	}
	sortColImage.setAttribute('src',sortImages[3]);
	var indexCol = td_element.cellIndex;
	var table_element = td_element.parentNode;
	while (table_element.nodeName != "TABLE") {
		table_element = table_element.parentNode;
	}
	var tr_elements = table_element.getElementsByTagName('tr');
	var allImg = tr_elements[0].getElementsByTagName('img');
	for(var i=0;i<allImg.length;i++){
		if(allImg[i]!=sortColImage){allImg[i].setAttribute('src',sortImages[2])}
	}
	var clonedRows = new Array() 
	var originalCol = new Array();
	for (var i=1; i<tr_elements.length - ignoreLastLines; i++) {
		clonedRows[i]=tr_elements[i].cloneNode(true);
		var valueCol = getTextValue(tr_elements[i].cells[indexCol]);
		valueCol = FormatForType(valueCol);
		originalCol[i]=[valueCol,tr_elements[i].rowIndex];
	}
	originalCol.shift();
	sortCol = originalCol.sort(sortCompare);
	if (sortAscending) { sortCol.reverse(); }
	for (var i=1; i < tr_elements.length - ignoreLastLines; i++) { 
		var old_row = sortCol[i-1][1];
		var new_row = i;
		tr_elements[i].parentNode.replaceChild(clonedRows[old_row],tr_elements[new_row]);
	}
	makePretty(table_element,indexCol,ignoreLastLines);
	sortColImage.setAttribute('src',newSortColImage);
}
function sortCompare (currValue, nextValue) {
	if ( currValue[0] == nextValue[0] ) return 0;
	if ( currValue[0] < nextValue[0] ) return -1;
	if ( currValue[0] > nextValue[0] ) return 1;
}
if (document.ELEMENT_NODE == null) {
	document.ELEMENT_NODE = 1;
	document.TEXT_NODE = 3;
}
function getTextValue(el) {
	var i;
	var s;
	s = "";
	for (i = 0; i < el.childNodes.length; i++)
	if (el.childNodes[i].nodeType == document.TEXT_NODE)
		s += el.childNodes[i].nodeValue;
	else if (el.childNodes[i].nodeType == document.ELEMENT_NODE &&
		el.childNodes[i].tagName == "BR")
		s += " ";
	else
		s += getTextValue(el.childNodes[i]);
	return normalizeString(s);
}
var whtSpEnds = new RegExp("^\\s*|\\s*$", "g");
var whtSpMult = new RegExp("\\s\\s+", "g");
function normalizeString(s) {
	s = s.replace(whtSpEnds, ""); 
	return s;
}
function FormatForType(itm) {
	var sortValue = itm.toLowerCase();
	if (itm.match(/^\d\d[\/-]\d\d[\/-]\d\d\d\d$/) || 
	itm.match(/^\d\d[\/-]\d\d[\/-]\d\d$/) ||
	itm.match(/^\d[\/-]\d\d[\/-]\d\d\d\d$/) ) {
		var yr = -1;
		if (itm.length == 10) {
			sortValue = itm.substr(6,4)+itm.substr(0,2)+itm.substr(3,2);
		} 
		else if (itm.length == 9) {
				sortValue = itm.substr(5,4)+"0" + itm.substr(0,1)+itm.substr(2,2);
		} 
		else {
			yr = itm.substr(6,2);
			if (parseInt(yr) < 50)  
				yr = '20'+yr; 
			else  
				yr = '19'+yr; 
			sortValue = yr+itm.substr(3,2)+itm.substr(0,2);
		}
	}
	if (itm.match(/%/)) {
		sortValue = itm.replace(/[^0-9.-]/g,'');
		sortValue = parseFloat(sortValue);
	}
	if (itm.substr(0,1) == "(" & itm.substr(itm.length - 1,1) == ")")
		itm = "-" + itm.substr(1,itm.length - 2);
	if (itm.match(/^[£$]|(^-)/)) {
		sortValue = itm.replace(/[^0-9.-]/g,'');
		if (isNaN(sortValue)) 
			sortValue = 0;
		else 
			sortValue = parseFloat(sortValue);
	}
	if (itm.match(/(\d*,\d*$)|(^-?\d\d*\.\d*$)|(^-?\d\d*$)|(^-?\.\d\d*$)/)) {
		sortValue = itm.replace(/[^0-9.-]/g,'');
		if (isNaN(sortValue))
			sortValue = 0;
		else 
			sortValue = parseFloat(sortValue);

	}
	return sortValue;
}
var rowClsNm = "alternateRow";
var colClsNm = "sortedColumn";
var rowTest = new RegExp(rowClsNm, "gi");
var colTest = new RegExp(colClsNm, "gi");
function makePretty(tblEl, col, ignoreLastLines){
	var i, j;
	var rowEl, cellEl;
	for (i = 1; i < tblEl.rows.length - ignoreLastLines; i++) {
		rowEl = tblEl.rows[i];
		rowEl.className = rowEl.className.replace(rowTest, "");
		if (i % 2 != 0)
		  rowEl.className += " " + rowClsNm;
		rowEl.className = normalizeString(rowEl.className);
		for (j = 0; j < tblEl.rows[i].cells.length; j++) {
			cellEl = rowEl.cells[j];
			cellEl.className = cellEl.className.replace(colTest, "");
			if (j == col)
				cellEl.className += " " + colClsNm;
			cellEl.className = normalizeString(cellEl.className);
		}
	}
}
