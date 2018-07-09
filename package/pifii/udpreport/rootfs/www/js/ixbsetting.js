//js document
var onlineModels = {'pppoe':'PPPoE拨号','static':'网线（手动配置）','dhcp':'网线（自动配置）','wifi':'无线连接'};//数组生成上网配置方式,'wifi':'无线连接'
String.prototype.trim = function () {
    return this .replace(/^\s\s*/, '' ).replace(/\s\s*$/, '' );
}
$(document).ready(function() {
	var onlineHtml = "";
	for(var attr in onlineModels){
		onlineHtml += "<dd id='"+attr+"'>"+onlineModels[attr]+"</dd>";
	}
	setInterval(initPageData,20000,false);//定时获取状态
	$(".get_onlineStyle").html(onlineHtml);
	$('dl.get_onlineStyle,dl.get_mask').hide(); //初始dl隐藏
	$(".select span").hover(function(){
		$(this).parent().find('.get_onlineStyle').slideDown(100);  //找到dl.get_onlineStyle显示
		$(this).parent().find('dd').hover(function(){$(this).addClass('hover')},function(){$(this).removeClass('hover')}); //li的hover效果
		$(this).parent().hover(function(){},function(){
			$(this).parent().find(".get_onlineStyle").slideUp(100); 
		});
	},function(){});
	$(".select dl dd").click(function(){
		$(this).parents('.select').find('span').html($(this).html());
		var value = $(this).data("value");
		if(undefined != value && "" != value){
			$(this).parents('.select').find('span').data("value",value);
		}
		$(this).parents('.select').find('dl').slideUp();
	});
	
	//获取掩码
	$(".select_mask input").hover(function(){
		$(this).parent().find('.get_mask').slideDown(100);  //找到dl.get_onlineStyle显示
		$(this).parent().find('dd').hover(function(){$(this).addClass('hover')},function(){$(this).removeClass('hover')}); //li的hover效果
		$(this).parent().hover(function(){},function(){
			$(this).parent().find(".get_mask").slideUp(100); 
		});
	},function(){});
	$(".select_mask dl dd").click(function(){
		$(this).parents('.select_mask').find('input').val($(this).html());
		var value = $(this).data("value");
		if(undefined != value && "" != value){
			$(this).parents('.select_mask').find('input').data("value",value);
		}
		$(this).parents('.select_mask').find('dl').slideUp();
	});
	
	//获取wifi
	$(".select_wifi span").hover(function(){
		$(this).parent().find('.get_wifiList').slideDown(100);  //找到dl.get_onlineStyle显示
		$(this).parent().find('dd').hover(function(){$(this).addClass('hover')},function(){$(this).removeClass('hover')}); //li的hover效果
		$(this).parent().hover(function(){},function(){
			$(this).parent().find(".get_wifiList").slideUp(100); 
		});
	},function(){});
	
	//点击上网方式获取对应的属性
	$("#pppoe").click(function(){
		$(".name,.pwd").show();
		$(".ip,.mask,.gateway,.dns,.use,.wifi,.wifipwd").hide();
		$(".dns input").val(models.pppoe.dns);
		$(".pwd input").val(models.pppoe.pass);
	});
	
	$("#static").click(function(){
		$(".ip,.mask,.gateway,.dns").show();
		$(".name,.pwd,.use,.wifi,.wifipwd").hide();
		$(".dns input").val(models.static.dns);
	});
	$("#dhcp").click(function(){
		$(".dns").show();
		$(".name,.pwd,.ip,.mask,.gateway,.use,.wifi,.wifipwd").hide();
		$(".dns input").val(models.dhcp.dns);
	});
	$("#wifi").click(function(){
		$(".wifi,.wifipwd").show();
		$(".name,.pwd,.ip,.mask,.gateway,.dns").hide();
		$("#wifiName").text(models.wifi.ssid);
		$(".wifipwd input").val(models.wifi.key);
		getWiFiList();
	});
	$(".wifi_reload").click(function(){
		getWiFiList();
	});

	resizeScreen();   
	$(window).resize(function(){
		resizeScreen();
	});
	function resizeScreen(){
		var w = $(window).width();
		var h = $(window).height();
		var runH = $(".outer .running_state").outerHeight();
		var changeH = $(".outer .change_pwd").outerHeight()+50;//50为中间值
		var wifiNameH = $(".outer .change_wifiName").outerHeight();
		$(".outer").css({"width":w + "px","height":h + "px"});
		$(".outer .menu_bar").css({"height":h-80 + "px"});
		$(".outer .show_content").css({"width":w-130 + "px","height":h-80 + "px"});
		$(".outer .show_content .content_1").css({"margin-top":(h-80-555)/2 + "px"});
		$(".outer .show_content .content_2").css({"margin-top":(h-80-400)/2 + "px"});
		$(".outer .show_content .content_3").css({"margin-top":(h-80-400)/2 + "px"});
		$(".outer .show_content .content_4").css({"margin-top":(h-80-400)/2 + "px"});
		$(".outer .running_state").css({"margin-top":-runH/2 + "px"});
		$(".outer  .change_pwd").css({"margin-top":-changeH/2 + "px"});
		$(".outer  .change_wifiName").css({"margin-top":-wifiNameH/2 + "px"});
		if($(".show_content").height() < 555){
			$(".content_1_b p").css({"top":"-40px","position":"relative"});
		}
	}
	
	//主菜单栏
	$(".outer .menu_bar ul li").click(function(){
		$(this).addClass("current").siblings(".outer .menu_bar ul li").removeClass("current");
		if($(this).index()==3){
			getNewVersion();
			if(VerCmp($(".new_version").html(),$(".current_version").html())<0){
				$(".update p").html("已是最新固件");
				$(".update p").unbind("click");
			}
		}
		$(".outer .show_content > div").stop(true,true).animate({opacity:0},100).hide().eq($(this).index()).animate({opacity:1},400).show();
		
	});
	
	//分菜单栏
	$(".outer .show_content .content_2 .content_2_menu ul > li").click(function(){
		$(this).addClass("active").siblings(".outer .show_content .content_2 .content_2_menu ul > li").removeClass("active");
		$(".outer .show_content .content_2 .content_2_list ul > li").stop(true,true).animate({opacity:0},100).hide().eq($(this).index()).animate({opacity:1},400).show();
		
	});
	/*$(".outer .show_content .content_3 .content_3_menu ul > li").click(function(){
		$(this).addClass("active").siblings(".outer .show_content .content_3 .content_3_menu ul > li").removeClass("active");
		$(".outer .show_content .content_3 .content_3_list dl > dd").stop(true,true).animate({opacity:0},100).hide().eq($(this).index()).animate({opacity:1},400).show();
		
	});*/
	$(".outer .show_content .content_4 .content_4_menu ul > li").click(function(){
		$(this).addClass("active").siblings(".outer .show_content .content_4 .content_4_menu ul > li").removeClass("active");
		$(".outer .show_content .content_4 .content_4_list dl > dd").stop(true,true).animate({opacity:0},100).hide().eq($(this).index()).animate({opacity:1},400).show();
		
	});
	
	
	//外网、无线跳转
	$(".outside_network_set").click(function(){
		$(".outer .menu_bar ul li:eq('"+$(this).index()+"')").addClass("current").siblings().removeClass("current");
		$(".outer .show_content > div").stop(true,true).animate({opacity:0},100).hide().eq($(this).index()).animate({opacity:1},400).show();
	});
	$(".network_set").click(function(){
		/*$(".outer .menu_bar ul li:eq('"+$(this).index()+"')").addClass("current").siblings().removeClass("current");
		$(".outer .show_content > div").stop(true,true).animate({opacity:0},100).hide().eq($(this).index()).animate({opacity:1},400).show();
		$(".show_content .content_2 .content_2_menu ul > li:eq(2)").addClass("active").siblings(".show_content .content_2 .content_2_menu ul > li").removeClass("active");*/
		$(".outer .change_wifiName,.transparent").show();
		getWifiName();
	});
	
	//修改名称
	$(".set_wifiName").click(function(){
		$(".outer .change_wifiName,.transparent").show();
		getWifiName();
	});
	
	//修改密码
	$(".set_wifiPwd").click(function(){
		$(".outer .change_pwd,.transparent").show();
	});
	
	//点击修改WiFi密码
	$("#set_wifiPWD").click(function(){
		$("#first_input,#second_input").slideToggle();
	});
	
	//运行状态
	$(".show_runningState").click(function(){
		$(".outer .running_state,.transparent").show();

	});
	
	//点击关闭
	$(".closed").click(function(){
		$(this).parent().parent().hide();
		$(".transparent").hide();
	});
	$(".closed_state").click(function(){
		$(this).parent().hide();
		$(".transparent").hide();
	});
	
	//点击退出
	$(".exit").click(function(){
		window.location.replace("login.html");
	});

	//设置有无密码
	$('#set_pass').change(function(){
		var p=$(this).children('option:selected').val();
		if(p=="pass_null"){
			$("#show_setpass").hide();
		}else if(p=="pass_set"){
			$("#show_setpass").show();
		}
	})

	//lanIP设置
	$(".lanconfirm").click(function(){
		var url = 'http://'+window.location.host+'/cgi-bin/luci/admin/sys_set_ipaddr';
		var valIP = /^(\d{1,2}|1\d\d|2[0-4]\d|25[0-5])\.(\d{1,2}|1\d\d|2[0-4]\d|25[0-5])\.(\d{1,2}|1\d\d|2[0-4]\d|25[0-5])\.(\d{1,2}|1\d\d|2[0-4]\d|25[0-5])$/;
		var data;
		var ip = $(".lanip input").val();

		var valdIp = valIP.exec(ip);
		//IP校验
		if (valdIp == null) {
			alert('注意IP有效性');
			$(".ip input").focus();
			return;
		}
		if (valdIp != '') {
			if (valdIp[0] != ip) {
				alert('注意IP有效性');
				$(".ip input").focus();
				return;
			}
		}
		data = {ipaddr:ip,network:"255.255.255.0"};
		$.ajax({
			type: "POST",
			async: false,
			url: url,
			data:data,
			success: function(data,status){
				alert("设置成功，请稍候重试新设置的IP地址！");
				setTimeout(function(){
					location.href = "http://"+ip;
				},10000);
			},
			error: function(jqXHR, textStatus,errorThrown){
				alert("设置网络失败，请稍后重试！");
			}
		});
	})

	//设置名称
	$(".setWifiName p").click(function(){
		var wifiNameStr1 = $("input[name='wifiName1']").val();
		var wifi_pass = $("input[name='wifi_pass']").val();
		var pass="";
		var hidden;
		if($("input[name='hiddenssid']").is(":checked")){
			hidden=1;
		}else{
			hidden=0;
		}
		if(wifiNameStr1 == ""){
			alert("请输入WiFi名称！");
			return false;
		}
		if($('#set_pass').children('option:selected').val()=="pass_set"){
		if(wifi_pass ==""){
			alert("密码没有设置！");
			return ;
		}
		if(wifi_pass !="" && wifi_pass.length <=7){
			alert("密码不少于8位！");
			$("input[name='wifi_pass']").focus();
			return;
		}
		}

		if($('#set_pass').children('option:selected').val()=="pass_null"){
			pass="N/A";
		}else{
			pass=wifi_pass;
		}
		setWifiName(hidden,wifiNameStr1,pass);//调用设置WiFi名称的方法
	});
	
	//设置密码
	$(".setPwdVal p").click(function(){
		var pwdVal_old = $("input[name='pwdVal_old']").val();
		var pwdVal1 = $("input[name='pwdVal1']").val();
		var pwdVal2 = $("input[name='pwdVal2']").val();
		var pwdVal3 = $("input[name='pwdVal3']").val();
		var pwdVal4 = $("input[name='pwdVal4']").val();
		if(pwdVal_old == ""){
			alert("请输入原登录密码！");
			return false;
		}
		if(pwdVal1 == ""){
			alert("请输入新的登录密码！");
			return false;
		}
		if(pwdVal1.length < 8){
			alert("登录密码不能小于8位！");
			return false;
		}
		if(pwdVal2 == ""){
			alert("请再次输入登录密码！");
			return false;
		}
		if(pwdVal2.length < 8){
			alert("登录密码不能小于8位！");
			return false;
		}
		if(pwdVal1 != pwdVal2){
			alert("两次输入的登录密码不同！");
			return false;
		}
                if(pwdVal_old == pwdVal2){
                        alert("新密码和原密码相同，不需要修改密码！");
                        return false;
                }		
		if($("#first_input").is(":visible") && $("#second_input").is(":visible")){//判断WiFi密码是否同时修改
			if(pwdVal3 == ""){
				alert("请输入新的WiFi密码！");
				return false;
			}
			if(pwdVal3.length < 8){
				alert("WiFi密码不能小于8位！");
				return false;
			}
			if(pwdVal4 == ""){
				alert("请再次输入WiFi密码！");
				return false;
			}
			if(pwdVal4.length < 8){
				alert("WiFi密码不能小于8位！");
				return false;
			}
			if(pwdVal3 != pwdVal4){
				alert("两次输入的WiFi密码不同！");
				return false;
			}
			changePWD(pwdVal_old,pwdVal2,pwdVal4);//调用设置密码的方法
		}else{
			changePWD(pwdVal_old,pwdVal2);//调用设置密码的方法
		}
	});
	initPageData(true);//页面加载初始状态
	getNewVersion();//获取新版本
	getDeviceArguments();
	setInterval(getDeviceArguments,60000);//获取WiFi连接设备列表
	getPortVal();//获取WiFi连接端口
	setInterval(function(){
		$.ajax({
			type: "POST",
			url: 'http://'+window.location.host+'/cgi-bin/luci/admin/wan_ip_get?token='+token,
			success: function(data,status) {
				data=JSON.parse(data);
				$(".connect_num").text(data.real.wifinum);
			}
		});
	},60000);
	$(".confirm").click(function(){
		var url = 'http://'+window.location.host+'/cgi-bin/luci/admin/wan_ip_set?token='+token;
		var data;
		if($(".name").is(":visible")){//pppoe方式
			var user = $(".name input").val();
			var pass = $(".pwd input").val();
			var dns = $(".dns input").val();
                        if (user == '') {
                                alert('pppoe账号为空，请重新输入');
                                $(".name input").focus();
                                return;
                        }
			data = {proto:'pppoe',user:user,pass:pass,dns:dns};
		}else if($(".ip").is(":visible")){//网线(手动配置)
			var valIP = /^(\d{1,2}|1\d\d|2[0-4]\d|25[0-5])\.(\d{1,2}|1\d\d|2[0-4]\d|25[0-5])\.(\d{1,2}|1\d\d|2[0-4]\d|25[0-5])\.(\d{1,2}|1\d\d|2[0-4]\d|25[0-5])$/;
			var ip = $(".ip input").val();

			var valdIp = valIP.exec(ip);
			//IP校验
			if (valdIp == null) {
				alert('注意IP有效性');
				$(".ip input").focus();
				return;
			}
			if (valdIp != '') {
				if (valdIp[0] != ip) {
					alert('注意IP有效性');
					$(".ip input").focus();
					return;
				}
			}

			var gw = $(".gateway input").val();
			//网关校验
			var valdgw = valIP.exec(gw);
			if (valdgw == null) {
				alert('注意网关有效性');
				$(".gateway input").focus();
				return;
			}
			if (valdgw != '') {
				if (valdgw[0] != gw) {
					alert('注意网关有效性');
					$(".gateway input").focus();
					return;
				}
			}
			var dns = $(".dns input").val();
			var mask = $(".select_mask input").first().val();//.data("value");
			//网关校验
			var valdmask = valIP.exec(mask);
			if (valdmask == null) {
				alert('注意子网掩码有效性');
				$(".select_mask input").focus();
				return;
			}
			if (valdmask != '') {
				if (valdmask[0] != mask) {
					alert('注意子网掩码有效性');
					$(".select_mask input").focus();
					return;
				}
			}

			data = {proto:'static',ip:ip,gw:gw,dns:dns,mask:mask};
		}else if($(".dns").is(":visible")){//网线(自动配置)
			var dns = $(".dns input").val();
			//url = 'http://'+window.location.host+'/cgi-bin/luci/api/0/module/wan_ip_set?token='+token;
			data = {proto:'dhcp',dns:dns};
		}else if($(".wifi").is(":visible")){//无线连接
			var selectedWifi = $(".select_wifi span").first();
			var ssid = selectedWifi.text();
			var encryption = selectedWifi.data("encryption");
			var key = $(".wifipwd input").val();
			url = 'http://'+window.location.host+'/cgi-bin/luci/admin/wan_relay_set?token='+token;
			if(encryption==0){
				data = {ssid:ssid};
			}else{
				if ( key.length < 8)
				{
					alert('请输入密码大于等于8个字符');
					$(".pwd input").focus();
				    return;
				}

				data = {ssid:ssid,key:key};
			}


			var dd=$(".get_wifiList dd");
			var wifinum=0;
			$.each(dd,function(){
				if($(this).html()==$(".select_wifi span").html()){
					wifinum=1;
				}
			})
			if(wifinum != 1){
				alert("设置失败，wifi名称没有搜索到，请重新刷新！");
				$(".select_wifi").focus();
				return false;
			}


		}
		$.ajax({
			type: "POST",
			async: false,
			url: url,
			data:data,
			success: function(data,status){
				alert("设置配置成功！");
				$(".confirm").unbind("click");
				initPageData(false);
				setTimeout(function(){
					location.href="index.html?token="+token+"&sj="+Math.random();
				},5000);

			},
			error: function(jqXHR, textStatus,errorThrown){
				alert("设置网络失败，请稍后重试！");
			}
		});
	});
	
	//获取新的版本
	$(".refresh").click(function(){
		getNewVersion();	
	});
	
	//升级新版本
	$(".update p").click(function(){
		if($(this).attr("data-clicked") == "0"){
			if(version.swver == ""){
				return false;
			}else if(version.swver != version.curversion){
				updateVersion();
				$(this).attr("data-clicked","1");
			}
		}else{
			return false;
		}
	});
	
	//恢复出厂设置
	$(".recover p").click(function(){
		if($(this).attr("data-clicked") == "0"){
			factoryReset();
			$(this).attr("data-clicked","1");
		}else{
			return false;
		}
	});
	
	//重启路由器
	$(".reboot p").click(function(){
		if($(this).attr("data-clicked") == "0"){
			reboot();
			$(this).attr("data-clicked","1");
		}else{
			return false;
		}
	});
});



