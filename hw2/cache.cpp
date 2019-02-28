#include "cache.h"

void Cache::init_cache(std::string request_input) { request = request_input; }
bool Cache::correct_status() {
  string recv;
  recv.resize(response.size());
  recv.assign(response.begin(), response.end());
  if (recv.find("200 OK") != string::npos) {
    return true;
  }
  return false;
}

string Cache::get_expire() {
  string recv;
  string time;
  recv.resize(response.size());
  recv.assign(response.begin(), response.end());

  if (recv.find("Expires:") != string::npos) {
    recv = recv.substr(recv.find("Expires:") + 9);
    time = recv.substr(0, recv.find("\r\n"));
  }
  return time;
}

string Cache::get_maxage() {
  string recv;
  string time;
  recv.resize(response.size());
  recv.assign(response.begin(), response.end());
  if (recv.find("max-age=") != string::npos) {
    recv = recv.substr(recv.find("max-age=") + 8);
    int i = 0;
    vector<char> temp;
    while (recv[i] != '\r') {
      if (isdigit(recv[i])) {
        temp.push_back(recv[i]);
      }
      i++;
    }
    time.resize(temp.size());
    time.assign(temp.begin(), temp.end());
  }
  return time;
}

string Cache::get_date() {
  string recv;
  string date;
  recv.resize(response.size());
  recv.assign(response.begin(), response.end());

  if (recv.find("Date:") != string::npos) {
    recv = recv.substr(recv.find("Date:") + 6);
    date = recv.substr(0, recv.find("\r\n"));
  }
  return date;
}
string Cache::get_ETag() {
  string recv;
  string control;
  recv.resize(response.size());
  recv.assign(response.begin(), response.end());
  if (recv.find("Cache-Control: ") != string::npos) {
    recv = recv.substr(recv.find("Cache-Control: "));
    control = recv.substr(0, recv.find("\r\n"));
  }
  return control;
}

string Cache::get_modified() {
  std::string recv;
  std::string modified_t;
  recv.resize(response.size());
  recv.assign(response.begin(), response.end());

  if (recv.find("Last-Modified:") != string::npos) {
    recv = recv.substr(recv.find("Last-Modified:") + 15);
    modified_t = recv.substr(0, recv.find("\r\n"));
  }
  return modified_t;
}

void Cache::savecache(unordered_map<string, finalResponse> cache) {

  time_t maxageint;
  cache[request].data = response;
  if (correct_status() == false) {
    return;
  }
  if ((response.find("no-cache") != string::npos) ||
      (response.find("no-transform") != string::npos) ||
      (response.find("no-store") != string::npos)) {
    ofstream file;
    file.open("/var/log/erss/proxy.log", ios::app);
    file << id << ": "
         << "not cacheable because no-" << endl;
    file.close();
    if (response.find("no-cache") != string::npos) {
      cache[request].nocache = true;
    }
    if (response.find("no-transform") != string::npos) {
      cache[request].notransform = true;
    }
    if (response.find("no-store") != string::npos) {
      cache[request].nostore = true;
    }
    return;
  }
  // time_t rawtime;
  // struct tm timeinfo;
  // time(&rawtime);
  // timeinfo=localtime(&rawtime);
  // string str;
  // Ctime system_t;

  if (response.find("max-age=") != string::npos) {
    string recv = response.substr(response.find("max-age=") + 8);
    int i = 0;
    vector<char> temp;
    while (recv[i] != '\r') {
      if (isdigit(recv[i])) {
        temp.push_back(recv[i]);
      }
      i++;
    }
    string maxage;
    // string inter; // string expiretime;
    // time_t current = time(0);
    maxage.resize(temp.size());
    maxage.assign(temp.begin(), temp.end());
    // response.maxage = maxage;
    maxageint = atoi(maxage.c_str());
    cache[request].maxage = maxageint;
    if (maxageint < Age) {
      if (expiretime.size() != 0) {
        ofstream file;
        const char *expire_t = expiretime.c_str();
        struct tm time;
        strptime(expire_t, "%a %b %d %Y %H:%M:%S UTC",
                 &time); // alright or has to be like tutorial???
        file.open("/var/log/erss/proxy.log", ios::app);
        file << id << ": "
             << "cached, expires at " << asctime(&time) << endl;
        file.close();
        cache[request].Age = Age;
        return;
      }
      chrono::system_clock::time_point now = chrono::system_clock::now();
      // str = system_t.format("%a %b %d %Y %H:%M:%S UTC");
      time_t tt;
      tt = chrono::system_clock::to_time_t(now);
      ofstream file;
      file.open("/var/log/erss/proxy.log", ios::app);
      file << id << ": "
           << "cached, expires at " << ctime(&tt) << endl;
      return;
    }
  }
  if ((response.find("must-revalidate") != string::npos) ||
      ((response.find("proxy-revalidate") != string::npos) &&
       (cache[request].priv == true))) {
    ofstream file;
    file.open("/var/log/erss/proxy.log", ios::app);
    file << id << ": "
         << "cached, but requires re-validation" << endl;
    if (response.find("must-revalidate") != string::npos) {
      cache[request].revalidation = true;
    }
    if (response.find("proxy-revalidate") != string::npos) {
      cache[request].proxyrev = true;
    }
    return;
  }
}

