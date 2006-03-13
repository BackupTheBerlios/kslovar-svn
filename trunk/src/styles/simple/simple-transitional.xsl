<?xml version='1.0' encoding='UTF-8'?>
<xsl:stylesheet version='1.0' xmlns:xsl='http://www.w3.org/1999/XSL/Transform'>
  <xsl:output method='html'/>
  <xsl:template match='phrase'>
    <html>
      <body>
        <table width='100%' border='0'>
          <tr>
            <td><h1><xsl:value-of select='word'/></h1></td>
            <td align='right'><img src='{images-dir}/kslovar.png' /></td><!--{styles-dir} goes one level up (/home/user/.kde/share/apps/kslovar/styles/stylename) -->
          </tr>
        </table>
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