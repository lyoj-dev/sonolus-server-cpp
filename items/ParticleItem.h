#ifndef PARTICLEITEM_H
#define PARTICLEITEM_H

using namespace std;

class ParticleItem {
    public:

    int id;
    string name;
    int version = 1;
    string title;
    string subtitle;
    string author;
    SRL<ParticleThumbnail> thumbnail;
    SRL<ParticleData> data;
    SRL<ParticleTexture> texture;

    ParticleItem(){}
    ParticleItem(int id, string name, string title, string subtitle, string author,
        SRL<ParticleThumbnail> thumbnail, SRL<ParticleData> data, SRL<ParticleTexture> texture):
        id(id), name(name), title(title), subtitle(subtitle), author(author),
        thumbnail(thumbnail), data(data), texture(texture){}

    Json::Value toJsonObject() {
        Json::Value res;
        res["name"] = name;
        res["version"] = version;
        res["title"] = title;
        res["subtitle"] = subtitle;
        res["author"] = author;
        res["thumbnail"] = thumbnail.toJsonObject();
        res["data"] = data.toJsonObject();
        res["texture"] = texture.toJsonObject();
        return res;
    }

    argvar fetchParamList() {
        argvar args;
        args["name"] = name;
        args["version"] = to_string(version);
        args["title"] = title;
        args["subtitle"] = subtitle;
        args["author"] = author;
        args["thumbnail"] = thumbnail.url;
        args["data"] = data.url;
        args["texture"] = texture.url;
        args["url"] = "/particles/" + name;
        args["sonolus.url"] = "sonolus:" + appConfig["server.rootUrl"].asString() + "/particles/" + name;
        return args;
    }

    H toHTMLObject() {
        string buffer = readFile("./web/html/components/particles.html");
        buffer = str_replace(buffer, fetchParamList());
        return H(buffer);
    }
};

int particleNumber(string filter) {
    string sql = "SELECT COUNT(*) AS sum FROM Particle";
    if (filter != "") sql += " WHERE (" + filter + ")";
    mysqld res = mysqli_query(mysql, sql.c_str());
    return atoi(res[0]["sum"].c_str());
}

Section<ParticleItem> particleList(string filter, int st = 1, int en = 20) {
    // ??????????????????
    int pageCount = ceil(1.0 * particleNumber(filter) / 20);

    // ????????????
    string sql = "SELECT * FROM Particle";
    if (filter != "") sql += " WHERE (" + filter + ")";
    sql += " ORDER BY id DESC LIMIT " + to_string(st - 1) + ", " + to_string(en - st + 1);
    mysqld res = mysqli_query(mysql, sql.c_str());
    Section<ParticleItem> list = Section<ParticleItem>(pageCount, ParticleSearch);
    for (int i = 0; i < res.size(); i++) {
        ParticleItem data = ParticleItem(
            atoi(res[i]["id"].c_str()),
            res[i]["name"],
            res[i]["title"],
            res[i]["subtitle"],
            res[i]["author"],
            SRL<ParticleThumbnail>(res[i]["thumbnail"], "/data/" + res[i]["thumbnail"]),
            SRL<ParticleData>(res[i]["data"], "/data/" + res[i]["data"]),
            SRL<ParticleTexture>(res[i]["texture"], "/data/" + res[i]["texture"])
        ); list.append(data);
    } return list;
}

string particleFilter(argvar arg) {
    string filter = "";
    if (arg["keywords"] != "") filter = "title like \"%" + str_replace("\"", "\\\"", urldecode(arg["keywords"])) + "%\"";
    return filter;
}

#endif