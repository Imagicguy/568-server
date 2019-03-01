#include "cache.h"

void Cache::init_checkcache(string request_input, int id_input,
                            int clientfd_input, int serverfd_input,
                            unordered_map<string, finalResponse> cache_input) {
  cache = cache_input;
  request = request_input;
  id = id_input;
  clientfd = clientfd_input;
  serverfd = serverfd_input;
}

void Cache::init_savecache(string response_input, string request_input,
                           int id_input,
                           unordered_map<string, finalResponse> cache_input) {

  cache = cache_input;
  request = request_input;
  response = response_input;
  id = id_input;
}
bool Cache::correct_status(string response) {
  if (response.find("200 OK") != string::npos) {
    return true;
  }
  return false;
}

string Cache::get_expire(string response) {
  string time;
  if (response.find("Expires:") != string::npos) {
    response = response.substr(response.find("Expires:") + 9);
    time = response.substr(0, response.find("\r\n"));
  }
  return time;
}

string Cache::get_maxage(string response) {
  string time;
  if (response.find("max-age=") != string::npos) {
    response = response.substr(response.find("max-age=") + 8);
    int i = 0;
    vector<char> temp;
    while (response[i] != '\r') {
      if (isdigit(response[i])) {
        temp.push_back(response[i]);
      }
      i++;
    }
    time.resize(temp.size());
    time.assign(temp.begin(), temp.end());
  }
  return time;
}

string Cache::get_Age(string response) {
  string Age;
  if (response.find("Age = ") != string::npos) {
    response = response.substr(response.find("Age = ") + 6);
    Age = response.substr(0, response.find("\r\n"));
  }
  return Age;
}

string Cache::control_sig(string response) {
  string control;
  if (response.find("Cache-Control: ") != string::npos) {
    response = response.substr(response.find("Cache-Control: ") + 15);
    control = response.substr(0, response.find("\r\n"));
  }
  return control;
}

string Cache::get_modified(string response) {
  string modified_t;
  if (response.find("Last-Modified:") != string::npos) {
    response = response.substr(response.find("Last-Modified: ") + 15);
    modified_t = response.substr(0, response.find("\r\n"));
  }
  return modified_t;
}

string Cache::get_ETag(string response) {
  string ETag;
  if (response.find("ETag: ") != string::npos) {
    response = response.substr(response.find("ETag: ") + 6);
    ETag = response.substr(0, response.find("\r\n"));
  }
  return ETag;
}

