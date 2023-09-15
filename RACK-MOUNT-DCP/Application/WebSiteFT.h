/* $Header:   D:/databases/VMdb/archives/DCX/RackMount/DCP/Application/WebSiteFT.h_v   1.1   07 Jun 2016 12:17:28   GButron  $*/
/*****************************************************************************
$Log:   D:/databases/VMdb/archives/DCX/RackMount/DCP/Application/WebSiteFT.h_v  $
 * 
 *    Rev 1.1   07 Jun 2016 12:17:28   GButron
 * Old revisions removed
 * 
 *    Rev 1.0   06 Jun 2016 16:43:02   GButron
 * Initial revision.
*/

#ifndef WEBSITEFT_H_
#define WEBSITEFT_H_

#include "portable.h"
#include "HttpServer.h"
#include "CyGOS.h"
#include "CommandsFT.h"

#define WEBSITE_FT_PORT                                  81
#define MAXFILELEN                                       45

class WebSiteFT {
public:
   static BOOLEAN WebPostHandler(STR *PostPtr,STR *UrlPtr, HTTPReplyHeader &Header);
   static BOOLEAN WebGetHandler(STR *UrlPtr, HTTPReplyHeader &Header);
   static void InitWebSite(UINT8 Interface,UINT8 Port);
private:
   static const STR *HtmlTerminalPtr;
   static const STR *JsTerminalPtr;
};
#endif /* WEBSITEFT_H_ */
