/**
 * @description set web 
 */
document.write("<meta name='viewport' content='width=device-width, initial-scale=1.0'>");
document.write("<link href='./images/favicon.ico' rel='shortcut icon'>");
document.write("<title>泽耀医疗管理系统</title>");
document.write("<link type='text/css' rel='stylesheet' href='./index.css'></link>");
document.write("<link type='text/css' rel='stylesheet' href='./main.css'></link>");
document.write("<link type='text/css' rel='stylesheet' href='./report.css'></link>");
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
                new Report({
                    time : report.reporttime,
                    title : report.name,
                    data : report.reportdata,
                }).openLayer();
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
/**
 * @description send username and password to server
 */
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

/**
 * @param options 弹窗基本配置信息
 * @constructor 构造方法
 */
/**
 * @description create report object
 * @param {object} options 
 */
 function Report(options) {
    this.options = options ;
}

/**
 * 打开弹窗
 */
Report.prototype.openLayer = function () {
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
    // 打印节点
    var span_print = document.createElement("span");
    span_print.setAttribute("class", "span-close");
    span_print.onclick = function () { 
        PrintDiv("reprort-table");
    };
    var span_print_report_layer = document.createTextNode("打印");
    span_print.appendChild(span_print_report_layer);
    div_toolBar.appendChild(span_print);
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
    open_layer.appendChild(reportLayer(this.options.data, this.options.time));

    // open_layer, background_layer添加到网页中
    document.body.appendChild(open_layer);
    document.body.appendChild(background_layer);
    // 在调用MyLayer实例的openLayer方法时显示弹框
    open_layer.style.display = "block" ;
    background_layer.style.display = "block";
};
/**
 * @description print report window
 * @param {string} ID table id
 */
function PrintDiv(ID) {
    var needPrint = document.getElementById(ID);
    var win = window.open("");
    win.document.write('<html><head><link href="index.css" rel="stylesheet" />'
        + '<link href="main.css" rel="stylesheet" />'
        + '<link href="report.css" rel="stylesheet" /></head><body>'
        + needPrint.outerHTML + '</body>'
        + "</html>");
    win.document.close();
    //Chrome
    if (navigator.userAgent.indexOf("Chrome") != -1) {
        win.onload = function () {
            win.document.execCommand('print');
            win.close();
        }
    }
    //Firefox
    else {
        win.print();
        win.close();
    }
}
/**
 * @description report word window
 * @param {jsonString} reportdata 
 * @param {Date} time 
 * @returns html div
 */
