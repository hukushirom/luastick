<?xml version="1.0"?>

<!--
ref.xsl - XSLT style-sheet for generating a C Library HTML reference
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
<xsl:param name="color"/>

<xsl:preserve-space elements="*"/>

<xsl:template name="string-replace" >
	<xsl:param name="string"/>
	<xsl:param name="from"/>
	<xsl:param name="to"/>
	<xsl:choose>
		<xsl:when test="contains($string,$from)">
			<xsl:value-of select="substring-before($string,$from)"/>
			<xsl:copy-of select="$to"/>
			<xsl:call-template name="string-replace">
				<xsl:with-param name="string" select="substring-after($string,$from)"/>
				<xsl:with-param name="from" select="$from"/>
				<xsl:with-param name="to" select="$to"/>
			</xsl:call-template>
		</xsl:when>
		<xsl:otherwise>
			<xsl:value-of select="$string"/>
		</xsl:otherwise>
	</xsl:choose>
</xsl:template>

<xsl:template match="library">
		<xsl:apply-templates select="group"/>
</xsl:template>

<xsl:template match="library">

	<HTML><HEAD>
	<TITLE><xsl:value-of select="title"/></TITLE>
	</HEAD>
	<FRAMESET COLS="20%,80%">
		<FRAME SRC="toc.html" NAME="left"/>

<xt:document method="html" href="toc.html">
	<HTML><HEAD>
	<STYLE TYPE="text/css">
		BODY {
			background-color: f5f5f5;
			font-family: arial, verdana; font-size: small;
		}
		H2, H3, .tfc {
			font-family: arial, verdana; font-size: small;
			color: <xsl:value-of select="$color"/>;
		}
		A {
			color: <xsl:value-of select="$color"/>;
			font-weight: bold;
		}
		PRE { 
		    font-family: monospaced, courier;
			border: 1px lightgrey dotted;
		    white-space: pre; 
		    color: black;
		    background-color: #efefef; 
		}
		TABLE {
			float: right;
			border-collapse: collapse;
			border-top: 1px solid #000000;
			border-right: 1px solid #000000;
		}
		TH, TD {
			padding-top: 2px;
			padding-bottom: 2px;
			padding-right: 5px;
			padding-left: 5px;
			border-bottom: 1px solid #000000;
			border-left: 1px solid #000000;
		}
	</STYLE>
	<TITLE><xsl:value-of select="title"/></TITLE>
	</HEAD><BODY>
	<H2>Index</H2>

	<A HREF="overview.html" TARGET="right">Overview</A><BR/>
	<xsl:for-each select="interface">
		<A>
			<xsl:attribute name="href">
				<xsl:value-of select="concat(@name,'.html')"/>
			</xsl:attribute>
			<xsl:attribute name="target">
				<xsl:value-of select="'right'"/>
			</xsl:attribute>
			<xsl:value-of select="title"/>
		</A><BR/>
		<SMALL>
			<xsl:for-each select="group/code">
&#160;&#160;&#160;&#160;<A>
					<xsl:attribute name="href">
						<xsl:value-of select="concat(ancestor::interface/@name,'.html','#',@name)"/>
					</xsl:attribute>
					<xsl:attribute name="target">right</xsl:attribute>
					<xsl:choose>
						<xsl:when test="string-length(title)&gt;21">
							<xsl:value-of select="substring(title,1,21)"/>
							<xsl:text>...</xsl:text>
						</xsl:when>
						<xsl:otherwise>
							<xsl:value-of select="title"/>
						</xsl:otherwise>
					</xsl:choose>
				</A><BR/>
			</xsl:for-each>
			<xsl:for-each select="group/meth|group/func">
&#160;&#160;&#160;&#160;<A>
					<xsl:attribute name="href">
						<xsl:value-of select="concat(ancestor::interface/@name,'.html','#',@name)"/>
					</xsl:attribute>
					<xsl:attribute name="target">right</xsl:attribute>
					<xsl:choose>
						<xsl:when test="local-name(.)='func'">
							<xsl:value-of select="@name"/>
						</xsl:when>
						<xsl:otherwise>
<!--
							<xsl:value-of select="concat(ancestor::interface/@name,'_',@name)"/>
-->
							<xsl:value-of select="@name"/>
						</xsl:otherwise>
					</xsl:choose>
				</A><BR/>
			</xsl:for-each>
		</SMALL>
	</xsl:for-each>

	</BODY></HTML>
</xt:document>

		<FRAME SRC="overview.html" NAME="right"/>

