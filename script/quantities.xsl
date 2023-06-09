<?xml version="1.0"?>
<html xsl:version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
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
        #filter_control { float: left; padding-left: 1em; padding-top: 4ex; }
        #key { float: right; margin-bottom: 2ex; margin-right: 2em; }
        #key h2 { margin: 0 0 1ex; font-size: 1em; }
        table { width: 100%; }
        td { padding: 0.5em; }
        .code {
            font-family: Courier;
            font-size: 10pt;
        }
        .units, #swatch {
            display: inline-block;
            border: solid black 1px;
        }
        .units {
            padding: 1em;
            margin: 1em;
        }
        #swatch {
            min-height: 1.2ex;
            min-width: 1.2ex;
        }
        .multiple { background-color: #EE8888; }
        .missing { background-color: #EEEE88; }
        .okay { background-color: #88EE88; }
        .quantity_type {
            margin-left: 1em;
            font-family: Times;
        }
        .keep_together { white-space: nowrap; }
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
                $('#quantity_filter').keyup(function() {
                    $("td.code:icontains('" + $(this).val() + "')").parent().show();
                    $("td.code:not(:icontains('" + $(this).val() + "'))").parent().hide();
                });
            });

        });
    </script>

    <h1>BioCro Quantity List</h1>
    <div id="head">
      <div id="filter_control">Show only quantities with names matching <input type="text" id="quantity_filter"/> (case insensitive)</div>
      &#xA0;
      <div id="key">
        <h2>Key: </h2>
        <div>
          <div><div id="swatch" class="multiple"/> = multiple different unit designations</div>
          <div><div id="swatch" class="missing"/> = no unit designation found</div>
          <div><div id="swatch" class="okay"/> = single unique unit designation found</div>
        </div>
      </div>
    </div>
    <table border="1">
      <tr>
        <th>Quantity Name</th><th>Units</th><th>Direct Module Usage</th><th>Differential Module Usage</th>
      </tr>
      <xsl:for-each select="quantity_info/quantities/quantity[not(@name = preceding-sibling::quantity/@name)]">
        <xsl:sort select="@name"/>
        <tr>
          <td class="code"><xsl:value-of select="@name"/></td>
          <td>
            <xsl:choose>
              <xsl:when test="count(../quantity[@name = current()/@name]) > 1">
                <xsl:for-each select="../quantity[@name = current()/@name]">
                  <span class="units multiple"><xsl:value-of select="@units"/></span><br/>
                </xsl:for-each>
              </xsl:when>
              <xsl:when test="@units = 'No units found'">
                <span class="units missing"><xsl:value-of select="@units"/></span><br/>
              </xsl:when>
              <xsl:otherwise>
                <span class="units okay"><xsl:value-of select="@units"/></span><br/>
              </xsl:otherwise>
            </xsl:choose>
          </td>
          <td>
            <xsl:for-each select="/quantity_info/usage_info/quantity[@quantity_name = current()/@name]">
              <xsl:if test="/quantity_info/modules/module[@name = current()/@module_name]/@type = 'direct'">
                <span class="keep_together">
                  <xsl:if test="@quantity_type = 'input'">⟶ </xsl:if>
                  <span class="code"><xsl:value-of select="@module_name"/></span>
                  <xsl:if test="@quantity_type = 'output'"> ⟶</xsl:if>
                </span>
                <span class="quantity_type"><xsl:value-of select="concat(' (', @quantity_type, ')')"/></span>
                <br/>
              </xsl:if>
            </xsl:for-each>
          </td>
          <td>
            <xsl:for-each select="/quantity_info/usage_info/quantity[@quantity_name = current()/@name]">
              <xsl:if test="/quantity_info/modules/module[@name = current()/@module_name]/@type = 'differential'">
                <span class="keep_together">
                  <xsl:if test="@quantity_type = 'input'">⟶ </xsl:if>
                  <span class="code"><xsl:value-of select="@module_name"/></span>
                  <xsl:if test="@quantity_type = 'output'"> ⟶</xsl:if>
                </span>
                <span class="quantity_type"><xsl:value-of select="concat(' (', @quantity_type, ')')"/></span>
                <br/>
              </xsl:if>
            </xsl:for-each>
          </td>
        </tr>
      </xsl:for-each>
    </table>
  </body>
</html>
