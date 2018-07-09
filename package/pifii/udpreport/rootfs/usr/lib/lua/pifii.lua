local _M = {}
local file_wifi = "/tmp/his_wifi_list"
function split(str, delimiter)
    local fs = delimiter
    if not fs or "" == string.gsub(fs," ","") then
                fs="%s"
        end
    if str==nil or str=="" or delimiter==nil then
        return nil
    end

    local result = {}
        for match in string.gmatch((str..delimiter),"[^"..fs.."]+") do
        table.insert(result, match)
    end
    return result
end
function get_arp()
    local res = {}
    local num = 0
    local f_arp = "/proc/net/arp"
    if 0 == os.execute("test -f "..f_arp) then 
        local cmd = "grep br-lan "..f_arp.." | grep 0x2"
        local fd = io.popen(cmd, "r");
        local row = nil
        local mac = nil
        if fd then
            while true do
                local line = nil
                line = fd:read("*l")
                if not line or #line==0 then break end
                row=split(line," ")
                mac = string.upper(row[4])
                res[ mac ] = row[1]
            end
        end
        fd:close()
    end
    return res
end
function get_assoclist()
    --local ifn_t = {"wlan0","wlan0-1","wlan0-2",""}
    local res = {}
    local prefix = "ra"
    local num = 0
    local dir = "/sys/devices/virtual/net/"
    while 5 >= num  do
        ifn = prefix..num
        if 0 ~= os.execute("test -d "..dir..ifn) then break end
        local cmd = "iwinfo "..ifn.." assoclist | grep -E \".{2}:.{2}:.{2}:.{2}:.{2}:.{2}\""
        local fd = io.popen(cmd, "r");
        local row = nil
        local mac = nil
        if fd then
            while true do
                local line = nil
                line = fd:read("*l")
                if not line or #line==0 then break end
                row=split(line," ")
                mac = string.upper(row[1])
                res[ mac ] = 1
            end
        end
        fd:close()
        num = num + 1
    end
    return res
end

function get_his_wifi()
    
    local wifi_list = {}
    if 0 == os.execute("test -f "..file_wifi) then 
        local cmd = "grep -E \".{2}:.{2}:.{2}:.{2}:.{2}:.{2}\" "..file_wifi
        local fd = io.popen(cmd, "r");                                                      
        local row = nil                                                                     
        local mac = nil                                                                     
        if fd then                                                                          
            while true do                                                                   
                local line = nil                                                            
                line = fd:read("*l")                                                        
                if not line or #line==0 then break end 
                row=split(line,"|")                                                         
                for var = 1,#row,1 do
                    mac = string.upper(row[var])                                                  
                    wifi_list[mac] = 0                                                           
                end
            end                                                                             
        end                                                                                 
        fd:close()                           
    end
    return wifi_list
end
function _M.get_wifi_client() 
    local arp = get_arp()
    local wifi_list = get_his_wifi()
    local l = get_assoclist()
    local s = require "luci.tools.status"
    local leases = s.dhcp_leases()
    local ret = {}
    for k,v in pairs(l) do
        wifi_list[k] = 1
    end
    local str = "" 
    for k, v in pairs(wifi_list) do
        str = str..k.."|" 
    end
    os.execute("echo \'"..str.."\' > "..file_wifi)
    local host_l = {}
    for _, v in pairs(leases) do
        host_l[v.macaddr:upper()] = v.hostname
    end
    for k, v in pairs(arp) do
        local che = {}
        local mac = k 
        che.mac = mac
        if mac then 
            if wifi_list[mac] and 1 == wifi_list[mac] then
                che.ip = v
                che.host = host_l[mac] or ""
                che.type = "0"
                table.insert(ret,che)
            elseif not wifi_list[mac] then 
                che.ip = v
                che.host = host_l[mac] or ""
                che.type = "1"
                table.insert(ret,che)
            end
        end
    end
    
    return ret
end 

return _M
--[[
local wl = get_wifi_client()
require("luci.json")                                                                              
local para=luci.json.encode(wl)                                                                  
print(para) 
--]]