var isFirst = true;
//初始状态
var models;
function initPageData(refresh){
	$.ajax({
		type: "POST",
		url: 'http://'+window.location.host+'/cgi-bin/luci/admin/wan_ip_get?token='+token,
		success: function(data,status){
			data=JSON.parse(data);
			models = data;
			$("#connectWay").text(onlineModels[data.wantype]);
			$(".wtype").text("外网模式："+onlineModels[data.wantype]);
			if(data.real.ip == ""){
				$("#idAddress").text("连接重试中....");
				$(".wsucc").text("连接状态：连接重试中....");
			}else{
				$(".wsucc").text("连接状态：已连接");
				$("#idAddress").text(data.real.ip+" / "+data.real.mask);

				if(data.pppoe.reg == "0"&&data.real.httpreg.trim() == "0" ){
					if(isFirst){
						$.post("http://" + window.location.host + "/cgi-bin/luci/admin/http_set_register", {});

						var pppoe_username = data.pppoe.user;
						var regUrl = data.real.regUrl;
						var acMac = data.real.mac;
						var actUrl = data.real.actUrl;
						var factory = data.real.dHard;
						var model = data.real.dName;
						document.getElementById("register_iframe").src = "http://117.139.108.10:8888/register.html" + "?t=" + Date.parse(new Date()) + "&pppoe_username=" + pppoe_username + "&acmac=" + acMac + "&ac_active_address=" + actUrl + "&factory=" + factory + "&model=" + model;

						isFirst = false;
					}
				} else {
					//showInfo("wifi_revise");
				}
			}

			var thisDNS = "";
			for(var i=0;i<data.real.dns.length;i++){
				thisDNS += data.real.dns[i]+",";
			}
			if(thisDNS != ""){
				thisDNS = thisDNS.substring(0,thisDNS.length-1);
			}
			$("#dnsAddress").text(thisDNS);
			$("#wirelessName").text(data.real.wifiname);
			$("#wirelessNum").text(data.real.wifinum);
			$(".connect_num").text(data.real.wifinum);
			
			//获取当前版本信息
			if(data.real.curversion == ""){
				$(".current_version").text("版本获取中...");
			}else{
				$(".current_version").text(data.real.curversion);
			}



			if(refresh){
				$("#"+data.wantype).click();
				
				$(".name input").val(data.pppoe.user);
				$(".pwd input").val(data.pppoe.pass);
				//$(".dns input").val(data.pppoe.dns);
				//$(".dns input").val(data.dhcp.dns);
				
				$(".ip input").val(data.static.ip);
				$(".select_mask input").first().val(data.static.mask);
				$(".select_mask input").first().data("value",data.static.mask);
				$(".select_mask .get_mask dd").each(function(){
					if(data.static.mask == $(this).data("value")){
						$("select_mask span").first().text($(this).text());
					}
				});
				$(".gateway input").val(data.static.gw);
				$(".select_wifi span").first().text(data.wifi.ssid);
				$(".select_wifi span").first().data("encryption",data.wifi.encryption);
				if(data.wifi.encryption==0){

				}
			}
		},
		error: function(jqXHR, textStatus,errorThrown){
			//alert("请求失败，稍后请重试！");
		}
	});
}