function reportLayer(reportdata, time) {
    try {
        var data = JSON.parse(reportdata);
        var div_report = document.createElement("div");
        div_report.setAttribute("class", "report-div");
        div_report.setAttribute("id", "report-layer");
        // info table
        var table_info = document.createElement("table");
        table_info.setAttribute("class", "report-table");
        table_info.setAttribute("id", "reprort-table");
        div_report.appendChild(table_info);

        var tbody_info = document.createElement("tbody");
        tbody_info.setAttribute("class", "report-tbody");
        table_info.appendChild(tbody_info);
        var patientInfo = data.patientInfo;
        // base data
        var bodyInfo = {
            age : patientInfo.age,
            height : patientInfo.height,
            weight : patientInfo.weight,
            sex : (patientInfo.sex == "男" ? 0 : 1),
            hb : patientInfo.hb
        }
        // place info
        var place = data.place;
        var tr_place = document.createElement("tr");
        tr_place.style.fontWeight = "bold";
        tbody_info.appendChild(tr_place);
        var td_primary_place = document.createElement("td");
        td_primary_place.setAttribute("class", "report-td");
        td_primary_place.textContent = "医院：" + place.primaryPlace;
        tr_place.appendChild(td_primary_place);
        var td_secondary_palce = document.createElement("td");
        td_secondary_palce.setAttribute("class", "report-td");
        td_secondary_palce.textContent = "科室：" + place.secondaryPlace;
        tr_place.appendChild(td_secondary_palce);
        var td_inspector = document.createElement("td");
        td_inspector.setAttribute("class", "report-td");
        td_inspector.textContent = "检查人员：" + place.inspector;
        tr_place.appendChild(td_inspector);
        // row 0 patient info
        var tr_0 = document.createElement("tr");
        tbody_info.appendChild(tr_0);
        // 0, 0
        var td_name = document.createElement("td");
        td_name.setAttribute("class", "report-td");
        td_name.textContent = "姓名：" + patientInfo.patientName;
        tr_0.appendChild(td_name);
        // 0, 1
        var td_sex = document.createElement("td");
        td_sex.setAttribute("class", "report-td");
        td_sex.textContent = "性别：" + patientInfo.sex;
        tr_0.appendChild(td_sex);
        // 0, 2
        var td_age = document.createElement("td");
        td_age.setAttribute("class", "report-td");
        td_age.textContent = "年龄：" + bodyInfo.age + " 岁";
        tr_0.appendChild(td_age);
        // row 1
        var tr_1 = document.createElement("tr");
        tbody_info.appendChild(tr_1);
        // 1, 0
        var td_height = document.createElement("td");
        td_height.setAttribute("class", "report-td");
        td_height.textContent = "身高：" + bodyInfo.height + " cm";
        tr_1.appendChild(td_height);
        // 1, 1
        var td_weight = document.createElement("td");
        td_weight.setAttribute("class", "report-td");
        td_weight.textContent = "体重：" + bodyInfo.weight + " kg";
        tr_1.appendChild(td_weight);
        // 1, 2
        var td_bsa = document.createElement("td");
        td_bsa.setAttribute("class", "report-td");
        var bsa = cBsa(patientInfo.height, patientInfo.weight);
        td_bsa.textContent = "体表面积：" + bsa.toFixed(2) + " ㎡";
        tr_1.appendChild(td_bsa);
        // row 2
        var tr_2 = document.createElement("tr");
        tbody_info.appendChild(tr_2);
        // 2, 0
        var td_hb = document.createElement("td");
        td_hb.setAttribute("class", "report-td");
        td_hb.textContent = "血红蛋白：" + bodyInfo.hb + " g/dl";
        tr_2.appendChild(td_hb);
        // 2, 1
        var td_number = document.createElement("td");
        td_number.setAttribute("class", "report-td");
        td_number.colSpan = "2";
        td_number.textContent = "病历号：" + patientInfo.medicalRecordNumber;
        tr_2.appendChild(td_number);
        // row 3
        var tr_3 = document.createElement("tr");
        tbody_info.appendChild(tr_3);
        var td_time = document.createElement("td");
        td_time.setAttribute("class", "report-td");
        td_time.colSpan = "3";
        td_time.textContent = "报告时间：" + time;
        tr_3.appendChild(td_time);

        // report table
        reportTable(data.position, bodyInfo, data.reportConclusion, table_info);
        
        return div_report;
    } catch (error) {
        console.error(error);
    }
}
/**
 * 
 * @param {JSON} position 
 * @param {Object} bodyInfo 
 * @param {string} reportConclusion 
 * @param {html table} table 
 * @returns 
 */
