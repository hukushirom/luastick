<?xml version="1.0"?>

<!--
man.xsl - XSLT style-sheet for generating C Library man Page
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
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:output method="text"
	encoding="UTF-8"/>
<xsl:param name="section"/>
<xsl:param name="date"/>
<xsl:param name="lib"/>
<xsl:param name="title"/>
<xsl:param name="baseref"/>

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

<xsl:template match="interface">
	<xsl:apply-templates select="comments"/>

	<xsl:text>.TH </xsl:text>
	<xsl:value-of select="@name"/>
	<xsl:text> </xsl:text>
	<xsl:value-of select="$section"/>
	<xsl:text> "</xsl:text>
	<xsl:value-of select="$date"/>
	<xsl:text>" "</xsl:text>
	<xsl:if test="$lib">
		<xsl:value-of select="$lib"/>
		<xsl:text>" "</xsl:text>
	</xsl:if>
	<xsl:value-of select="$title"/>
	<xsl:text>"
</xsl:text>

	<xsl:text>.SH NAME
</xsl:text>
	<xsl:value-of select="@name"/>
	<xsl:text> \- </xsl:text>
	<xsl:value-of select="normalize-space(@short)"/>
	<xsl:text>
</xsl:text>
	<xsl:apply-templates select="." mode="synopsis"/>
	<xsl:apply-templates select="." mode="description"/>
	<xsl:apply-templates select="self::interface[descendant::ret]" mode="returns"/>

	<xsl:if test="descendant::link">
		<xsl:text>.SH LINKS
</xsl:text>
		<xsl:for-each select="descendant::link">
			<xsl:text>
.I </xsl:text>
			<xsl:value-of select="$baseref"/>
			<xsl:value-of select="@ref"/>
			<xsl:text>.html
</xsl:text>
		</xsl:for-each>
	</xsl:if>
</xsl:template>

<xsl:template match="comments">
	<xsl:text>.\" </xsl:text>
	<xsl:call-template name="string-replace">
		<xsl:with-param name="string" select="."/>
		<xsl:with-param name="from" select="'&#10;'"/>
		<xsl:with-param name="to">&#10;.\" </xsl:with-param>
	</xsl:call-template>
	<xsl:text>
</xsl:text>
</xsl:template>

<!-- SYNOPSIS -->

<xsl:template match="interface" mode="synopsis">
	<xsl:text>.SH SYNOPSIS
.nf
</xsl:text>
	<xsl:for-each select="include">
		<xsl:text>.B #include &lt;</xsl:text>
		<xsl:value-of select="."/>
		<xsl:text>&gt;
</xsl:text>
	</xsl:for-each>
	<xsl:text>.sp
</xsl:text>
	<xsl:apply-templates select="group" mode="synopsis"/>
	<xsl:text>
.fi
</xsl:text>
</xsl:template>

<xsl:template match="group" mode="synopsis">
	<xsl:apply-templates select="code|func|meth" mode="synopsis"/>
	<xsl:if test="not(position()=last())">
		<xsl:text>.sp
</xsl:text>
	</xsl:if>
</xsl:template>

<xsl:template match="code" mode="synopsis">
	<xsl:call-template name="string-replace">
		<xsl:with-param name="string">
			<xsl:call-template name="string-replace">
				<xsl:with-param name="string">
					<xsl:call-template name="string-replace">
						<xsl:with-param name="string" select="pre"/>
						<xsl:with-param name="from" select="'&#10;&#10;'"/>
						<xsl:with-param name="to">pLaCeHoLdEr&#10;</xsl:with-param>
					</xsl:call-template>
				</xsl:with-param>
				<xsl:with-param name="from" select="'&#10;'"/>
				<xsl:with-param name="to">&#10;.B </xsl:with-param>
			</xsl:call-template>
		</xsl:with-param>
		<xsl:with-param name="from" select="'pLaCeHoLdEr'"/>
		<xsl:with-param name="to">&#10;.sp</xsl:with-param>
	</xsl:call-template>
	<xsl:text>
.sp
</xsl:text>
</xsl:template>

