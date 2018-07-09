function getQueryString(name)
{
    var reg = new RegExp('(^|&)' + name + '=([^&]*)(&|$)', 'i');
    var r = window.location.search.substr(1).match(reg);
    if (r != null)
    {
        return unescape(r[2]);
    }
    return "";
}

function CheckRefresh()
{
    var token = getQueryString("token");
    var sj = 0 + getQueryString("sj");
    var nowsj = (new Date()).valueOf();
    if(( nowsj - sj ) > 5000)
    {
        window.location.href = window.location.pathname + "?token=" + token + "&sj=" + nowsj;
    }
}
CheckRefresh();