function reportTable(position, bodyInfo, reportConclusion, table) {
    var fdata = position[0].data;
    var tbody = document.createElement("tbody");
    tbody.setAttribute("class", "report-tbody");
    tbody.setAttribute("id", "main-data-tbody");
    table.appendChild(tbody);
    // 表头
    var tr_header = document.createElement("tr");
    tr_header.setAttribute("class", "report-tr");
    tbody.appendChild(tr_header);
    var th_arg = document.createElement("th");
    th_arg.setAttribute("class", "report-th");
    th_arg.colSpan = "2";
    th_arg.textContent = "参数";
    tr_header.appendChild(th_arg);
    var th_fp = document.createElement("th");
    th_fp.setAttribute("class", "report-th");
    th_fp.textContent = positionCn(fdata[getValue(DataType.Pos)]);
    tr_header.appendChild(th_fp);
    try {
        var sdata = position[1].data;
        var th_sp = document.createElement("th");
        th_sp.setAttribute("class", "report-th");
        th_sp.textContent = positionCn(sdata[getValue(DataType.Pos)]);
        tr_header.appendChild(th_sp);
    } catch (error) {
        
    }
    var th_range = document.createElement("th");
    th_range.setAttribute("class", "report-th");
    th_range.textContent = "正常值范围";
    tr_header.appendChild(th_range);
    var th_unit = document.createElement("th");
    th_unit.setAttribute("class", "report-th");
    th_unit.textContent = "单位";
    tr_header.appendChild(th_unit);
    // 内容
    // 第一体位数据
    var fpd = positionData(bodyInfo, fdata);
    var arr_co = [fpd.co, fpd.ci, fpd.do2, fpd.sv, fpd.si, fpd.hrv];
    var arr_tfc = [fpd.tfc, fpd.edi, fpd.svv, fpd.vol];
    var arr_svr = [fpd.svr, fpd.ssvr, fpd.ssvri, fpd.svri, fpd.vas];
    var arr_pep = [fpd.pep, fpd.lvet, fpd.lsw, fpd.lswi, fpd.lcw, fpd.lcwi, fpd.str, fpd.epci, fpd.isi, fpd.ino];
    var arr_hr = [fpd.hr, fpd.sbp, fpd.dbp, fpd.map];
    // 第二体位数据
    try {
        let spd = positionData(bodyInfo, sdata);
        arr_co.push(spd.co, spd.ci, spd.do2, spd.sv, spd.si, spd.hrv);
        arr_tfc.push(spd.tfc, spd.edi, spd.svv, spd.vol);
        arr_svr.push(spd.svr, spd.ssvr, spd.ssvri, spd.svri, spd.vas);
        arr_pep.push(spd.pep, spd.lvet, spd.lsw, spd.lswi, spd.lcw, spd.lcwi, spd.str, spd.epci, spd.isi, spd.ino);
        arr_hr.push(spd.hr, spd.sbp, spd.dbp, spd.map);
    } catch (error) {

    }
    getTableRowElement(
        tbody,
        "心排量", 
        [DataType.Co, DataType.Ci, DataType.Do2, DataType.Sv, DataType.Si, DataType.Hrv], 
        arr_co
    );
    getTableRowElement(
        tbody,
        "前负荷容量", 
        [DataType.Tfc, DataType.Edi, DataType.Svv, DataType.Vol], 
        arr_tfc
    );
    getTableRowElement(
        tbody,
        "后负荷阻力", 
        [DataType.Svr, DataType.Ssvr, DataType.Ssvri, DataType.Svri, DataType.Vas], 
        arr_svr
    );
    getTableRowElement(
        tbody,
        "心肌收缩力动力", 
        [DataType.Pep, DataType.Lvet, DataType.Lsw, DataType.Lswi, DataType.Lcw, DataType.Lcwi, DataType.Str, DataType.Epci, DataType.Isi, DataType.Ino], 
        arr_pep
    );
    getTableRowElement(
        tbody,
        "常规", 
        [DataType.Hr, DataType.Sbp, DataType.Dbp, DataType.Map], 
        arr_hr
    );
    // reportConclusion
    var tr_conclusion = document.createElement("tr");
    tbody.appendChild(tr_conclusion);
    var td_conclusion = document.createElement("td");
    td_conclusion.setAttribute("class", "report-td");
    td_conclusion.colSpan = "6";
    tr_conclusion.appendChild(td_conclusion);
    var div_conclusion = document.createElement("div");
    div_conclusion.setAttribute("class", "textarea-box");
    div_conclusion.setAttribute("contentEditable", "true");
    div_conclusion.setAttribute("id", "reportConclusion");
    div_conclusion.textContent = reportConclusion;
    td_conclusion.appendChild(div_conclusion);

    return table;
}
/**
 * @description return position chinese name
 * @param {int} pos 
 * @returns 
 */
