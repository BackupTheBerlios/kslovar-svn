<?xml version='1.0' encoding='UTF-8'?>
<xsl:stylesheet version='1.0' xmlns:xsl='http://www.w3.org/1999/XSL/Transform'>
  <xsl:output method='html'/>
  <xsl:template match='phrase'>
    <html>
      <body>
        <h1><xsl:value-of select='word'/>, <xsl:value-of select='type'/></h1>
        <p>
          <h2>{explanations-examples}</h2>
          <xsl:for-each select='explanations'>
            <li><xsl:value-of select='explanation'/>, <i><xsl:value-of select='example'/></i> - <b><xsl:value-of select='explanation2'/></b>,  <b><i><xsl:value-of select='example2'/></i></b></li>
          </xsl:for-each>
        </p>
      </body>
    </html>
  </xsl:template>
</xsl:stylesheet>