//获取WiFi列表
function getWiFiList(){
	$.ajax({
		type: "POST",
		url: 'http://'+window.location.host+'/cgi-bin/luci/admin/wifi_scan_get?token='+token,
		success: function(data,status){
			data=JSON.parse(data);
			$(".get_wifiList").html("");
			var wifiListHtml = "";
			for(var i=0;i<data.length;i++){
				wifiListHtml += "<dd data-encryption='"+data[i].encryption+"'>"+data[i].ssid+"</dd>";
			}
			$(".get_wifiList").html(wifiListHtml);
			$(".get_wifiList dd").click(function(){
				if($(this).html() != $(this).parents('.select_wifi').find('span').html()){
					$(".pwd input").val("");
					if($(this).html() == models.wifi.ssid){
						$(".pwd input").val(models.wifi.key);
					}
					$(this).parents('.select_wifi').find('span').html($(this).html());
					$(this).parents('.select_wifi').find('span').data("encryption",$(this).data("encryption"));
					if($(this).data("encryption")==0){
						$(".wifipwd").hide();
					}else{
						$(".wifipwd").show();
					}
					$(this).parents('.select_wifi').find('dl').slideUp();

				}
			});
		},
		error: function(jqXHR, textStatus,errorThrown){
		//	alert("获取wifi列表失败，请稍后重试！");
		}
	});
}