<xt:document method="html" href="overview.html">
	<html>
	<head>
	<STYLE TYPE="text/css">
		BODY {
			background-color: f5f5f5;
			font-family: arial, verdana; font-size: small;
		}
		H2, H3, A, .tfc {
			color: <xsl:value-of select="$color"/>;
			font-family: arial, verdana; font-size: small;
		}
		PRE { 
		    font-family: monospace;
			border: 1px lightgrey dotted;
		    white-space: pre; 
		    color: black;
		    background-color: #efefef; 
		}
		TABLE {
			float: right;
			border-collapse: collapse;
			border-top: 1px solid #000000;
			border-right: 1px solid #000000;
		}
		TH, TD {
			padding-top: 2px;
			padding-bottom: 2px;
			padding-right: 5px;
			padding-left: 5px;
			border-bottom: 1px solid #000000;
			border-left: 1px solid #000000;
		}
	</STYLE>
	<title><xsl:value-of select="title"/></title>
	</head>
	<body bgcolor="#f5f5f5">
	<h1><xsl:value-of select="title"/></h1>
	<xsl:apply-templates select="desc"/>
	<xsl:for-each select="interface">
		<H2><xsl:apply-templates select="title"/></H2>
		<xsl:apply-templates select="desc" mode="min"/>
	</xsl:for-each>
	<hr noshade="noshade"/>
	<small><xsl:value-of select="comments"/></small>
	</body>
	</html>
</xt:document>

	</FRAMESET>
	</HTML>

	<xsl:apply-templates select="interface"/>
	<xsl:apply-templates select="subtopic"/>

</xsl:template>

<xsl:template match="desc/example" mode="min"/>
<xsl:template match="desc/table|ret/table" mode="min"/>
<xsl:template match="title/def|desc/def|th/def|td/def" mode="min">
	<xsl:apply-templates select="."/>
</xsl:template>
<xsl:template match="title/ident|desc/ident|th/ident|td/ident" mode="min">
	<xsl:apply-templates select="."/>
</xsl:template>
<xsl:template match="desc/pre|ret/pre" mode="min">
	<xsl:apply-templates select="."/>
</xsl:template>
<xsl:template match="tt|p|i|b" mode="min">
	<xsl:apply-templates select="."/>
</xsl:template>


<xsl:template match="interface|subtopic">
	<xsl:variable name="file" select="concat(@name,'.html')"/>
<xt:document method="html" href="{$file}">
	<HTML><HEAD>
	<STYLE TYPE="text/css">
		BODY {
			background-color: f5f5f5;
			font-family: arial, verdana; font-size: small;
		}
		H2, H3, A, .tfc {
			color: <xsl:value-of select="$color"/>;
			font-family: arial, verdana; font-size: small;
		}
		PRE { 
		    font-family: monospace;
			border: 1px lightgrey dotted;
		    white-space: pre; 
		    color: black;
		    background-color: #efefef; 
		}
		TABLE {
			float: right;
			border-collapse: collapse;
			border-top: 1px solid #000000;
			border-right: 1px solid #000000;
			border-left: 1px solid #000000;
		}
		TH {
			padding-top: 2px;
			padding-bottom: 2px;
			padding-right: 5px;
			padding-left: 5px;
		}
		TD {
			padding-top: 2px;
			padding-bottom: 2px;
			padding-right: 5px;
			padding-left: 5px;
			border-bottom: 1px solid #000000;
			border-right: 1px solid #000000;
			font-family: arial, verdana; font-size: small;
		}
	</STYLE>
	<TITLE><xsl:value-of select="title"/></TITLE>
	</HEAD><BODY>

	<H1>
		<xsl:if test="local-name(.)='interface'">
			<xsl:number level="single" count="interface" format="1. "/>
		</xsl:if>
		<xsl:value-of select="title"/>
	</H1>

	<xsl:apply-templates select="desc"/>
	<xsl:apply-templates select="group"/>

	<HR NOSHADE="noshade"/>
	<small><xsl:value-of select="comments"/></small>
	</BODY></HTML>
</xt:document>
</xsl:template>

<xsl:template match="group">
	<H3>
		<xsl:if test="local-name(..)='interface'">
			<xsl:number level="multiple" count="interface|group" format="1.1. "/>
		</xsl:if>
		<xsl:value-of select="title"/>
	</H3>
	<xsl:apply-templates select="desc"/>
	<xsl:apply-templates select="code|func|meth"/>
</xsl:template>

<xsl:template match="code">
	<A><xsl:attribute name="name"><xsl:value-of select="title"/></xsl:attribute></A>
	<P>
	<xsl:if test="title">
		<B CLASS="tfc"><xsl:apply-templates select="title"/></B><BR/>
	</xsl:if>
	<xsl:if test="pre">
		<B>Synopsis</B>
		<PRE><BR/>
			<xsl:for-each select="ancestor::interface/include">  #include &lt;<xsl:value-of select="."/>&gt;
	</xsl:for-each>
			<xsl:call-template name="string-replace">
				<xsl:with-param name="string" select="pre"/>
				<xsl:with-param name="from" select="'&#10;'"/>
				<xsl:with-param name="to" select="'&#10;  '"/>
			</xsl:call-template>
		</PRE>
	</xsl:if>
	<xsl:if test="desc">
		<xsl:if test="pre">
			<B>Description</B><BR/>
		</xsl:if>
		<xsl:apply-templates select="desc"/><BR/>
	</xsl:if>
	<xsl:if test="ret">
		<B>Returns</B><BR/>
			<xsl:apply-templates select="ret"/><BR/>
	</xsl:if>
	</P>
</xsl:template>

