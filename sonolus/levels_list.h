using namespace std;

auto sonolus_levels_list = [](client_conn conn, http_request request, param argv){
    auto $_GET = getParam(request);
    string filter = "";
    if ($_GET["keywords"] != "") filter = "title like \"%" + str_replace("\"", "\\\"", $_GET["keywords"]) + "%\"";
    Section<LevelItem> res = levelList(filter);
    Json::Value val = res.toJsonObject();
    putRequest(conn, 200, __api_default_response);
    send(conn, json_encode(val));
    exitRequest(conn);
};