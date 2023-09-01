/**
 * @description data type; enum name : [enum value, chinese, english, unit, min, max, fixed]
 */
export const DataType = {
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
  Svr :   [21, "系统阻力", "SVR", "dyn·s·m²/cm^5", 742, 1500, 0], 
  Ssvr :  [22, "每搏外周阻力", "SSVR", "dyn·s/cm^5", 119.6, 429.2, 1], 
  Ssvri : [23, "每搏外周阻力指数", "SSVRI", "dyn·s/cm^5/beat/m²", 99.7, 185.1, 1], 
  Svri :  [24, "系统阻力指数", "SVRI", "dyn·s·m²/cm^5", 1337, 2483, 0], 
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
  Do2 :   [40, "氧输送", "DO₂", "mL/min", 1000, 1000, 0], 
  Svv :   [41, "每搏输出变异性", "SVV", "%", 0, 10, 0],
};
/**
 * @description create enum class 
 * @param {object} dataType 
 * @returns data type enum class
 */
export function createDataType(dataType) {
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
  for(const i = 0; i < result.length;i++){
  if(result[i] == max){
    result.splice(i,1);
  }
  if(result[i] == min){
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
  const svAvg = spliceMaxMin(svList).reduce((sum, index) => sum + index, 0);
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

export function cBsa(height, weight)
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
/**
 * @description check value
 * @param {double} value 
 * @returns 
 */
function actualValue(value) {
  if (valueInvalid(value) || isNaN(value)) {
    return "-";
  }
  return value;
}
export function intercept(value, n) {
  return Math.floor(value*qPow(10, n))/qPow(10, n)
}
/**
* @description return position data object
* @param {object} bodyInfo 
* @param {object} pdata 
* @returns 
*/
export function positionData(bodyInfo, pdata) {
  if (pdata.length == 0) {
    return {};
  }
  const dt = createDataType(DataType);
  // 身体数据
  const age = bodyInfo.age;
  const height = bodyInfo.height;
  const weight = bodyInfo.weight;
  const sex = bodyInfo.sex === '男' ? 0 : 1;
  const hb = bodyInfo.hb;
  // 血压
  const sbp = pdata[getValue(DataType.Sbp)];
  const dbp = pdata[getValue(DataType.Dbp)];
  const lap = pdata[getValue(DataType.Lap)];
  const cvp = pdata[getValue(DataType.Cvp)];
  // 计算值
  const bsa = intercept(cBsa(height, weight), 2);
  const vept = intercept(cVept(height, weight, sex), 6);
  const idealW = intercept(cIdealW(height, sex), 6);
  const map = intercept(cMap(sbp, dbp), dt.getFixed(DataType.Map));

  const hr = intercept(cHr(pdata[getValue(DataType.Hr)]), dt.getFixed(DataType.Hr));
  const lvet = intercept(cVet(pdata[getValue(DataType.Vet)]), dt.getFixed(DataType.Vet));
  const pep = intercept(cPep(pdata[getValue(DataType.Pep)]), dt.getFixed(DataType.Pep));
  const str = intercept(cStr(pep, lvet), dt.getFixed(DataType.Str));
  const tfc = intercept(cTfc(pdata[getValue(DataType.Tfc)]), dt.getFixed(DataType.Tfc));
  const epci = intercept(cEpci(pdata[getValue(DataType.Epci)]), dt.getFixed(DataType.Epci));
  const isi = intercept(cIsi(pdata[getValue(DataType.Isi)]), dt.getFixed(DataType.Isi));
  const ino = intercept(cIno(isi, sex, age), dt.getFixed(DataType.Ino));
  const ef = intercept(cEf(pdata[getValue(DataType.Ef)]), dt.getFixed(DataType.Ef));
  const si = intercept(cSi(pdata[getValue(DataType.Si)], bsa, vept), dt.getFixed(DataType.Si));
  const sv = intercept(cSv(si, bsa), dt.getFixed(DataType.Sv));
  const edi = intercept(cEdi(si, ef), dt.getFixed(DataType.Edi));
  const lsw = intercept(cLsw(sv, map, lap), dt.getFixed(DataType.Lsw));
  const lswi = intercept(cLswi(si, map, lap), dt.getFixed(DataType.Lswi));
  const vol = intercept(cVol(lswi, ino), dt.getFixed(DataType.Vol));
  const ssvr = intercept(cSsvr(sv, map, cvp), dt.getFixed(DataType.Ssvr));
  const ssvri = intercept(cSsvri(si, map, cvp), dt.getFixed(DataType.Ssvri));
  const vas = intercept(cVas(ssvri), dt.getFixed(DataType.Vas));
  const svv = intercept(cSvv(sv, eval(pdata[getValue(DataType.Svv)])), dt.getFixed(DataType.Svv));
  const ci = intercept(cCi(pdata[getValue(DataType.Ci)], bsa, vept), dt.getFixed(DataType.Ci));
  const co = intercept(cCo(ci, bsa), dt.getFixed(DataType.Co));
  const hrv = intercept(cHrv(ci), dt.getFixed(DataType.Hrv));
  const svr = intercept(cSvr(co, map, cvp), dt.getFixed(DataType.Svr));
  const svri = intercept(cSvri(ci, map, cvp), dt.getFixed(DataType.Svri));
  const lcw = intercept(cLcw(co, map, lap), dt.getFixed(DataType.Lcw));
  const lcwi = intercept(cLcwi(ci, map, lap), dt.getFixed(DataType.Lcwi));
  const do2 = intercept(cDo2(co, hb), dt.getFixed(DataType.Do2));
  return {
    sbp : actualValue(sbp),
    dbp : actualValue(dbp),
    lap : actualValue(lap),
    cvp : actualValue(cvp),
    bsa : actualValue(bsa),
    vept : actualValue(vept),
    idealW : actualValue(idealW),
    map : actualValue(map),
    hr : actualValue(hr),
    lvet : actualValue(lvet),
    pep : actualValue(pep),
    str : actualValue(str),
    tfc : actualValue(tfc),
    epci : actualValue(epci),
    isi : actualValue(isi),
    ino : ino,
    ef : actualValue(ef),
    si : actualValue(si),
    sv : actualValue(sv),
    edi : actualValue(edi),
    lsw : actualValue(lsw),
    lswi : actualValue(lswi),
    vol : vol,
    ssvr : actualValue(ssvr),
    ssvri : actualValue(ssvri),
    vas : vas,
    svv : actualValue(svv),
    ci : actualValue(ci),
    co : actualValue(co),
    hrv : hrv,
    svr : actualValue(svr),
    svri : actualValue(svri),
    lcw : actualValue(lcw),
    lcwi : actualValue(lcwi),
    do2 : actualValue(do2)
  }
}