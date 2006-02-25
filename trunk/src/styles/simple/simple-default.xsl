<?xml version='1.0' encoding='UTF-8'?>
<xsl:stylesheet version='1.0' xmlns:xsl='http://www.w3.org/1999/XSL/Transform'>
  <xsl:output method='html'/>
  <xsl:template match='phrase'>
    <html>
      <body>
        <h1><xsl:value-of select='word'/>, <xsl:value-of select='type'/></h1>
        <p>
          <h2>{explanations-examples}</h2> <!--You can use also {explanations} and {examples} -->
          <xsl:for-each select='explanations'>
            <li><xsl:value-of select='explanation'/>, <i><xsl:value-of select='example'/></i></li>
          </xsl:for-each>
        </p>
        <p>
          <h2>{synonym}</h2>
          <xsl:for-each select='synonym'>
            <li><a><xsl:attribute name='href'>http://<xsl:value-of select='@id'/></xsl:attribute><xsl:value-of select='.'/></a></li>
          </xsl:for-each>
        </p>
        <p>
          <h2>{antonym}</h2>
          <xsl:for-each select='antonym'>
            <li><a><xsl:attribute name='href'>http://<xsl:value-of select='@id'/></xsl:attribute><xsl:value-of select='.'/></a></li>
          </xsl:for-each>
        </p>
        <p>
          <h2>{word-family}</h2>
          <xsl:for-each select='word-family'>
            <li><a><xsl:attribute name='href'>http://<xsl:value-of select='@id'/></xsl:attribute><xsl:value-of select='.'/></a></li>
          </xsl:for-each>
        </p>
      </body>
    </html>
  </xsl:template>
</xsl:stylesheet>