//获取WiFi名称
function getWifiName(){
	$.ajax({
		type: "POST",
		url: 'http://'+window.location.host+'/cgi-bin/luci/admin/sys_hostname_get?token='+token,
		success: function(data,status){	
			data=JSON.parse(data);
			var wirelessName = data.hostname;
			$("#wirelessName").text(wirelessName);
			$("input[name='wifiName1']").val(wirelessName);
			if(data.wifihidden==1){
				$("input[name='hiddenssid']").attr("checked","checked");
			}

			if(data.wifipass =="N/A"){
				$("#set_pass option:first").attr("selected","selected");
			}else{
				$("#set_pass option:last").attr("selected","selected");
				$("#show_setpass").show();
				$("input[name='wifi_pass']").val(data.wifipass);
			}
		},
		error: function(jqXHR, textStatus,errorThrown){
			alert("无法获取WiFi名称，请稍后重试！");
		}
	});
}

//设置WiFi名称
function setWifiName(hidden,wifiName,pass){
	$.ajax({
		type: "POST",
		async: false,
		url: 'http://'+window.location.host+'/cgi-bin/luci/admin/sys_hostname_set?token='+token,
		data:{hostname:wifiName,action:'apply',pass:pass,hidden:hidden},
		success: function(data,status){
			data=JSON.parse(data);
			alert("设置WiFi配置成功！【提示：wifi将会自动断开】。");
			location.href="index.html?token="+token+"&sj="+Math.random();
		},
		error: function(jqXHR, textStatus,errorThrown){
			alert("修改WiFi名称失败，请稍后重试！");
		}
	});
}

