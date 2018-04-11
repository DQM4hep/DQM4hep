
<table>
  <tbody>

    <tr>
      <td><strong>Pattern</strong></td>
      <td><strong>XML</strong></td>
      <td><strong>JSON</strong></td>
      <td><strong>YAML</strong></td>
      <td><strong>C++</strong></td>
    </tr>

    <tr>
      <td>Null</td>
      <td><code>&lt;e/&gt;</code></td>
      <td><code>"e": null</code></td>
      <td><code>e: null</code></td>
      <td><code>node["e"]</code></td>
    </tr>

    <tr>
      <td>Boolean</td>
      <td><code>&lt;e&gt;<br/>
      &nbsp;&nbsp;true<br/>
      &lt;/e&gt;</code></td>
      <td><code>"e": true</code></td>
      <td><code>e: true</code></td>
      <td><code>node["e"].as&lt;bool&gt;()</code></td>
    </tr>

    <tr>
      <td>Integer</td>
      <td><code>&lt;e&gt;<br/>
      &nbsp;&nbsp;12345<br/>
      &lt;/e&gt;</code></td>
      <td><code>"e": 12345</code></td>
      <td><code>e: 12345</code></td>
      <td><code>node["e"].as&lt;int&gt;()</code></td>
    </tr>

    <tr>
      <td>Floating point</td>
      <td><code>&lt;e&gt;<br/>
      &nbsp;&nbsp;42.43<br/>
      &lt;/e&gt;</code></td>
      <td><code>"e": 42.43</code></td>
      <td><code>e: 42.43</code></td>
      <td><code>node["e"].as&lt;float&gt;()<br/>node["e"].as&lt;double&gt;()</code></td>
    </tr>

    <tr>
      <td>String</td>
      <td><code>&lt;e&gt;<br/>
      &nbsp;&nbsp;text<br/>
      &lt;/e&gt;</code></td>
      <td><code>"e": "text"</code></td>
      <td><code>e: text</code></td>
      <td><code>node["e"].as&lt;std::string&gt;()</code></td>
    </tr>

    <tr>
      <td>Attribute</td>
      <td><code>&lt;e name="value"/&gt;</code></td>
      <td><code>"e":{<br/>
      &nbsp;&nbsp;"@name": "value"<br/>
      }</code></td>
      <td><code>e:<br/>&nbsp;&nbsp;_name: value</code></td>
      <td><code>node["e"].attribute("name")</code></td>
    </tr>

    <tr>
      <td>Attribute and value</td>
      <td><code>&lt;e name="value"&gt;<br/>
      &nbsp;&nbsp;text<br/>
      &lt;/e&gt;</code></td>
      <td><code>"e": {<br/>
      &nbsp;&nbsp;"@name": "value",<br/>
      &nbsp;&nbsp;"#text": "text"<br/>
      }</code></td>
      <td><code>e:<br/>
      &nbsp;&nbsp;_name: value<br/>
      &nbsp;&nbsp;__text: text</code></td>
      <td><code>node["e"].attribute("name")<br/>node["e"].as&lt;std::string&gt;()</code></td>
    </tr>

    <tr>
      <td>Associative object</td>
      <td><code>
        &lt;e&gt;<br/>
        &nbsp;&nbsp;&lt;a&gt;text&lt;/a&gt;<br/>
        &nbsp;&nbsp;&lt;b&gt;text&lt;/b&gt;<br/>
        &lt;/e&gt;</code></td>
      <td><code>"e": {<br/>
        &nbsp;&nbsp;"a": "text",<br/>
        &nbsp;&nbsp;"b": "text"<br/>
        }</code></td>
      <td><code>e:<br/>
      &nbsp;&nbsp;a: text<br/>
      &nbsp;&nbsp;b: text
      </code></td>
      <td><code>
      node["e"]["a"].as&lt;std::string&gt;()<br/>
      node["e"]["b"].as&lt;std::string&gt;()</code></td>
    </tr>

    <tr>
      <td>Array</td>
      <td><code>
        &lt;e&gt;<br/>
        &nbsp;&nbsp;&lt;a&gt;text1&lt;/a&gt;<br/>
        &nbsp;&nbsp;&lt;a&gt;text2&lt;/a&gt;<br/>
        &lt;/e&gt;</code></td>
      <td><code>"e": {<br/>
        &nbsp;&nbsp;"a": ["text1", "text2"]<br/>
        }<br/>
        </code></td>
      <td><code>e:<br/>
      &nbsp;&nbsp;a: [text1, text2]<br/>
      e:<br/>
      &nbsp;&nbsp;a: text1<br/>
      &nbsp;&nbsp;a: text2
      </code></td>
      <td><code>
      node["e"]["a"][0].as&lt;std::string&gt;()<br/>
      node["e"]["a"][1].as&lt;std::string&gt;()</code></td>
    </tr>

  </tbody>
</table>
