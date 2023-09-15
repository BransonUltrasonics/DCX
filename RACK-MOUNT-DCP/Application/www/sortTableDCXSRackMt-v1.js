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
//              The images 'desc.gif','asc.gif','none.gif','sorting.gif' exist
//
//              The img tag is in each column of the the title row to represent the sort graphic.
//
//              The CSS classes 'alternateRow' and 'sortedColumn' exist so we can have alternating 
//              colors for each row and a highlight the sorted column.  Something like the <style>
//              definition below, but with the background colors set to whatever you want.
//
//              <style>
//              tr.alternateRow {
//                background-color: #E0F1E0;
//              }
//
//              td.sortedColumn {
//                background-color: #E0F1E0;
//              }
//
//              tr.alternateRow td.sortedColumn {
//                background-color: #c5e5c5;
//              }
//              </style>
//

function sortTable(td_element,ignoreLastLines) {

 // If the optional ignoreLastLines parameter (number of lines *not* to sort at end of table) 
 // was not passed then make it 0
 ignoreLastLines = (typeof(ignoreLastLines)=='undefined') ? 0 : ignoreLastLines;

 var sortImages =['desc.gif','asc.gif','none.gif','sorting.gif']; 

 // Get the image used in the first row of the current column
 var sortColImage = td_element.getElementsByTagName('img')[0];

 // If current image is 'asc.gif' or 'none.gif' (elements 1 and 2 of sortImages array) then this will
 // be a descending sort else it will be ascending - get new sort image icon and set sort order flag
 var sortAscending = false;
 var newSortColImage = "";
 if (sortColImage.getAttribute('src').indexOf(sortImages[1])>-1 || 
     sortColImage.getAttribute('src').indexOf(sortImages[2])>-1) {
  newSortColImage = sortImages[0];
  sortAscending = false;
 } else {
  newSortColImage = sortImages[1];
  sortAscending = true;
 }

 // Assign "SORTING" image icon (element 3 of sortImages array)) to current column title 
 // (will replace with newSortColImage when sort completes)
 sortColImage.setAttribute('src',sortImages[3]);

 // Find which column was clicked by getting it's column position
 var indexCol = td_element.cellIndex;

 // Get the table element from the td element that was passed as a parameter to this function
 var table_element = td_element.parentNode;
 while (table_element.nodeName != "TABLE") {
  table_element = table_element.parentNode;
 }

 // Get all "tr" elements from the table and assign then to the Array "tr_elements"
 var tr_elements = table_element.getElementsByTagName('tr');

 // Get all the images used in the first row then set them to 'none.gif'
 // (element 2 or sortImages array) except for the current column (all ready been changed)
 var allImg = tr_elements[0].getElementsByTagName('img');
  for(var i=0;i<allImg.length;i++){
  if(allImg[i]!=sortColImage){allImg[i].setAttribute('src',sortImages[2])}
 }
  
 // Some explantion of the basic concept of the following code before we
 // actually start.  Essentially we are going to copy the current columns information
 // into an array to be sorted. We'll sort the column array then go back and use the information 
 // we saved about the original row positions to re-order the entire table.
 // We are never really sorting more than a columns worth of data, which should keep the sorting fast.
 
 // Create a new array for holding row information
 var clonedRows = new Array() 

 // Create a new array to store just the selected column values, not the whole row
 var originalCol = new Array();
 
 // Now loop through all the data row elements
 // NOTE: Starting at row 1 because row 0 contains the column titles
 for (var i=1; i<tr_elements.length - ignoreLastLines; i++) {

  // "Clone" the tr element i.e. save a copy all of its attributes and values
  clonedRows[i]=tr_elements[i].cloneNode(true);

  // Text value of the selected column on this row
  var valueCol = getTextValue(tr_elements[i].cells[indexCol]);

  // Format text value for sorting depending on its type, ie Date, Currency, number, etc..
  valueCol = FormatForType(valueCol);

  // Assign the column value AND the row number it was originally on in the table
  originalCol[i]=[valueCol,tr_elements[i].rowIndex];
 }

 // Get rid of element "0" from this array.  A value was never assigned to it because the first row
 // in the table just contained the column titles, which we did not bother to assign.
 originalCol.shift();

 // Sort the column array returning the value of a sort into a new array
 sortCol = originalCol.sort(sortCompare);

 // If it was supposed to be an Ascending sort then reverse the order 
 if (sortAscending) { sortCol.reverse(); }

 // Now take the values from the sorted column array and use that information to re-arrange
 // the order of the tr_elements in the table
 for (var i=1; i < tr_elements.length - ignoreLastLines; i++) { 

  var old_row = sortCol[i-1][1];
  var new_row = i;
  tr_elements[i].parentNode.replaceChild(clonedRows[old_row],tr_elements[new_row]);
 }

  // Format the table, making the rows alternating colors and highlight the sorted column
  makePretty(table_element,indexCol,ignoreLastLines);
  
 // Assign correct sort image icon to current column title
 sortColImage.setAttribute('src',newSortColImage);
}

// Function used by the sort routine to compare the current value in the array with the next one
function sortCompare (currValue, nextValue) {
 // Since the elements of this array are actually arrays themselves, just sort
 // on the first element which contiains the value, not the second which contains
 // the original row position
 if ( currValue[0] == nextValue[0] ) return 0;
 if ( currValue[0] < nextValue[0] ) return -1;
 if ( currValue[0] > nextValue[0] ) return 1;
}

//-----------------------------------------------------------------------------
// Functions to get and compare values during a sort.
//-----------------------------------------------------------------------------