//设置WiFi和登录密码
function changePWD(pwdVal_old,loginPwd,wifiPwd){
	$.ajax({
		type: "POST",
		async: false,
		url: 'http://'+window.location.host+'/cgi-bin/luci/admin/sys_password_set?token='+token,
		data:{oldpw:pwdVal_old,pw:loginPwd,wifiPwd:wifiPwd},
		success: function(data,status){

				data=JSON.parse(data);
			if(data.status==0){
				alert("原密码错误");
			}else{
				alert("设置密码成功！");
				var loginUrl ="http://" + window.location.host + "/login.html";
				window.open(loginUrl,"_self");
			}
		//	location.reload();
		},
		error: function(jqXHR, textStatus,errorThrown){
			
			alert("修改密码失败，请稍后重试！");
		}
	});
}

//获取新的版本信息
var version;
function getNewVersion(){
	$.ajax({
		type:"get",//ajax是不支持post方式的跨域
		url:"http://www.cmttihome.com:9091/wifi_home_gx/open/newFirmwareVersion?model=IJLY_410&factory=CMTT&callback=jsonpcallback",
		dataType:"jsonp",
		jsonp:"callback",
		jsonpCallback:"jsonpcallback",
		success: function(data){
			version = data;
			if(data.swver == ""){
				$(".new_version").text("版本获取中...");
			}else{
				$(".new_version").text(data.swver);
			}
			if(VerCmp($(".new_version").html(),$(".current_version").html())<0){
				$(".update p").html("已是最新固件");
				$(".update p").unbind("click");
			}
		},
		error:function(XMLHttpRequest,textStatus,errorThrown){
			alert("请求失败，请稍候重试！1");
		}
	});
}