function positionCn(pos) {
    if (pos == 1) {
        return "半卧";
    }
    else if (pos == 2) {
        return "平躺";
    }
    else if (pos == 3) {
        return "抬腿";
    }
    return "未知";
}
/**
 * 
 * @param {html tbody} tbody 
 * @param {string} content data group
 * @param {object} type data type
 * @param {object} data data value
 */
function getTableRowElement(tbody, content, type, data) {
    var dtype = createDataType(DataType);
    // 数据分组
    var tr_group = document.createElement("tr");
    tr_group.setAttribute("class", "report-tr");
    var th_group = document.createElement("th");
    th_group.setAttribute("class", "report-th");
    th_group.style.writingMode = "tb-rl";
    th_group.rowSpan = (type.length + 1).toString();
    th_group.textContent = content;
    tr_group.appendChild(th_group);
    tbody.appendChild(tr_group);

    for(let index = 0; index < type.length; ++index) {
        let tr = document.createElement("tr");
        tr.setAttribute("class", "report-tr");
        tbody.appendChild(tr);
        let key = type[index];
        // 参数名称
        let td_arg = document.createElement("td");
        td_arg.setAttribute("class", "report-td");
        td_arg.textContent = dtype.getCn(key) + "/" + dtype.getEn(key);
        tr.appendChild(td_arg);
        // 第一体位
        let td_fd = document.createElement("td");
        td_fd.setAttribute("class", "report-td");
        td_fd.textContent = actualValue(data[index].toFixed(dtype.getFixed(key)));
        tr.appendChild(td_fd);
        // 第二体位
        if (type.length*2 == data.length) {
            let td_sd = document.createElement("td");
            td_sd.setAttribute("class", "report-td");
            td_sd.textContent = actualValue(data[type.length + index].toFixed(dtype.getFixed(key)));
            tr.appendChild(td_sd)
        }
        // 正常值范围
        let td_range = document.createElement("td");
        td_range.setAttribute("class", "report-td");
        if (dtype.getMin(key) != dtype.getMax(key)) {
            td_range.textContent = dtype.getMin(key) + "~" + dtype.getMax(key);
        }
        else {
            td_range.textContent = "-";
        }
        tr.appendChild(td_range);
        // 单位
        let td_unit = document.createElement("td");
        td_unit.setAttribute("class", "report-td");
        td_unit.textContent = dtype.getUnit(key);
        tr.appendChild(td_unit);
    }
}
/**
 * @description check value
 * @param {double} value 
 * @returns 
 */
function actualValue(value) {
    if (valueInvalid(value)) {
        return "-";
    }
    return value;
}
/**
 * @description return position data object
 * @param {object} bodyInfo 
 * @param {object} pdata 
 * @returns 
 */