// This code is necessary for browsers that don't reflect the DOM constants
// (like IE).
if (document.ELEMENT_NODE == null) {
  document.ELEMENT_NODE = 1;
  document.TEXT_NODE = 3;
}

function getTextValue(el) {

  var i;
  var s;

  // Find and concatenate the values of all text nodes contained within the
  // element.
  s = "";
  for (i = 0; i < el.childNodes.length; i++)
    if (el.childNodes[i].nodeType == document.TEXT_NODE)
      s += el.childNodes[i].nodeValue;
    else if (el.childNodes[i].nodeType == document.ELEMENT_NODE &&
             el.childNodes[i].tagName == "BR")
      s += " ";
    else
      // Use recursion to get text within sub-elements.
      s += getTextValue(el.childNodes[i]);

  return normalizeString(s);
}

// Regular expressions for normalizing white space.
var whtSpEnds = new RegExp("^\\s*|\\s*$", "g");
var whtSpMult = new RegExp("\\s\\s+", "g");

function normalizeString(s) {

  s = s.replace(whtSpMult, " ");  // Collapse any multiple whites space.
  s = s.replace(whtSpEnds, "");   // Remove leading or trailing white space.

  return s;
}

// Function used to modify values to make then sortable depending on the type of information
function FormatForType(itm) {

  var sortValue = itm.toLowerCase();

  // If the item matches a date pattern (MM/DD/YYYY or MM/DD/YY or M/DD/YYYY)
  if (itm.match(/^\d\d[\/-]\d\d[\/-]\d\d\d\d$/) || 
      itm.match(/^\d\d[\/-]\d\d[\/-]\d\d$/) ||
	  itm.match(/^\d[\/-]\d\d[\/-]\d\d\d\d$/) ) {

    // Convert date to YYYYMMDD format for sort comparison purposes
    // y2k notes: two digit years less than 50 are treated as 20XX, greater than 50 are treated as 19XX
    var yr = -1;

    if (itm.length == 10) {
      sortValue = itm.substr(6,4)+itm.substr(0,2)+itm.substr(3,2);
	} else if (itm.length == 9) {
      sortValue = itm.substr(5,4)+"0" + itm.substr(0,1)+itm.substr(2,2);
	} else {
      yr = itm.substr(6,2);
      if (parseInt(yr) < 50) { 
        yr = '20'+yr; 
      } else { 
        yr = '19'+yr; 
      }
        sortValue = yr+itm.substr(3,2)+itm.substr(0,2);
    }

  }


  
  // If the item matches a Percent patten (contains a percent sign)
  if (itm.match(/%/)) {
   // Replace anything that is not part of a number (decimal pt, neg sign, or 0 through 9) with an empty string.  
   sortValue = itm.replace(/[^0-9.-]/g,'');
   sortValue = parseFloat(sortValue);
  }

  // If item starts with a "(" and ends with a ")" then remove them and put a negative sign in front
  if (itm.substr(0,1) == "(" & itm.substr(itm.length - 1,1) == ")") {
   itm = "-" + itm.substr(1,itm.length - 2);
  }

// If the item matches a currency pattern (starts with a dollar or negative dollar sign)
  if (itm.match(/^[£$]|(^-)/)) {
   // Replace anything that is not part of a number (decimal pt, neg sign, or 0 through 9) with an empty string.  
   sortValue = itm.replace(/[^0-9.-]/g,'');
   if (isNaN(sortValue)) { 
     sortValue = 0;
   } else {
     sortValue = parseFloat(sortValue);
   }
}

  // If the item matches a numeric pattern 
  if (itm.match(/(\d*,\d*$)|(^-?\d\d*\.\d*$)|(^-?\d\d*$)|(^-?\.\d\d*$)/)) {
   // Replace anything that is not part of a number (decimal pt, neg sign, or 0 through 9) with an empty string.  
   sortValue = itm.replace(/[^0-9.-]/g,'');
 //  sortValue = sortValue.replace(/,/g,'');
   if (isNaN(sortValue)) { 
     sortValue = 0;
   } else {
     sortValue = parseFloat(sortValue);
   }
  }

  return sortValue;
}


//-----------------------------------------------------------------------------
// Functions to update the table appearance after a sort.
//-----------------------------------------------------------------------------

// Style class names.
var rowClsNm = "alternateRow";
var colClsNm = "sortedColumn";

// Regular expressions for setting class names.
var rowTest = new RegExp(rowClsNm, "gi");
var colTest = new RegExp(colClsNm, "gi");

function makePretty(tblEl, col, ignoreLastLines) {

  var i, j;
  var rowEl, cellEl;

  // Set style classes on each row to alternate their appearance.
  for (i = 1; i < tblEl.rows.length - ignoreLastLines; i++) {
   rowEl = tblEl.rows[i];
   rowEl.className = rowEl.className.replace(rowTest, "");
    if (i % 2 != 0)
      rowEl.className += " " + rowClsNm;
    rowEl.className = normalizeString(rowEl.className);
    // Set style classes on each column (other than the name column) to
    // highlight the one that was sorted.
    for (j = 0; j < tblEl.rows[i].cells.length; j++) {
      cellEl = rowEl.cells[j];
      cellEl.className = cellEl.className.replace(colTest, "");
      if (j == col)
        cellEl.className += " " + colClsNm;
      cellEl.className = normalizeString(cellEl.className);
    }
  }


}
//
// END Generic Table sort. 
///////////////////////////////////////////////////////////////////////////////////////////////////////