<xsl:template match="func|meth" mode="synopsis">
	<xsl:text>.BI "</xsl:text>
	<xsl:choose>
		<xsl:when test="param">
			<xsl:apply-templates select="param"/>
		</xsl:when>
		<xsl:otherwise>
			<xsl:value-of select="pre"/>
		</xsl:otherwise>
	</xsl:choose>
	<xsl:text>
.br
</xsl:text>
</xsl:template>

<xsl:template match="func/param|meth/param">
	<xsl:variable name="subs">
		<xsl:choose>
			<xsl:when test="position()=1">
				<xsl:value-of select="../pre"/>
			</xsl:when>
			<xsl:otherwise>
				<xsl:variable name="prev" select="preceding-sibling::param[position()=1]/@name"/>
				<xsl:value-of select="concat(substring-after(../pre,concat(' ',$prev,',')),substring-after(../pre,concat(' ',$prev,')')),substring-after(../pre,concat('*',$prev,')')),substring-after(../pre,concat('*',$prev,',')),substring-after(../pre,concat('*',$prev,'[')))"/>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:variable>

	<xsl:variable name="intr" select="concat(substring-before($subs,concat(' ',@name,',')),substring-before($subs,concat(' ',@name,')')),substring-before($subs,concat('*',@name,')')),substring-before($subs,concat('*',@name,',')),substring-before($subs,concat('*',@name,'[')))"/>

	<xsl:choose>
		<xsl:when test="../@wrap='true' and substring-before($intr,',')">
			<xsl:value-of select="substring-before($intr,',')"/>
			<xsl:text>,
.BI "           </xsl:text>
			<xsl:value-of select="substring-after($intr,',')"/>
		</xsl:when>
		<xsl:otherwise>
			<xsl:value-of select="$intr"/>
		</xsl:otherwise>
	</xsl:choose>
	<xsl:variable name="off" select="string-length($intr)+1"/>
	<xsl:value-of select="substring($subs,$off,1)"/>
	<xsl:text>" </xsl:text>
	<xsl:value-of select="@name"/>
	<xsl:text> "</xsl:text>
	<xsl:variable name="off" select="$off+string-length(@name)+1"/>
	<xsl:choose>
		<xsl:when test="../@wrap='true' and substring($subs,$off,1)=','">
			<xsl:text>,
.BI "           </xsl:text>
		</xsl:when>
		<xsl:otherwise>
			<xsl:value-of select="substring($subs,$off,1)"/>
		</xsl:otherwise>
	</xsl:choose>

	<xsl:if test="position()=last()">
		<xsl:value-of select="substring($subs,$off+1)"/>
	</xsl:if>
</xsl:template>

<!-- DESCRIPTION -->

<xsl:template match="interface" mode="description">
	<xsl:text>.SH DESCRIPTION
</xsl:text>
	<xsl:apply-templates select="desc"/>
	<xsl:text>
.PP
</xsl:text>
	<xsl:apply-templates select="group/meth[child::desc]|group/func[child::desc]" mode="description"/>
</xsl:template>

<xsl:template match="meth|func" mode="description">
	<xsl:text>.TP
.B </xsl:text>
	<xsl:value-of select="@name"/>
	<xsl:if test="desc/@combine">
		<xsl:text>\fR, \fB</xsl:text>
		<xsl:value-of select="desc/@combine"/>
	</xsl:if>
	<xsl:text>
</xsl:text>
	<xsl:apply-templates select="desc"/>
	<xsl:text>
</xsl:text>
</xsl:template>

<!-- RETURNS -->

<xsl:template match="interface" mode="returns">
	<xsl:text>.SH RETURNS
</xsl:text>
	<xsl:apply-templates select="group/meth[ret]|group/func[ret]" mode="returns"/>
</xsl:template>

<xsl:template match="meth|func" mode="returns">
	<xsl:text>.TP
.B </xsl:text>
	<xsl:value-of select="@name"/>
	<xsl:text>
</xsl:text>
	<xsl:apply-templates select="ret"/>
	<xsl:text>
</xsl:text>
</xsl:template>

<xsl:template match="tt|link">
	<xsl:variable name="next-node" select="following-sibling::node()[position()=1]"/>
	<xsl:choose>
		<xsl:when test="starts-with($next-node,' ') or starts-with($next-node,'&#10;')">
			<xsl:text>
