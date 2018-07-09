//js document
var checkLogin = false;
//获取url参数值
function getQueryStringByName(name){
	var result = location.search.match(new RegExp("[\?\&]" + name+ "=([^\&]+)","i"));
	if(result == null || result.length < 1){
		return "";
	}
	return result[1];
}

var token = $.cookie("token");//获取cookie中的token
if(getQueryStringByName("token")!=""){//避免浏览器不支持cookie导致无法获取token
	token = getQueryStringByName("token");
}
if(undefined != token && "" != token){
	$.ajax({
		type: "GET", 
		async :true,
		url: 'http://'+window.location.host+'/cgi-bin/luci/admin/timeout_get?token='+token,
		success: function(data,status){
			data = JSON.parse(data);
			if(data.timeout == undefined||data.timeout == null||data.timeout == 0){
				app_gotoLogin();
			}else{
				if(location.href.indexOf("app_lg.html") != -1){
					gotoIndex(token);
				}
			}
		},
		error: function(jqXHR, textStatus,errorThrown){
			//alert("请求失败，稍后请重试！");
		},
		complete: function(){
			checkLogin = true;
		}
	});
}else{
	checkLogin = true;
	app_gotoLogin();
}

function app_gotoLogin(){
	//判断当前页面是不是login页面，不是则跳转
	if(location.href.indexOf("app_lg.html") == -1){
		window.location.href="app_lg.html";
	}
}
function app_gotoIndex(token){
	//判断当前页面是不是index页面不是则跳转
	if(location.href.indexOf("index.html") == -1){
		if(undefined != token && "" != token){
			window.location.href="phone.html?token="+token;
		}else{
			window.location.href="phone.html";
		}
	}
}