string Cache::get_Host(string request) {
  string Host;
  if (request.find("Host: ") != string::npos) {
    request = request.substr(request.find("Host: ") + 6);
    Host = request.substr(0, request.find("\r\n"));
  }
  return Host;
}
string Cache::get_firstline(string request) {
  string firstline;
  firstline = request.substr(0, request.find("\r\n"));
  return firstline;
}
void Cache::savecache() {

  time_t maxageint;
  cache[request].data = response;
  if (correct_status(response) == false) {
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
  if (response.find("max-age=") != string::npos) {
    //  string recv = response.substr(response.find("max-age=") + 8);
    // int i = 0;
    // vector<char> temp;
    // while (recv[i] != '\r') {
    // if (isdigit(recv[i])) {
    //  temp.push_back(recv[i]);
    //}
    // i++;
    //}

    string Age = get_Age(response);
    const char *agechar = Age.c_str();
    int ageint = atoi(agechar);
    string maxage = get_maxage(response);
    string expiretime = get_expire(response);
    const char *maxchar = maxage.c_str();
    maxageint = atoi(maxchar);
    cache[request].maxage = maxageint;
    if (maxageint < ageint) {
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
        cache[request].Age = ageint;
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
      file.close();
      return;
    }
    if (control_sig(response).find("private") != string::npos) {
      cache[request].priv = true;
    }
    if (control_sig(response).find("public") != string::npos) {
      cache[request].pub = true;
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
    file.close();
    return;
  }
}

void Cache::checkcache() {
  if (cache.find(request) != cache.end()) {
    if (cache[request].priv) {
      // do not care do not forward
      return;
    } else {
      if ((cache[request].data.find("no-cache") != string::npos) ||
          (cache[request].data.find("no-transform") != string::npos) ||
          (cache[request].data.find("no-store") != string::npos)) {
        ofstream file;
        file.open("/var/log/erss/proxy.log", ios::app);
        file << id << ": "
             << "in cache, requires validation" << endl;
        file.close();
        if (cache[request].data.find("no-cache") != string::npos) {
          cache[request].nocache = true;
        }
        if (cache[request].data.find("no-transform") != string::npos) {
          cache[request].notransform = true;
        }
        if (cache[request].data.find("no-store") != string::npos) {
          cache[request].nostore = true;
        }
      }

      string Age = get_Age(cache[request].data);
      const char *agechar = Age.c_str();
      int ageint = atoi(agechar);
      string expiretime = get_expire(cache[request].data);
      if (cache[request].maxage < ageint) {
        if (cache[request].data.find("must-revalidate") ||
            cache[request].data.find("proxy-revalidate")) {
          ofstream file;
          file.open("/var/log/erss/proxy.log", ios::app);
          file << id << ": "
               << "in cache, requires validation" << endl;
          file.close();
          cache.clear(); // clear map!
          const char *message = request.c_str();
          if (send(serverfd, message, strlen(message), 0) < 0) {
            // error_check(id, 2);
            close(serverfd);
            return;
          }
          // send(serverfd, message, strlen(message), 0);
          return;
        }
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
          send(clientfd, message, strlen(request.c_str()), 0);
          return;
        }
        if ((get_ETag(cache[request].data.data()).size() != 0) ||
            (get_modified(cache[request].data.data()).size() != 0)) {
          string temp1 = get_firstline(request);
          const char *message = temp1.c_str();
          send(serverfd, message, strlen(message), 0);
          string temp2 = get_Host(request);
          string temp3 = "Host: ";
          string fin = temp3 + temp2 + "\r\n";
          const char *message1 = fin.c_str();
          send(serverfd, message1, strlen(message), 0);
          if (get_ETag(cache[request].data.data()).size() != 0) {
            ofstream file;
            file.open("/var/log/erss/proxy.log", ios::app);
            file << id << ": "
                 << "NOTE ETag: " << get_ETag(cache[request].data.data())
                 << endl;
            string ET = get_ETag(cache[request].data.data());
            string temp1 = "If-None-Match: ";
            string temp2 = temp1 + ET + "\r\n";
            file.close();
            const char *message = temp2.c_str();
            size_t sz = strlen(message);
            send(serverfd, message, sz, 0);
            return;
          }
          if (get_modified(cache[request].data.data()).size() != 0) {
            ofstream file;
            file.open("/var/log/erss/proxy.log", ios::app);
            file << id << ": "
                 << "NOTE modified: "
                 << get_modified(cache[request].data.data()) << endl;
            file.close();
            string temp1 = "If-Modified-Since: ";
            string temp2 =
                temp1 + get_modified(cache[request].data.data()) + "\r\n";

            const char *message = temp2.c_str();
            size_t sz = strlen(message);
            send(serverfd, message, sz, 0);
            file.close();
            return;
          }
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
        string temp(message);
        send(clientfd, message, cache[request].data.size(), 0);
        return;
      }
      ofstream file;
      file.open("/var/log/erss/proxy.log", ios::app);
      file << id << ": "
           << "in cache,valid" << endl;
      const char *message = cache[request].data.data();
      send(clientfd, message, cache[request].data.size(), 0);
      return;
    }
  }
  ofstream file;
  file.open("/var/log/erss/proxy.log", ios::app);
  file << id << ": "
       << "not in cache" << endl;
  send(serverfd, request.c_str(), strlen(request.c_str()), 0);
  return;
}
