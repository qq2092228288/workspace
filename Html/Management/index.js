/**
 * @description set web 
 */
document.write("<meta name='viewport' content='width=device-width, initial-scale=1.0'>");
document.write("<link href='./images/favicon.ico' rel='shortcut icon'>");
document.write("<title>泽耀医疗管理系统</title>");
document.write("<link type='text/css' rel='stylesheet' href='./index.css'></link>");
document.write("<link type='text/css' rel='stylesheet' href='./main.css'></link>");
document.write("<script type='text/javascript' src='./qwebchannel.js'></script>");
/**
 * @description Html connection to server 
 */
function connect_server(url) {
    // set web server address
    if (location.search != "")
        var baseUrl = (/[?&]webChannelBaseUrl=([A-Za-z0-9\-:/\.]+)/.exec(location.search)[1]);
    else
        var baseUrl = url;
    var socket = new WebSocket(baseUrl);
    // connect failed
    socket.onclose = function() {
        console.error("web channel closed");    // server close
    };
    socket.onerror = function(error) {
        console.error("web channel error: " + error);
    };
    // connected
    socket.onopen = function() {
        new QWebChannel(socket, function(channel) {
            // 'core' is the channel between server and web
            window.core = channel.objects.core;
            // received server message
            core.sendText.connect(function(message) {
                try {
                    // set html from server
                    let json = JSON.parse(message);
                    document.body.innerHTML = json.html;
                } catch (error) {
                    alert(message);
                }
            });
            core.sendUserInfo.connect(function(message) {
                // get user info when login succeeded
                userInfo = message;
            });
            core.sendReport.connect(function(report) {
                showReport(report);
            });
            // get login ui
            core.loginUi();
        });
    }
}
/**
 * @description Create a temporary file and return the file address 
 * @param {string} fdata file data
 * @param {string} ftype file type, example: text/javascript text/css
 * @returns {string}
 */
function createTempFile(fdata, ftype) {
    var blob = new Blob([fdata], {type: ftype});
    return URL.createObjectURL(blob);
}
/**
 * 
 * @param {string} xmlUrl 
 * @returns {rootNode}
 */
function readXml(xmlUrl) {
    var elements = document.getElementsByClassName('btn');
    if (window.XMLHttpRequest)
    {// code for IE7+, Firefox, Chrome, Opera, Safari
        xmlhttp=new XMLHttpRequest();
    }
    else
    {// code for IE6, IE5
        xmlhttp=new ActiveXObject("Microsoft.XMLHTTP");
    }
    xmlhttp.open("GET",xmlUrl,false);
    xmlhttp.send();
    xmlDoc=xmlhttp.responseXML;
    return xmlDoc.getElementsByTagName("body");
}

function loginBtnClick() {
    core.login(document.getElementById('username').value, document.getElementById('password').value);
}
/**
 * @description get the table data
 * @param {string} id button id
 */
function tablesBtnClick(id) {
    var elements = document.getElementsByClassName('btn');
    let array = [];
    Array.prototype.forEach.call(elements, function (element) {
        array.push(element.id);
    });
    core.currentTable(userInfo, id);
}
/**
 * @description click tbody element
 * @param {window.event} event 
 */
