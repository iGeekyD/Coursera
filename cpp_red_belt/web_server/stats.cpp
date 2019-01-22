#include "stats.h"

unordered_set<string_view> METHODS = {"GET", "POST", "DELETE", "PUT",
                                      "UNKNOWN"};
unordered_set<string_view> URI = {"/",       "/order", "/product",
                                  "/basket", "/help",  "unknown"};

string_view getMethod(string_view line) {
  auto begin = line.find_first_not_of(" ");
  auto end = line.find_first_of(" ", begin);
  return line.substr(begin, end - begin);
}

string_view getUri(string_view line) {
  auto begin = line.find_first_of("/");
  auto end = line.find_first_of(" ", begin);
  return line.substr(begin, end - begin);
}

string_view getProto(string_view line) {
  auto begin = line.find_first_of("/");
  begin = line.find_first_of(" ", begin);
  return line.substr(begin + 1, line.size() - begin);
}

HttpRequest ParseRequest(string_view line) {
  HttpRequest req;
  req.method = getMethod(line);
  req.uri = getUri(line);
  req.protocol = getProto(line);
  return req;
}

const map<string_view, int> &Stats::GetMethodStats() const { return methodMap; }

const map<string_view, int> &Stats::GetUriStats() const { return uriMap; }

void Stats::AddMethod(string_view method) {
  auto it = METHODS.find(method);
  if (it != METHODS.end())
    methodMap[*it]++;
  else {
    it = METHODS.find("UNKNOWN");
    methodMap[*it]++;
  }
}

void Stats::AddUri(string_view uri) {
  auto it = URI.find(uri);
  if (it != URI.end())
    uriMap[*it]++;
  else {
    it = URI.find("unknown");
    uriMap[*it]++;
  }
}

Stats::Stats() {
  methodMap = {
      {"GET", 0}, {"POST", 0}, {"DELETE", 0}, {"PUT", 0}, {"UNKNOWN", 0}};
  uriMap = {{"/", 0},       {"/order", 0}, {"/product", 0},
            {"/basket", 0}, {"/help", 0},  {"unknown", 0}};
}