function positionData(bodyInfo, pdata) {
    // 身体数据
    var age = bodyInfo.age;
    var height = bodyInfo.height;
    var weight = bodyInfo.weight;
    var sex = bodyInfo.sex;
    var hb = bodyInfo.hb;
    // 血压
    var sbp = pdata[getValue(DataType.Sbp)];
    var dbp = pdata[getValue(DataType.Dbp)];
    var lap = pdata[getValue(DataType.Lap)];
    var cvp = pdata[getValue(DataType.Cvp)];
    // 计算值
    var bsa = cBsa(height, weight);
    var vept = cVept(height, weight, sex);
    var idealW = cIdealW(height, sex);
    var map = cMap(sbp, dbp);

    var hr = cHr(pdata[getValue(DataType.Hr)]);
    var lvet = cVet(pdata[getValue(DataType.Vet)]);
    var pep = cPep(pdata[getValue(DataType.Pep)]);
    var str = cStr(pep, lvet);
    var tfc = cTfc(pdata[getValue(DataType.Tfc)]);
    var epci = cEpci(pdata[getValue(DataType.Epci)]);
    var isi = cIsi(pdata[getValue(DataType.Isi)]);
    var ino = cIno(isi, sex, age);
    var ef = cVet(pdata[getValue(DataType.Ef)]);
    var si = cSi(pdata[getValue(DataType.Si)], bsa, vept);
    var sv = cSv(si, bsa);
    var edi = cEdi(si, ef);
    var lsw = cLsw(sv, map, lap);
    var lswi = cLswi(si, map, lap);
    var vol = cVol(lswi, ino);
    var ssvr = cSsvr(sv, map, cvp);
    var ssvri = cSsvri(si, map, cvp);
    var vas = cVas(ssvri);
    var svv = cSvv(sv, eval(pdata[getValue(DataType.Svv)]));
    var ci = cCi(pdata[getValue(DataType.Ci)], bsa, vept);
    var co = cCo(ci, bsa);
    var hrv = cHrv(ci);
    var svr = cSvr(co, map, cvp);
    var svri = cSvri(ci, map, cvp);
    var lcw = cLcw(co, map, lap);
    var lcwi = cLcwi(ci, map, lap);
    var do2 = cDo2(co, hb);
    const pdataObject = {
        sbp : sbp,
        dbp : dbp,
        lap : lap,
        cvp : cvp,
        bsa : bsa,
        vept : vept,
        idealW : idealW,
        map : map,
        hr : hr,
        lvet : lvet,
        pep : pep,
        str : str,
        tfc : tfc,
        epci : epci,
        isi : isi,
        ino : ino,
        ef : ef,
        si : si,
        sv : sv,
        edi : edi,
        lsw : lsw,
        lswi : lswi,
        vol : vol,
        ssvr : ssvr,
        ssvri : ssvri,
        vas : vas,
        svv : svv,
        ci : ci,
        co : co,
        hrv : hrv,
        svr : svr,
        svri : svri,
        lcw : lcw,
        lcwi : lcwi,
        do2 : do2
    }
    return pdataObject;
}

/**************************************************************************/
/**
 * @description data type; enum name : [enum value, chinese, english, unit, min, max, fixed]
 */
const DataType = {
    Hr :    [0, "心率", "HR", "bpm", 60, 100, 0], 
    Vet :   [1, "左室射血时间", "LVET", "ms", 250, 350, 0],  
    Pep :   [2, "射血前期", "PEP", "ms", 50, 120, 0],  
    Tfc :   [3, "胸液传导性", "TFC", "1/Ω", 0.025, 0.045, 3],  
    Epci :  [4, "射血期收缩指数", "EPCI", "1/s", 0.038, 0.076, 3],  
    Isi :   [5, "变力状态指数", "ISI", "1/s²", 0.90, 1.70, 2], 
    Ef :    [6, "射血分数", "EF", "%", 50, 70, 0], 
    Si :    [7, "心搏指数", "SI", "mL/beat·m²", 35, 65, 0], 
    Ci :    [8, "心脏指数", "CI", "L/min·m²", 2.0, 5.0, 1], 
    Rr :    [9, "呼吸频率", "RR", "breaths/min", 12, 24, 0], 
    Beep :  [10], 
    Reserved: [15],
    Co :    [16, "心输出量", "CO", "L/min", 3.5, 9.0, 1], 
    Sv :    [17, "心搏量", "SV", "mL/beat", 50, 120, 1], 
    Hrv :   [18, "心率变异性", "HRV", "%", -50, 50, 0], 
    Edi :   [19, "舒张末期指数", "EDI", "mL/beat·m²", 54, 130, 0], 
    Vol :   [20, "血管容积", "Vol", "%", -50, 50, 0], 
    Svr :   [21, "系统阻力", "SVR", "dyn·s·m²/cm^5", 489, 2262, 0], 
    Ssvr :  [22, "每搏外周阻力", "SSVR", "dyn·s/cm^5", 119.6, 429.2, 1], 
    Ssvri : [23, "每搏外周阻力指数", "SSVRI", "dyn·s/cm^5/beat/m²", 99.7, 185.1, 1], 
    Svri :  [24, "系统阻力指数", "SVRI", "dyn·s·m²/cm^5", 1056, 4000, 0], 
    Vas :   [25, "血管顺应性", "Vas", "%", -50, 50, 0], 
    Lvet :  [26, "左室射血时间", "LVET", "ms", 250, 350, 0], 
    Lsw :   [27, "左心室每搏做功", "LSW", "g·m", 20, 90, 0], 
    Lswi :  [28, "每搏做功指数", "LSWI", "g·m/beat/m²", 39.3, 73.0, 1],
    Lcw :   [29, "左心室做功", "LCW", "kg·m", 5.4, 10.0, 1], 
    Lcwi :  [30, "左心室做功指数", "LCWI", "kg·m/m²", 1.81, 7.06, 2], 
    Str :   [31, "收缩时间比", "STR", "-", 0, 0.4, 1], 
    Ino :   [32, "收缩变力性", "Ino", "%", -50, 50, 0], 
    Sbp :   [33, "收缩压", "SBP", "mmHg", 90, 140, 0], 
    Dbp :   [34, "舒张压", "DBP", "mmHg", 60, 90, 0], 
    Map :   [35, "平均动脉压", "MAP", "mmHg", 70, 105, 0], 
    Cvp :   [36, "中心静脉压", "CVP", "mmHg", 2, 6, 0], 
    Lap :   [37, "左房压", "LAP", "mmHg", 6, 12, 0], 
    Dz :    [38], 
    Pos :   [39], 
    Do2 :   [40, "氧输送", "DO2", "mL/min", 1000, 1000, 0], 
    Svv :   [41, "每搏输出变异性", "SVV", "%", 0, 10, 0],
};
/**
 * @description create enum class 
 * @param {object} dataType 
 * @returns data type enum class
 */