//升级新的版本
function updateVersion(){
	$.ajax({
		type:"POST",
		url:'http://'+ window.location.host +'/cgi-bin/luci//admin/sys_upgrade',
		success: function(data){
			data=JSON.parse(data);
			$(".current_version").text("版本升级中...");
			var i = 0;
			var waitTime = setInterval(function(){
				i++;
				if(i >= 100){
					clearInterval(waitTime);
					var loginUrl ="http://" + window.location.host + "/login.html";
					window.open(loginUrl,"_self");
				}
				var rebootText = "版本升级中... " + i + "s"
				$(".current_version").text(rebootText);
			},1000);
			if(data.status == 1){
				alert("版本升级成功！");
				$(".current_version").text(version.swver);
				return;
			}else if(data.status == 2){
				alert("系统出错，没找到指定版本资源文件！");
				$(".current_version").text(version.curversion);
				return;
			}else if(data.status == 3){
				alert("请求版本资源文件失败！");
				$(".current_version").text(version.curversion);
				return;
			}else if(data.status == 4){
				alert("版本升级出错，请重新升级！");
				$(".current_version").text(version.curversion);
				return;
			}
		},
		error:function(XMLHttpRequest,textStatus,errorThrown){
			alert("请求失败，请稍候重试！2");
		}
	});
}