function tbodyClick(event) {
    var element = event.target || event.srcElement;
    if (element.tagName == 'TD') {
        element.style.whiteSpace = 'normal';
        if (element.contentEditable == 'true') {
            element.style.color = 'red';
        }
    }
    else if (element.className == 'cell-btn') {
        var headers = document.getElementsByTagName('th');
        var rowdata = element.parentNode.parentNode.children;
        var tableName = document.getElementsByTagName('caption')[0].id;
        try {
            if (element.id == 'append') {
                let data = [];
                for (let i = 0; i < rowdata.length - 1; ++i) {
                    data.push(sqlValue(rowdata[i].textContent));
                }
                let arr = ['INSERT INTO', tableName, 'VALUES (', data.toString(), ')'];
                core.execSqlStatement(userInfo, tableName, arr.join(' '));
            }else if (element.id == 'delete') {
                let arr = ['DELETE FROM', tableName, 'WHERE', sqlPair(headers[0].id, rowdata[0].textContent)];
                if (confirm('确定要删除此数据吗?')) {
                    core.execSqlStatement(userInfo, tableName, arr.join(' '));
                }
            }else if (element.id == 'update') {
                let dlist = [];
                for (let i = 1; i < rowdata.length - 1; ++i) {
                    dlist.push(sqlPair(headers[i].id, rowdata[i].textContent));
                }
                let arr = ['UPDATE', tableName, 'SET', dlist.toString(), 'WHERE', sqlPair(headers[0].id, rowdata[0].textContent)];
                if (confirm('确定要修改此数据吗?')) {
                    core.execSqlStatement(userInfo, tableName, arr.join(' '));
                }
            }else if (element.id == 'show') {
                let arr = ['SELECT * FROM', tableName, 'WHERE', sqlPair(headers[0].id, rowdata[0].textContent), 'AND', sqlPair(headers[1].id, rowdata[1].textContent)];
                core.execSqlStatement(userInfo, tableName, arr.join(' '));
            }
        } catch (error) {
            console.error("update error: " + error);
        }
    }
}
/**
 * 
 * @param {string} key 
 * @param {string/int} value 
 * @returns {string} key='value'
 */
function sqlPair(key, value) {
    return (key + '=' + sqlValue(value));
}
/**
 * 
 * @param {string} value 
 * @returns {string}
 */
function sqlValue(value) {
    if (value == 'default')
        return value;
    return ('\'' + value.trim() + '\'');
}

function showReport(report) {
    // console.log(report);
    new MyLayer({
        top:"10%",
        left:"10%",
        width:"80%",
        height:"80%",
        title:"报告时间：" + report.reporttime + " 报告名称：" + report.name,
        content:report.reportdata,
        // url: "http://localhost:8080/"
    }).openLayer();
}

/**
 * @param options 弹窗基本配置信息
 * @constructor 构造方法
 */

 function MyLayer(options) {
    this.options = options ;
  }
  
  /**
   * 打开弹窗
   */

   MyLayer.prototype.openLayer = function () {
     // 弹框窗div 绝对定位、设置zindex值1001
    var background_layer = document.createElement("div");
    background_layer.setAttribute("class", "background-layer");
    // 弹框顶层div 绝对定位、设置zindex值1002
    var open_layer = document.createElement("div");
    open_layer.setAttribute("class", "open-layer");
    // 弹框标题行p---两个子节点--标题节点span---span_title--关闭节点span_close
    var div_toolBar = document.createElement("div");
    div_toolBar.setAttribute("class", "tool-bar");
    //标题节点span---span_title 子节点有一个文本节点
    var span_title = document.createElement("span");
    span_title.setAttribute("class", "span-title");
    var span_title_content = document.createTextNode(this.options.title === undefined ? "" : this.options.title);
    span_title.appendChild(span_title_content);
    div_toolBar.appendChild(span_title);
    // 关闭节点span_close 添加关闭点击事件，子节点也有一个文本节点
    var span_close = document.createElement("span");
    span_close.setAttribute("class", "span-close");
    span_close.onclick = function () {
        open_layer.style.display = "none";
        background_layer.style.display = "none";
    };
    var span_close_content = document.createTextNode("关闭");
    span_close.appendChild(span_close_content);
    div_toolBar.appendChild(span_close);
    open_layer.appendChild(div_toolBar);
    // 内容div 添加到open_layer中
    var div_content = document.createElement("div");
    div_content.style.textAlign = "center";
    var content_area = document.createTextNode(this.options.content === undefined ? "" : this.options.content);
    div_content.appendChild(content_area);
    open_layer.appendChild(div_content);
    // 内嵌页面iframe，添加到open_layer中
    // var iframe = document.createElement("iframe");
    // iframe.src = this.options.url === undefined ? "" : this.options.url;
    // iframe.style.width = "100%";
    // iframe.style.height = "100%";
    // open_layer.appendChild(iframe);
    
    // open_layer, background_layer添加到网页中
    document.body.appendChild(open_layer);
    document.body.appendChild(background_layer);
    // 在调用MyLayer实例的openLayer方法时显示弹框
    open_layer.style.display = "block" ;
    background_layer.style.display = "block";
};