function createDataType(dataType) {
    const keyMap = {};
    const cnMap = {};
    const enMap = {};
    const unitMap = {};
    const minMap = {};
    const maxMap = {};
    const fixedMap = {};
    for (const key of Object.keys(dataType)) {
        const [value, cn, en, unit, min, max, fixed] = dataType[key];
        keyMap[value] = key;
        cnMap[value] = cn;
        enMap[value] = en;
        unitMap[value] = unit;
        minMap[value] = min;
        maxMap[value] = max;
        fixedMap[value] = fixed;
    }
    return {
        getKey(value) {
            return keyMap[value] || keyMap[value[0]];
        },
        getCn(value) {
            return cnMap[value] || cnMap[value[0]];
        },
        getEn(value) {
            return enMap[value] || enMap[value[0]];
        },
        getUnit(value) {
            return unitMap[value] || unitMap[value[0]];
        },
        getMin(value) {
            return minMap[value] || minMap[value[0]];
        },
        getMax(value) {
            return maxMap[value] || maxMap[value[0]];
        },
        getFixed(value) {
            return fixedMap[value] || fixedMap[value[0]];
        }
    }
}
/**
 * 
 * @param {DataType} key 
 * @returns enum value
 */
function getValue(key) {
    return key[0];
}
/**
 * @description remove min and max value
 * @param {array} arry 
 * @returns 
 */
function spliceMaxMin (arry){
    let result = arry.splice(0)
    let max = Math.max(...result);
    let min = Math.min(...result);
    for(var i = 0; i < result.length;i++){
        if(result[i] == max){
            result.splice(i,1);
        }
        if(result[i] ==min){
            result.splice(i,1);
        }
    }
    return result;
}
/**
 * @description like qt qPow(x, y)
 * @param {double} x 
 * @param {double} y 
 * @returns {double} pow
 */
function qPow(x, y) {
    return Math.pow(x, y);
}
/**
 * @description like qt qAbs(x)
 * @param {double} x 
 * @returns 
 */
function qAbs(x) {
    return Math.abs(x);
}
/**
 * @description invalid value
 * @returns 
 */
function invalid()
{
    return 0xFFFFFFFF;
}
/**
 * @description check hr and return
 * @param {int} value 
 * @returns 
 */
function cHr(value)
{
    return checkValue(value);
}

function cVet(value)
{
    return checkValue(value);
}

function cPep(value)
{
    return checkValue(value);
}

function cStr(pep, vet)
{
    return (isInvalid(pep, vet) ? invalid() : pep/vet);
}

function cTfc(value)
{
    return checkValue(value/1000.0);
}

