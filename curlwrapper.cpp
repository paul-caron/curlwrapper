#include <iostream>
#include <curl/curl.h>

class Request{
public:
    enum Method{
        GET, POST, PUT
    };
    static int instances;
    Request(){
        if(instances++ == 0)
        curl_global_init(CURL_GLOBAL_ALL);
        
        curlHandle = curl_easy_init();
        
        curl_easy_setopt(curlHandle, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/74.0.3729.169 Safari/537.36");
    }
    ~Request(){
        clean();
        if(--instances == 0){
            curl_global_cleanup();
        }
        curl_slist_free_all(list);
        list = NULL;
    }
    void setMethod(Method m){
        method = m;
        if(method == Method::GET){
            curl_easy_setopt( curlHandle, 
                              CURLOPT_HTTPGET, 
                              1L );
        }else if(method == Method::POST){
            curl_easy_setopt( curlHandle,
                              CURLOPT_POST,
                              1L );
        }else if(method == Method::PUT){
            curl_easy_setopt( curlHandle,
                              CURLOPT_PUT,
                              1L );
        }
    }
    void setURL(std::string URL){
        url = URL;
        std::string s = url;
        if(args != ""){
            s = s + "?" + args;
        }
        curl_easy_setopt( curlHandle,
                          CURLOPT_URL,
                          s.c_str() );
    }
    void send(){
        curl_easy_perform( curlHandle );
    }
    void resetAll(){
        curl_easy_reset( curlHandle );
        args = "";
        curl_slist_free_all(list);
        list = curl_slist_append(NULL,"");
    }
    void addHeader(std::string header){
        list = curl_slist_append(list, header.c_str());
        curl_easy_setopt( curlHandle,
                          CURLOPT_HTTPHEADER,
                          list );
    }
    void addArg(std::string arg){
        if(args=="")
        args = arg;
        else
        args += "&"+arg;
        setURL(url);
    }
private:
    void clean(){
        curl_easy_cleanup( curlHandle );
    }
    Method method;
    std::string url;
    std::string args="";
    CURL * curlHandle;
    struct curl_slist *list = NULL;//headers
};

int Request::instances = 0;

using namespace std;
    
int main(){
    Request request;
    request.resetAll();
    request.setMethod(Request::Method::GET);
    request.setURL("https://httpbin.org/get");
    request.addHeader("Myheader: 42");
    request.addHeader("Myotherheader: 24");
    request.addHeader("Authorization: token");
    request.addArg("query=banana");
    request.addArg("lang=en");
    request.send();
    
    request.resetAll();
    request.setMethod(Request::Method::GET);
    request.setURL("https://httpbin.org/get");
    request.addHeader("Authorization: token");
    request.send();
    return 0;
}
