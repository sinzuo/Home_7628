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
			data=JSON.parse(data);
			if(data.timeout == undefined||data.timeout == null||data.timeout == 0){
				gotoLogin();
			}else{
				
				if(location.href.indexOf("login.html") != -1){
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
	gotoLogin();
}

function gotoLogin(){
	//判断当前页面是不是login页面，不是则跳转
	if(location.href.indexOf("login.html") == -1){
		window.location.href="login.html";
	}
}
function gotoIndex(token){
	//判断当前页面是不是index页面不是则跳转
	if(location.href.indexOf("index.html") == -1){
		if(undefined != token && "" != token){
			var exp = new Date(); 
			exp.setTime(exp.getTime() + 10*60*1000);
			document.cookie="name="+escape(token)+";expires=0";
			window.location.href="index.html?token="+escape(token)+"&sj="+Math.random();
		}else{
			window.location.href="index.html?sj="+Math.random();
		}
	}
}