//恢复出厂设置
function factoryReset(){
	$.ajax({
		type:"POST",
		url:'http://'+ window.location.host +'/cgi-bin/luci//admin/sys_factory',
		success: function(data){
			data=JSON.parse(data);
			$(".recover p").text("正在初始出厂设置...");
			var i = 0;
			var waitTime = setInterval(function(){
				i++;
				if(i >= 90){
					clearInterval(waitTime);
					$(".recover p").text("已恢复出厂设置");
					var loginUrl ="http://" + window.location.host + "/login.html";
					window.open(loginUrl,"_self");
				}
				var rebootText = "正在初始出厂设置... " + i + "s"
				$(".recover p").text(rebootText);
			},1000);
		},
		error:function(XMLHttpRequest,textStatus,errorThrown){
			alert("请求失败，请稍候重试！3");
		}
	});
}

//重启路由
function reboot(){
	$.ajax({
		type:"POST",
		url:'http://'+ window.location.host +'/cgi-bin/luci//admin/sys_reboot',
		success: function(data){
			data=JSON.parse(data);
			$(".reboot p").text("正在重启中...");
			var i = 0;
			var waitTime = setInterval(function(){
				i++;
				if(i >= 90){
					clearInterval(waitTime);
					$(".reboot p").text("路由器已重启");
					var loginUrl ="http://" + window.location.host + "/login.html";
					window.open(loginUrl,"_self");
				}
				var rebootText = "正在重启中... " + i + "s";
				$(".reboot p").text(rebootText);
			},1000);
		},
		error:function(XMLHttpRequest,textStatus,errorThrown){
			alert("请求失败，请稍候重试！4");
		}
	});
}

