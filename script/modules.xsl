<?xml version="1.0"?>
<xsl:transform version="1.0"
               xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
  <xsl:param name="verbose" select="'no'"/>
  <xsl:template match="/">
    <xsl:if test="$verbose = 'yes'">
      <xsl:message>
        <xsl:text>Starting transform</xsl:text>
      </xsl:message>
    </xsl:if>
    <html>
      <head>
        <style>
            body {
                font-family: Arial;
                font-size: 12pt;
                background-color: #EEEEEE;
            }
            h1 {
                text-align: center;
                padding: 1em 0em 0em;
            }
            #head { padding: 1em; }
            table { width: 100%; }
            td { padding: 0.5em; }
            .code {
                font-family: Courier;
                font-size: 10pt;
            }
        </style>
      </head>
      <body>
        <script src="jquery-3.6.0.js"></script>
        <script>
            $( document ).ready(function() {

                $.expr[':'].icontains = function(a, i, m) {

                    return $(a).text().toUpperCase()
                        .indexOf(
                            m[3].toUpperCase()
                        ) >= 0;
                };

                $(function() {
                    $('#module_filter').keyup(function() {
                        $("td.code:icontains('" + $(this).val() + "')").parent().show();
                        $("td.code:not(:icontains('" + $(this).val() + "'))").parent().hide();
                    });
                });

            });
        </script>

        <h1>BioCro Module List</h1>
        <div id="head">
          <div id="filter_control">Show only modules with names matching <input type="text" id="module_filter"/> (case insensitive)</div>
        </div>
        <table border="1">
          <tr>
            <th>Module Name</th><th>Type</th><th>Inputs</th><th>Outputs</th>
          </tr>
          <xsl:if test="$verbose = 'yes'">
            <xsl:message>
              <xsl:text>Processing modules</xsl:text>
            </xsl:message>
          </xsl:if>
          <xsl:for-each select="quantity_info/modules/module">
            <xsl:sort select="@name"/>
            <xsl:if test="$verbose = 'yes'">
              <xsl:message><xsl:value-of select="@name"/></xsl:message>
            </xsl:if>
            <tr>
              <td class="code"><xsl:value-of select="@name"/></td>
              <td class="code"><xsl:value-of select="@type"/></td>
              <td>
                <xsl:for-each
                    select="/quantity_info/usage_info/quantity[@module_name
                            = current()/@name and @quantity_type =
                            'input']">
                  <xsl:sort select="@quantity_name"/>
                  <span class="keep_together">
                    <span class="code"><xsl:value-of select="@quantity_name"/></span>
                  </span>
                  <span style="min-width: 1em; display: inline-block;"/>
                  <span>
                    <xsl:text>(</xsl:text>
                    <xsl:value-of
                        select="/quantity_info/quantities/quantity[@name
                                = current()/@quantity_name]/@units"/>
                    <xsl:text>)</xsl:text>
                  </span>
                  <br/>
                </xsl:for-each>
              </td>
              <td>
                <xsl:for-each
                    select="/quantity_info/usage_info/quantity[@module_name
                            = current()/@name and @quantity_type =
                            'output']">
                  <xsl:sort select="@quantity_name"/>
                  <span class="keep_together">
                    <span class="code"><xsl:value-of select="@quantity_name"/></span>
                  </span>
                  <span style="min-width: 1em; display: inline-block;"/>
                  <span>
                    <xsl:text>(</xsl:text>
                    <xsl:value-of
                        select="/quantity_info/quantities/quantity[@name
                                = current()/@quantity_name]/@units"/>
                    <xsl:text>)</xsl:text>
                  </span>
                  <br/>
                </xsl:for-each>
              </td>
            </tr>
          </xsl:for-each>
        </table>
      </body>
    </html>
  </xsl:template>
</xsl:transform>

