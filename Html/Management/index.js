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
                    var json = JSON.parse(message);
                    document.body.innerHTML = json.html;
                    currentTableName = "ReportInfo";
                } catch (error) {
                    alert(message);
                }
            });
            core.sendUserInfo.connect(function(message) {
                // get user info when login succeeded
                userInfo = message;
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
function tablesBtnClick(tableName) {
    var elements = document.getElementsByClassName('btn');
    let array = [];
    Array.prototype.forEach.call(elements, function (element) {
        array.push(element.id);
    });
    core.currentTable(userInfo, tableName);
    // current table name id
    currentTableName = tableName;
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
        if (element.id == 'append') {
        }else if (element.id == 'delete') {
            var pkey = document.getElementsByTagName('th')[0].id;
            var pval = element.parentNode.parentNode.children[0].textContent;
            var str = 'DELETE FROM ' + currentTableName + ' WHERE ' + pkey + '=\'' + pval + '\';';
            if (confirm("确定删除此条数据吗？")) {
                core.execSqlStatement(userInfo, currentTableName, str);
            }
        }else if (element.id == 'update') {
        }else if (element.id == 'show') {
        }
    }
}