.I </xsl:text>
			<xsl:apply-templates/>
			<xsl:if test="string-length(normalize-space($next-node))">
					<xsl:text>
</xsl:text>
			</xsl:if>
		</xsl:when>
		<xsl:when test="preceding-sibling::node()">
			<xsl:text>
.IR "</xsl:text>
			<xsl:apply-templates/>
			<xsl:text>" </xsl:text>
		</xsl:when>
		<xsl:otherwise>
			<xsl:text>.IR "</xsl:text>
			<xsl:apply-templates/>
			<xsl:text>" </xsl:text>
		</xsl:otherwise>
	</xsl:choose>
</xsl:template>

<xsl:template match="ident|def">
	<xsl:variable name="next-node" select="following-sibling::node()[position()=1]"/>
	<xsl:choose>
		<xsl:when test="starts-with($next-node,' ') or starts-with($next-node,'&#10;')">
			<xsl:text>
.B </xsl:text>
			<xsl:apply-templates/>
			<xsl:if test="string-length(normalize-space($next-node))">
					<xsl:text>
</xsl:text>
			</xsl:if>
		</xsl:when>
		<xsl:otherwise>
			<xsl:text>
.BR "</xsl:text>
			<xsl:apply-templates/>
			<xsl:text>" </xsl:text>
		</xsl:otherwise>
	</xsl:choose>
</xsl:template>

<xsl:template match="pre">
	<xsl:text>
.PP
.RS
.nf
.ta 4n 19n 31n
</xsl:text>
	<xsl:value-of select="."/>
	<xsl:text>
.ta
.fi
.RE
.PP
</xsl:text>
</xsl:template>

<xsl:template match="p">
	<xsl:text>
.sp
</xsl:text>
</xsl:template>

<xsl:template match="desc/ul|ret/ul|desc/ol|ret/ol">
	<xsl:text>
.sp</xsl:text>
	<xsl:apply-templates/>
	<xsl:text>
.sp
</xsl:text>
</xsl:template>

<xsl:template match="ul/li">
	<xsl:text>
.br
</xsl:text>
	<xsl:apply-templates/>
</xsl:template>

<xsl:template match="ol/li">
	<xsl:text>
.br
</xsl:text>
	<xsl:number level="single" count="li" format="1. "/>
	<xsl:apply-templates/>
</xsl:template>

<xsl:template match="text()">
	<xsl:variable name="prev-node" select="local-name(preceding-sibling::node()[position()=1])"/>
	<xsl:choose>
		<xsl:when test="($prev-node='tt' or $prev-node='ident' or $prev-node='def') and not(starts-with(.,' ')) and not(starts-with(.,'&#10;')) and contains(normalize-space(.),' ')">
<!--
If the preceeding tt, ident, or def butts up against the text
without a space between, the .BR macro must be use but that
requires that words after the first appear on a separate line.
 -->
			<xsl:call-template name="string-replace">
				<!-- escape escape -->
				<xsl:with-param name="string" select="substring-before(.,' ')"/>
				<xsl:with-param name="from" select="'&#92;'"/>
				<xsl:with-param name="to">&#92;&#92;</xsl:with-param>
			</xsl:call-template><xsl:text>
</xsl:text>
			<xsl:call-template name="string-replace">
				<!-- escape escape -->
				<xsl:with-param name="string" select="normalize-space(substring-after(.,' '))"/>
				<xsl:with-param name="from" select="'&#92;'"/>
				<xsl:with-param name="to">&#92;&#92;</xsl:with-param>
			</xsl:call-template>
		</xsl:when>
		<xsl:otherwise>
			<xsl:call-template name="string-replace">
				<!-- escape escape -->
				<xsl:with-param name="string" select="normalize-space(.)"/>
				<xsl:with-param name="from" select="'&#92;'"/>
				<xsl:with-param name="to">&#92;&#92;</xsl:with-param>
			</xsl:call-template>
		</xsl:otherwise>
	</xsl:choose>
</xsl:template>

<xsl:template match="example"/>

</xsl:stylesheet>