//获取连接的设备的参数
function getDeviceArguments(){
	var deviceList = "";
	$.ajax({
		type:"POST",
		url:'http://'+ window.location.host +'/cgi-bin/luci//admin/device_relay_get',
		success: function(data){
			data=JSON.parse(data);
			for(var i = 0;i < data.devicelist.length;i++){
				var deviceImg = "";
				var youxiang = "";
				var deviceName = data.devicelist[i].device;
				/*if(deviceName.indexOf("administrator") > -1){
					 deviceImg = $(".show_icon img").attr("images/pc_icon.png");
				}else{
					 deviceImg = $(".show_icon img").attr("images/phone_icon.png");
				}*/
				if(data.devicelist[i].type==1){
					youxiang="有线";
				}else{
					youxiang="WIFI 2.4G";
				}
				var minutes = parseInt(data.devicelist[i].time);
				var toHour = Math.floor(minutes/60) + "小时" + (minutes%60) + "分";
				var toMinutes = minutes;
				minutes > 60 ? minutes=toHour : minutes=toMinutes
//<h2>"+ data.devicelist[i].device +"</h2>\
				deviceList += "<li>\
								<div class='show_icon'><img src='images/menu3.png'></div>\
								<div class='terminal_info'>\
									<div class='info_type'>\
										<a href='javascript:void(0);'><label>已连接：</label><span></span><span></span></a>&nbsp;|&nbsp;\
										<a href='javascript:void(0);'><label>连接类型：</label><span>"+ youxiang +"</span></a>&nbsp;|&nbsp;\
										<a href='javascript:void(0);'><label>IP地址：</label><span>"+ data.devicelist[i].ip +"</span></a>&nbsp;|&nbsp;\
										<a href='javascript:void(0);'><label>MAC地址：</label><span>"+ data.devicelist[i].mac +"</span></a>\
									</div>\
								</div>\
							</li>"
			}
			$("#list3").html(deviceList);
		},
		error: function(XMLHttpRequest,textStatus){
			//alert("请求失败，请稍候重试！");
		}
	});
}

//获取WiFi连接端口
function getPortVal(){
	$.ajax({
		type: "POST",
		url: 'http://'+window.location.host+'/cgi-bin/luci/admin/wan_ip_get',
		async:false,
		success: function(data,status){
			data=JSON.parse(data);
			var obj = data;
			if(obj.real.port.port1 == "1"){
				$(".port1").show();
				$(".port1").css("display","inline-block");
			}else{
				$(".port1").hide();
			} 
			
			if(obj.real.port.port2 == "1"){
				$(".port2").show();
				$(".port2").css("display","inline-block");
			}else{
				$(".port2").hide();
			} 
			
			if(obj.real.port.port3 == "1"){
				$(".port3").show();
				$(".port3").css("display","inline-block");
			}else{
				$(".port3").hide();
			} 
			
			if(obj.real.port.port4 == "1"){
				$(".port4").show();
				$(".port4").css("display","inline-block");
			}else{
				$(".port4").hide();
			} 
			
			if(obj.real.port.port5 == "1"){
				$(".port5").show();
				$(".port5").css("display","inline-block");
			}else{
				$(".port5").hide();
			} 
						
		},
		error: function(XMLHttpRequest,textStatus){
			//alert("请求失败，请稍候重试！");
		}
	});
	setTimeout("getPortVal()",60000);
}



function VerCmp(verstr1, verstr2)
{
	var subvers1 = verstr1.split(".", 4);
	var subvers2 = verstr2.split(".", 4);
	subvers1[0] = subvers1[0].replace(/v/, "");
	subvers2[0] = subvers2[0].replace(/v/, "");
	for(var i = 0; i < subvers1.length && i < subvers2.length; i++)
	{
		if((0 + subvers1[i]) > (0 + subvers2[i]))
			return 1;
		else if((0 + subvers1[i]) < (0 + subvers2[i]))
			return -1;
	}
	if(subvers1.length > subvers2.length)
		return 1;
	else if(subvers1.length < subvers2.length)
		return -1;
	else
		return 0;
}