function cEpci(value)
{
    return checkValue(value/1000.0);
}

function cIsi(value)
{
    return checkValue(value/100.0);
}

function cIno(isi, sex, age)
{
    return percent(isi, age < 70 ? (sex == 1 ? 1.35 : 1.15) : (sex == 1 ? 1.20 : 1.10));
}

function cEf(value)
{
    return checkValue(value);
}

function cSi(value, bsa, vept)
{
    return (isInvalid(value, bsa) ? invalid() : 2/bsa*vept/6800*value);
}

function cSv(si, bsa)
{
    return (isInvalid(si, bsa) ? invalid() : si*bsa);
}

function cEdi(si, ef)
{
    return (isInvalid(si, ef) ? invalid() : si/ef*100);
}

function cLsw(sv, map, lap)
{
    return (isInvalid(sv, map) ? invalid() : 0.0144*sv*(map - lap));
}

function cLswi(si, map, lap)
{
    return (isInvalid(si, map) ? invalid() : 0.0144*si*(map - lap));
}

function cVol(lswi, ino)
{
    return (valueInvalid(lswi) || invalid() == ino ? invalid() : percent(lswi, 52.8) - ino);
}

function cSsvr(sv, map, cvp)
{
    return (isInvalid(sv, map) ? invalid() : 80*(map - cvp)/sv);
}

function cSsvri(si, map, cvp)
{
    return (isInvalid(si, map) ? invalid() : 80*(map - cvp)/si);
}

function cVas(ssvri)
{
    return (valueInvalid(ssvri) ? invalid() : percent(ssvri, 137.8));
}

function cSvv(sv, svList) 
{
    if (!(svList instanceof Array) || svList.length < 6 || valueInvalid(sv)) return invalid();
    var svAvg = spliceMaxMin(svList).reduce((sum, index) => sum + index, 0);
    return qAbs(sv - svAvg)/svAvg;
}

function cCi(value, bsa, vept)
{
    return (isInvalid(value, bsa) ? invalid() : 2/bsa*vept/6800*(value/10.0));
}

function cCo(ci, bsa)
{
    return (isInvalid(ci, bsa) ? invalid() : ci*bsa);
}

function cHrv(ci)
{
    return percent(ci, 4.5);
}

function cSvr(co, map, cvp)
{
    return (isInvalid(co, map) ? invalid() : (map - cvp)/co*80);
}

function cSvri(ci, map, cvp)
{
    return (isInvalid(ci, map) ? invalid() : 80*(map - cvp)/ci);
}

function cLcw(co, map, lap)
{
    return (isInvalid(co, map) ? invalid() : 0.0144*(map - lap - 2)*co);
}

function cLcwi(ci, map, lap)
{
    return (isInvalid(ci, map) ? invalid() : 0.0144*(map - lap)*ci);
}

function cDo2(co, hb)
{
    return (valueInvalid(co) ? invalid() : 1.34*hb*co*10);
}

function cRr(value)
{
    return checkValue(value/10);
}

function cBsa(height, weight)
{
    return (qPow(weight, 0.425)*qPow(height, 0.725)*0.007184);
}

function cVept(height, weight, sex)
{
    return ((qPow(0.17*height, 3)/4.25)*(1 + 0.65*(weight/(0.524*height - (sex == 0 ? 16.58 : 26.58)) - 1)));
}

function cIdealW(height, sex)
{
    return (0.524*height - (sex == 0 ? 16.58 : 26.58));
}

function cMap(sbp, dbp)
{
    return ((sbp + dbp*2)/3);
}

function percent(value, ideal)
{
    return (valueInvalid(value) ? invalid() : perPNJ(value, ideal));
}

function perPNJ(value1, value2)
{
    return (value1 >= value2 ? (value1/value2 - 1)*100 : (1 - value2/value1)*100);
}

function checkValue(value)
{
    return (0 != value ? value : invalid());
}

function valueInvalid(value)
{
    return ((0 == value || invalid() == value) ? true : false);
}

function isInvalid(value1, value2)
{
    return (valueInvalid(value1) || valueInvalid(value2));
}