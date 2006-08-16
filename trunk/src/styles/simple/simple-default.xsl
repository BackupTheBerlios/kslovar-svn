<?xml version='1.0' encoding='UTF-8'?>
<xsl:stylesheet version='1.0' xmlns:xsl='http://www.w3.org/1999/XSL/Transform'>
  <xsl:output method='html'/>
  <xsl:template match='phrase'>
    <html>
      <head>
        <script language="javascript" type="text/javascript">
          function toggleSynonym()
{
if(document.getElementById('synonyms').style.visibility == 'visible')
{
document.getElementById('synonyms').style.visibility = 'hidden';
}
else
{
document.getElementById('synonyms').style.visibility = 'visible';
}
}
function toggleAntonym()
{
if(document.getElementById('antonyms').style.visibility == 'visible')
{
document.getElementById('antonyms').style.visibility = 'hidden';
}
else
{
document.getElementById('antonyms').style.visibility = 'visible';
}
}
function toggleFamily()
{
if(document.getElementById('family').style.visibility == 'visible')
{
document.getElementById('family').style.visibility = 'hidden';
}
else
{
document.getElementById('family').style.visibility = 'visible';
}
}
</script>
      </head>
      <body>
        <table width='100%' border='0'>
          <tr>
            <td><h1><xsl:value-of select='word'/>, <xsl:value-of select='type'/></h1></td>
            <td align='right'><img src='{images-dir}/kslovar.png' /></td><!--{styles-dir} goes one level up (/home/user/.kde/share/apps/kslovar/styles/stylename) -->
          </tr>
        </table>
        <p>
          <h2>{explanations-examples}</h2> <!--You can use also {explanations} and {examples} -->
          <xsl:for-each select='explanations'>
            <li><xsl:value-of select='explanation'/>, <i><xsl:value-of select='example'/></i></li>
          </xsl:for-each>
        </p>
        <p>
          <table>
            <tr>
              <td><h2><a href="javascript:toggleSynonym()">{synonym}</a></h2></td>
              <td><h2><a href="javascript:toggleAntonym()">{antonym}</a></h2></td>
              <td><h2><a href="javascript:toggleFamily()">{word-family}</a></h2></td>
            </tr>
            <tr>
              <td><div id="synonyms" style="visibility: hidden">+
          <xsl:for-each select='synonym'>
            <li><a><xsl:attribute name='href'>http://<xsl:value-of select='@id'/></xsl:attribute><xsl:value-of select='.'/></a></li>
          </xsl:for-each>
          </div></td>
              <td><div id="antonyms" style="visibility: hidden">+<xsl:for-each select='antonym'>
            <li><a><xsl:attribute name='href'>http://<xsl:value-of select='@id'/></xsl:attribute><xsl:value-of select='.'/></a></li>
          </xsl:for-each></div></td>
              <td><div id="family" style="visibility: hidden">+<xsl:for-each select='word-family'>
            <li><a><xsl:attribute name='href'>http://<xsl:value-of select='@id'/></xsl:attribute><xsl:value-of select='.'/></a></li>
          </xsl:for-each></div></td>
            </tr>
          </table>
        </p>
      </body>
    </html>
  </xsl:template>
</xsl:stylesheet>