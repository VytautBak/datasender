module("luci.controller.datasender_controller", package.seeall)

function index()
	entry({"admin", "services", "Datasender"}, cbi("datasender_model"), _("Datasender"),105)
end