bool Cache::checkcache(unordered_map<string, finalResponse> cache) {

  if (cache.find(request) != cache.end()) {
    if (cache[request].priv) {
      // do not care do not forward
      return false;
    } else {
      if ((cache[request].data.find("no-cache") != string::npos) ||
          (cache[request].data.find("no-transform") != string::npos) ||
          (cache[request].data.find("no-store") != string::npos)) {
        ofstream file;
        file.open("/var/log/erss/proxy.log", ios::app);
        file << id << ": "
             << "in cache, requires validation" << endl;
        file.close();
        if (response.find("no-cache") != string::npos) {
          cache[request].nocache = true;
        }
        if (response.find("no-transform") != string::npos) {
          cache[request].notransform = true;
        }
        if (response.find("no-store") != string::npos) {
          cache[request].nostore = true;
        }
      }
      if (cache[request].data.find("must-revalidate") ||
          cache[request].data.find("proxy-revalidate")) {
        ofstream file;
        file.open("/var/log/erss/proxy.log", ios::app);
        file << id << ": "
             << "in cache, requires validation" << endl;
        return true;
      }

      if (cache[request].maxage < Age) {
        if (expiretime.size() != 0) {
          ofstream file;
          const char *expire_t = expiretime.c_str();
          struct tm time;
          strptime(expire_t, "%a %b %d %Y %H:%M:%S UTC",
                   &time); // alright or has to be like tutorial???
          file.open("/var/log/erss/proxy.log", ios::app);
          file << id << ": "
               << "in cache, but expired at " << asctime(&time) << endl;
          file.close();
          const char *message = cache[request].data.data();
          send(fd, message, size, 0);
          return false;
        }
        //   sys_t = GetCurrentTime();
        // str1 = sys_t.format("%a %b %d %Y %H:%M:%S UTC");
        time_t tt;
        chrono::system_clock::time_point today = chrono::system_clock::now();
        tt = chrono::system_clock::to_time_t(today);
        ofstream file;
        file.open("/var/log/erss/proxy.log", ios::app);
        file << id << ": "
             << "in cache, but expired at " << tt << endl;
        const char *message = cache[request].data.data();
        send(fd, message, size, 0);
        return false;
      }
      ofstream file;
      file.open("/var/log/erss/proxy.log", ios::app);
      file << id << ": "
           << "in cache,valid" << endl;
      const char *message = cache[request].data.data();
      send(fd, message, size, 0);
      return false;
    }
  }
  ofstream file;
  file.open("/var/log/erss/proxy.log", ios::app);
  file << id << ": "
       << "not in cache" << endl;
  return true;
}

void Cache::init_cache() {}
