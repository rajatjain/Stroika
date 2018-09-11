<% @ LANGUAGE=VBScript CODEPAGE=65001 %> 
<Script Language="JScript" RunAt="Server">
	function	EncodeStringForHTMLAttribute (s)
		{
			// convert special HTML characters to entities
			var result = "";
			var sLength = s.length;
			for (i = 0; i < sLength; ++i) {
				switch (s.charAt(i)) {
					case '<': result += "&lt;"; break;
					case '>': result += "&gt;"; break;
					case '&': result += "&amp;"; break;
					case '\"': result += "&quot;"; break;
					case '\t': result += "&#09;"; break;
					default:  {
						var	ccode	=	s.charCodeAt (i);
						if ((ccode < 32 || ccode > 127) && (ccode != 10 && ccode != 13)) {
							result +=  "&#" + ccode.toString () + ";";
						}
						else {
							result += s.charAt(i);
						}
					}
				}
			}
			return result;
		}
</Script>

<%
	'''
	'''  Simple demo of transfering ActiveLedIt! contents to/from the IIS server.
	'''
	dim myvalue1
	dim myvalue2
	dim DQ
					  
	DQ = chr(34)

	REM Swap comments one and two
	myvalue1 = Request.Form ("mycomments2")
	if myvalue1 = "" Then
		myvalue1 = "{\rtf1 \ansi {\fonttbl {\f0 \fnil Arial;}}{\colortbl \red0\green0\blue0;\red255\green0\blue0;}\pard \plain \f0 \fs20 \cf0 Some rich text \plain \f0 \fs28 \b \cf1 pre-loaded\plain \f0 \fs20 \cf0 .}"
	End If
	myvalue2 = Request.Form ("mycomments1")
	if myvalue2 = "" Then
		myvalue2 = "{\rtf1 \ansi {\fonttbl {\f0 \fnil Arial;}}{\colortbl \red0\green0\blue0;}\pard \plain \f0 \fs20 \cf0 Some rich text pre-loaded (including a picture {\pict \pichgoal510 \picwgoal510 \dibitmap 2800000022000000220000000100080000000000c804000000000000000000000001000000010000ffffff0000ffff000000ff00cccccc00999999000099990066666600000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000070707070707070707070707070707070707070707070707070707070707070707070000070303030403040604060406040604060406040604060406040604060406070607070000070404040403040304030003000300030000000000000000000000000000000007070000070304030303040303030300030003000000030000000000000000000000000007070000070306040202040402020602020200020203040202030002000000020202000007070000070304030402030002060702040003020000030203000302000000020000000004070000070306040604020202040602070400020403000201030402020000020200000006070000070304060203020002060402070604020300030002010203000200020002000004070000070306040204060202000604020202020202000202020203040200000202000006070000070004060206040202000403040607060203030003020103030000000000000004070000070306040602020202000007040406040604040304030104040300000000000006070000070004060406040000000706070004060406040303000101030303000000000004070000070306040604060000000000060400040604060706030401010304030000000006070000070004040406040000000707070704000406040407060301010303000300000004070000070306040604060000000000000407030004060406040604010103030403000006070000070004060406040000000707070707060300040604060706050103030300000004070000070306040604060000000000000006070603000406040604060101040403000006070000070004060406040000000000000000060706040604060406070301060303000004070000070306040604060000070707000000040704060406040607070301010403000306070000070007060406040000000000000000000403040604060406040303010103030004070000070306070604060000070707070707000403040406040604060304010103040306070000070007040706040000000000000000000000030304060406040303030101030004070000070306070607060000000000000000000000000304040604060403030407000306070000070007060706070000000000000000000007040304030707040304030303030004070000070307070707060000000000000000000000000304040604060404040403040306070000070007060706070000000000000000000007000707070707040303030303030304070000070306070607060000000000000000000000000000030404060404040404040306070000070007060706070000000000000000070007070007070707040604030403040304070000070307070607060000000000000000000000000000030403040406040404040406070000070007040706070000000000000000000000000000000000030604040403040304070000070306070607060706070607060706040604060406040604060406040604040404070000070007060706070607060706070607060406040604060406040604060406040303070000070300030003000300030003000300030003000300030403040304030403040404070000070707070707070707070707070707070707070707070707070707070707070707070000}).}"
	End If
	
	REM
	myvalue1 = EncodeStringForHTMLAttribute (myvalue1)
	myvalue2 = EncodeStringForHTMLAttribute (myvalue2)
%>
<HTML>
<HEAD>
	<TITLE>ActiveLedIt! IIS/ASP server demo (VBScript)</TITLE>
</HEAD>
<Body>
	<Table Border=0 Width=100% cols=2>
		<td valign=top>

			<Center><h1>ActiveLedIt! IIS/ASP server demo (VBScript)</h1></Center>

			<p>
				This page demonstrates how to use ASP pages with ActiveLedIt! to transfer content from a web server,
				to a client web page (running ActiveLedIt!),
				and then to transfer the edited text back to the web server (for possibly processing or saving into a database).
			<p>
				This sample merely swaps the text between fields one and two. But that swapping is done server side -
				 and so clearly you could have done any other
				logic you wanted server side with the RTF text in visual basic variables on the server.

			<Center>
			<FORM action="SimpleActiveLedItASP2FromServerDemo.asp" id="FORM2" method="post">
			<Table Border=2>
			<TR><TD>
			<OBJECT
				ID="ActiveLedIt1" Name="ActiveLedIt1"
				WIDTH=500 HEIGHT=200
				CLASSID="CLSID:9A013A76-AD34-11D0-8D9B-00A0C9080C73"
				CODEBASE="USE_CODEBASE_VAR"
			>
				<param Name="TextRTF" Value= "<%= myvalue1 %> ">
			</Object>
			</table>

			<p>
			<Table Border=2>
			<tr><td>
			<OBJECT
				ID="ActiveLedIt2" Name="ActiveLedIt2"
				WIDTH=500 HEIGHT=200
				CLASSID="CLSID:9A013A76-AD34-11D0-8D9B-00A0C9080C73"
				CODEBASE="USE_CODEBASE_VAR"
			>
				<param Name="TextRTF" Value= "<%= myvalue2 %>" >

			</Object>

			</table>


			<Input name=mycomments1 type=hidden>
			<Input name=mycomments2 type=hidden>
			 
			<p><INPUT name=Update_button type=submit value="Test submit of this client-side text back to the server" language="VBScript" 
			    onclick="mycomments1.Value = ActiveLedit1.TextRTF : mycomments2.Value = ActiveLedit2.TextRTF"> 


			</form>
			</Center>
		</td>
	</Table>
</BODY>
</HTML>