<?xml version="1.0"?>

<!--
proj.xsl - XSLT style-sheet for generating a project web page
Copyright (C) 2002 Michael B. Allen

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
-->

<xsl:stylesheet version="1.0"
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
	xmlns:xt="http://www.jclark.com/xt"
	extension-element-prefixes="xt">
<xsl:output method="html"
	encoding="ISO-8859-1"
	doctype-public="-//W3C//DTD HTML 4.01 Transitional//EN"/>
<xsl:param name="section"/>
<xsl:param name="date"/>
<xsl:param name="lib"/>
<xsl:param name="title"/>
<xsl:param name="copyright"/>

<xsl:param name="edge"/>
<xsl:param name="mainpane"/>
<xsl:param name="leftpane"/>
<xsl:param name="middlepane"/>
<xsl:param name="footer"/>
<xsl:param name="text1"/>
<xsl:param name="text2"/>

<xsl:template match="proj">
	<HTML><HEAD>
	<STYLE TYPE="text/css">
		BODY {
			background-color: <xsl:value-of select="$edge"/>;
		}
		H1 {
			font-family: verdana, arial;
			font-size: normal;
			color: <xsl:value-of select="$text1"/>;
		}
		H2 {
			font-family: verdana, arial;
			font-size: normal;
			color: <xsl:value-of select="$text1"/>;
		}
		H3 {
			font-family: verdana, arial;
			font-size: small;
			color: <xsl:value-of select="$text2"/>;
		}
		A {
			color: <xsl:value-of select="$text2"/>;
			font-weight: bold;
		}
		BIG {
			color: <xsl:value-of select="$text2"/>;
			font-weight: bold;
			font-size: 50px;
		}
		EM {
			color: <xsl:value-of select="$text2"/>;
			font-family: Times New Roman;
			font-weight: bold;
			font-size: 20px;
		}
		PRE { 
		    font-family: monospaced, courier;
		    white-space: pre; 
		    color: black;
		    background-color: #e0e0e0; 
		}
		TABLE {
			border-collapse: collapse;
		}
		TH, TD {
			font-family: verdana, arial;
			font-size: small;
		}
		.mainpane {
			background-color: <xsl:value-of select="$mainpane"/>;
			padding: 10px;
		}
		.leftpane {
			background-color: <xsl:value-of select="$leftpane"/>;
			padding: 10px;
		}
		.middlepane {
			background-color: <xsl:value-of select="$middlepane"/>;
			padding: 10px;
		}
		.footer {
			background-color: <xsl:value-of select="$footer"/>;
		}
	</STYLE>
	<TITLE><xsl:value-of select="title"/></TITLE>
	</HEAD><BODY>
	<TABLE>
	<TR><TD colspan="2" class="mainpane">
		<BIG><xsl:value-of select="title"/></BIG><BR/>
		<b><xsl:value-of select="short"/></b>
		<P/>
			<xsl:apply-templates select="desc"/>
		<P/>
	</TD></TR><TR><TD valign="top" width="250" class="leftpane">
		<xsl:apply-templates select="links"/>
	</TD><TD valign="top" class="middlepane">
		<xsl:apply-templates select="news"/>
	</TD></TR><TR><TD colspan="2" class="footer">
		<hr noshade="noshade"/>
		<small>
		Last updated <xsl:value-of select="$date"/><BR/>
		<xsl:value-of select="$lib"/><BR/>
		API documentation and website generated with <a href="../cstylex" style="color: black;">CSytleX</a><BR/>
		Copyright &#169; 2004 <xsl:value-of select="$copyright"/><BR/>
		<a href="http://validator.w3.org/check/referer" style="color: black;">validate this page</a>
		</small>
	</TD></TR></TABLE>
	</BODY></HTML>
</xsl:template>

<xsl:template match="links">
	<h1>Links</h1>
	<xsl:for-each select="a">
		<xsl:apply-templates select="."/><br/>
	</xsl:for-each>
	<xsl:for-each select="group">
		<br/>
		<h2><xsl:value-of select="title"/></h2>
		<xsl:apply-templates select="desc"/>
		<xsl:for-each select="a">
			<xsl:apply-templates select="."/><br/>
		</xsl:for-each>
	</xsl:for-each>
</xsl:template>

<xsl:template match="news">
	<h1>News</h1>
	<xsl:for-each select="entry[not(@old)]">
		<EM><xsl:apply-templates select="title"/></EM><BR/>
		<SMALL><xsl:value-of select="sub"/></SMALL><BR/>
		<xsl:apply-templates select="desc"/><BR/>
	</xsl:for-each>
	<P/>
</xsl:template>

<xsl:template match="br|p|a|i|tt|pre|blockquote|ul|li|small|img">
	<xsl:copy-of select="."/>
</xsl:template>

</xsl:stylesheet>