<xsl:template match="func">
	<A><xsl:attribute name="name"><xsl:value-of select="@name"/></xsl:attribute></A>
	<P/>
	<xsl:variable name="fn-name" select="@name"/><B CLASS="tfc">The <TT><xsl:value-of select="$fn-name"/></TT> function</B><BR/>
	<B>Synopsis</B>
	<PRE><BR/>
		<xsl:for-each select="ancestor::interface/include">  #include &lt;<xsl:value-of select="."/>&gt;
</xsl:for-each>
		<xsl:choose>
			<xsl:when test="@wrap='true'">
				<xsl:text>  </xsl:text>
				<xsl:call-template name="string-replace">
					<xsl:with-param name="string" select="pre"/>
					<xsl:with-param name="from" select="','"/>
					<xsl:with-param name="to" select="',&#10;          '"/>
				</xsl:call-template>
			</xsl:when>
			<xsl:otherwise>
				<xsl:text>  </xsl:text><xsl:value-of select="pre"/>
			</xsl:otherwise>
		</xsl:choose>
		<BR/>
	</PRE>
	<xsl:if test="desc">
		<B>Description</B><BR/>
		<xsl:apply-templates select="desc"/>
		<xsl:if test="not(local-name(desc/*[position()=last()])='pre')">
			<BR/>
		</xsl:if>
	</xsl:if>
	<xsl:if test="ret">
		<B>Returns</B><BR/>
			<xsl:apply-templates select="ret"/><BR/>
	</xsl:if>
	<xsl:if test="desc or ret">
		<P/>
	</xsl:if>
</xsl:template>
<xsl:template match="meth">
	<A><xsl:attribute name="name"><xsl:value-of select="@name"/></xsl:attribute></A>
	<P/>
	<xsl:variable name="fn-name" select="concat(ancestor::interface/@name, '_', @name)"/><B CLASS="tfc">The <TT><xsl:value-of select="$fn-name"/></TT> function</B><BR/>
	<B>Synopsis</B>
	<PRE><BR/>
		<xsl:for-each select="ancestor::interface/include">  #include &lt;<xsl:value-of select="."/>&gt;
</xsl:for-each>
		<xsl:choose>
			<xsl:when test="@wrap='true'">
				<xsl:text>  </xsl:text>
				<xsl:call-template name="string-replace">
					<xsl:with-param name="string" select="pre"/>
					<xsl:with-param name="from" select="','"/>
					<xsl:with-param name="to" select="',&#10;          '"/>
				</xsl:call-template>
			</xsl:when>
			<xsl:otherwise>
				<xsl:text>  </xsl:text><xsl:value-of select="pre"/>
			</xsl:otherwise>
		</xsl:choose>
		<BR/>
	</PRE>
	<xsl:if test="desc">
		<B>Description</B><BR/>
		<xsl:apply-templates select="desc"/>
		<xsl:if test="not(local-name(desc/*[position()=last()])='pre')">
			<BR/>
		</xsl:if>
	</xsl:if>
	<xsl:if test="ret">
		<B>Returns</B><BR/>
			<xsl:apply-templates select="ret"/>
	</xsl:if>
	<xsl:if test="desc or ret">
		<P/>
	</xsl:if>
</xsl:template>

<xsl:template match="desc/example">
	<P>
	<B CLASS="tfc">Example <xsl:number level="any" count="example" format="1. "/><xsl:apply-templates select="title"/></B><BR/>
	<xsl:apply-templates select="desc"/>
	<xsl:apply-templates select="pre"/>
	</P>
</xsl:template>

<xsl:template match="desc/table|ret/table">
	<TABLE>
		<CAPTION><B>Table <xsl:number level="any" count="table" format="1. "/>
				<xsl:apply-templates select="title"/></B>
		</CAPTION>
		<xsl:copy-of select="tr"/>
	</TABLE>
</xsl:template>

<xsl:template match="library/desc|interface/desc|subtopic/desc|group/desc|code/desc|meth/desc|example/desc">
	<xsl:apply-templates/>
</xsl:template>

<xsl:template match="title/def|desc/def|th/def|td/def">
	<I>
	<xsl:apply-templates/>
	</I>
</xsl:template>

<xsl:template match="title/ident|desc/ident|ret/ident|th/ident|td/ident">
	<TT>
	<xsl:apply-templates/>
	</TT>
</xsl:template>

<xsl:template match="desc/pre|ret/pre">
	<PRE><xsl:text>
</xsl:text>
		<xsl:call-template name="string-replace">
			<xsl:with-param name="string" select="."/>
			<xsl:with-param name="from" select="'&#10;'"/>
			<xsl:with-param name="to" select="'&#10;  '"/>
		</xsl:call-template>
	</PRE>
</xsl:template>

<xsl:template match="link">
	<A>
		<xsl:attribute name="href">
			<xsl:value-of select="concat(@ref,'.html')"/>
		</xsl:attribute>
		<xsl:value-of select="."/>
	</A>
</xsl:template>

<xsl:template match="h1|h2|h3|tt|p|i|b|font|ul|ol|li|a|small">
	<xsl:copy-of select="."/>
</xsl:template>

</xsl:stylesheet>

