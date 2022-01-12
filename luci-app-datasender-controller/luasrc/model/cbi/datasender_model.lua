map = Map("datasender", "Config")

section = map:section(NamedSection, "datasender_sct", "datasender", "Section")

flag = section:option(Flag, "enable", "Enable", "Enable program")
orgId = section:option(Value, "orgId", "Organisation ID")
typeId = section:option(Value, "typeId", "Type ID")
deviceId = section:option(Value, "deviceId", "Device ID")
token = section:option(Value, "token", "Token")
token.datatype = "String"
return map
