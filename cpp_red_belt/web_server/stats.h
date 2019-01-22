#pragma once

#include "http_request.h"

#include <map>
#include <string>
#include <string_view>
#include <unordered_set>


using namespace std;

class Stats {
public:
  Stats();
  void AddMethod(string_view method);
  void AddUri(string_view uri);
  const map<string_view, int> &GetMethodStats() const;
  const map<string_view, int> &GetUriStats() const;

private:
  map<string_view, int> methodMap;
  map<string_view, int> uriMap;
};

HttpRequest ParseRequest(string_view line);
